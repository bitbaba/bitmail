/**
 * 
 */
package com.bitbaba;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.security.Security;
import java.security.cert.CertificateException;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Properties;
import java.util.Vector;

import javax.mail.Address;
import javax.mail.Authenticator;
import javax.mail.Flags;
import javax.mail.Flags.Flag;
import javax.mail.Folder;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.NoSuchProviderException;
import javax.mail.PasswordAuthentication;
import javax.mail.Provider;
import javax.mail.Session;
import javax.mail.Store;
import javax.mail.Transport;
import javax.mail.URLName;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import javax.mail.search.FlagTerm;
import javax.net.SocketFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLEngine;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;

import org.bouncycastle.asn1.pkcs.RSAPrivateKey;
import org.bouncycastle.crypto.digests.SHA256Digest;
import org.bouncycastle.crypto.params.AsymmetricKeyParameter;
import org.bouncycastle.crypto.params.RSAPrivateCrtKeyParameters;
import org.bouncycastle.crypto.tls.TlsClient;
import org.bouncycastle.crypto.tls.TlsClientProtocol;
import org.bouncycastle.crypto.util.PrivateKeyFactory;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.jsse.provider.BouncyCastleJsseProvider;
import org.bouncycastle.util.encoders.Hex;
import org.bouncycastle.util.io.pem.PemObject;
import org.bouncycastle.util.io.pem.PemReader;
import org.bouncycastle.crypto.tls.AlertDescription;
import org.bouncycastle.crypto.tls.AlertLevel;
import org.bouncycastle.crypto.tls.Certificate;
import org.bouncycastle.crypto.tls.CipherSuite;
import org.bouncycastle.crypto.tls.DefaultTlsClient;
import org.bouncycastle.crypto.tls.ServerOnlyTlsAuthentication;
import org.bouncycastle.crypto.tls.TlsAuthentication;
import org.bouncycastle.crypto.tls.TlsFatalAlert;
import org.bouncycastle.crypto.tls.TlsKeyExchange;


import com.sun.org.apache.xml.internal.security.utils.Base64;


import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;

import org.bouncycastle.crypto.tls.CertificateRequest;
import org.bouncycastle.crypto.tls.DefaultTlsClient;
import org.bouncycastle.crypto.tls.TlsAuthentication;
import org.bouncycastle.crypto.tls.TlsClientProtocol;
import org.bouncycastle.crypto.tls.TlsCredentials;

/**
 * @author Administrator
 *
 */
public class EMailClient {

	private static int POP3_SSL_DEFAULT_PORT = 995;
	private static int SMTP_SSL_DEFAULT_PORT = 465; // or 994
	private static int IMAP_SSL_DEFAULT_PORT = 993;
	
	
	private String smtp_ = null;
	private String imap_ = null;
	
	private String login_ = null;
	private String password_ = null;
	
	/**
	 * 
	 * @param login, usually the email address
	 * @param password, secret to login into smtp/imap server
	 */
	public EMailClient(String login, String password){
		login_ = login;
		if (login_.indexOf("@") != -1){
			smtp_ = "smtp." + GetHostFromEmailAddress(login_);
			imap_ = "imap." + GetHostFromEmailAddress(login_);
		}
		password_ = password;
	}
		
	public boolean Send(String to, String content) 
	{
		Properties properties = new Properties();
		
		properties.put("mail.smtps.ssl.enable", "true");
		properties.put("mail.smtps.host", smtp_);
		properties.put("mail.smtps.auth", "true");
		properties.put("mail.smtps.ssl.socketFactory.class", "com.bitbaba.TSLSocketConnectionFactory");		
		properties.put("mail.smtps.socketFactory.fallback", "false");
		properties.put("mail.smtps.ssl.socketFactory.port", SMTP_SSL_DEFAULT_PORT);

		Session session = Session.getInstance(properties);		
			
		MimeMessage message = new MimeMessage(session);
		try {
			message.setFrom(new InternetAddress(login_));
			message.setRecipient(MimeMessage.RecipientType.TO, new InternetAddress(to));
			message.setSubject("BitMail");
			message.setSentDate(new Date());
			message.setText(content);
			message.saveChanges();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Transport trans = null;
		try {
			trans = session.getTransport("smtps");
		} catch (NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}		

		try {				
			trans.connect(smtp_, SMTP_SSL_DEFAULT_PORT, login_, password_);			
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}		
		
		try {
			trans.sendMessage(message, message.getAllRecipients());	
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			trans.close();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}
	
	
	public boolean Receive() 
	{
		Properties properties = new Properties();
		
		properties.put("mail.imaps.ssl.enable", "true");
		properties.put("mail.imaps.host", imap_);
		properties.put("mail.imaps.auth", "true");
		properties.put("mail.imaps.ssl.socketFactory.class", "com.bitbaba.TSLSocketConnectionFactory");		
		properties.put("mail.imaps.socketFactory.fallback", "false");
		properties.put("mail.imaps.ssl.socketFactory.port", IMAP_SSL_DEFAULT_PORT);

		Session session = Session.getInstance(properties);	
		
		Store store = null;
		try {
			store = session.getStore("imaps");
		} catch (NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}		

		try {				
			store.connect(imap_, IMAP_SSL_DEFAULT_PORT, login_, password_);			
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}		
		
		Folder inbox = null;
		try {
			inbox = store.getFolder("INBOX");
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {
			inbox.open(Folder.READ_WRITE);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		Message [] messages = null;
		
		try {
			FlagTerm flagTerm = new FlagTerm(new Flags(Flags.Flag.SEEN), false);			
			messages = inbox.search(flagTerm);
		} catch (MessagingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		for (int i = 0; i < messages.length; ++i){
			Message elt = messages[i];
			System.out.println("Msgno: " + elt.getMessageNumber());
			try {
				System.out.println("Subject: " + elt.getSubject());
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				System.out.println("Flags: " + elt.getFlags().toString());
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			continue; 
			
			/*
			try {
				System.out.println("Subject: " + elt.getSubject());
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				System.out.println("ContentType: " + elt.getContentType());
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				System.out.println("Content: " + elt.getContent());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (MessagingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			*/
		}
		
		try {
			inbox.close(true);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		try {
			store.close();
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		

		return true;
	}
	
	public static String GetHostFromEmailAddress(String email)
	{
		if (email.indexOf("@") == -1){
			return "";
		}
		return email.substring(email.indexOf("@")+1);
	}	
}


