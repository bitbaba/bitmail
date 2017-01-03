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
	
	public boolean Connect(String host, int port)
	{
		String vmVersion = System.getProperty("java.specification.version");
		System.out.println(vmVersion);
		
		Socket s = null;
		
		try {
			s = new Socket(host, port);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		TlsClientProtocol protocol = null;
		
		try {
			protocol = new TlsClientProtocol(s.getInputStream(), s.getOutputStream(), new SecureRandom());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
				
		MockTlsClient client = new MockTlsClient(null);
		
        try {
        	protocol.connect(client);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        

        BufferedReader is = new BufferedReader(new InputStreamReader(protocol.getInputStream()));
        OutputStream os = protocol.getOutputStream();

        try {
			//os.write("GET / HTTP/1.1\r\n\r\n".getBytes());
			os.write("HELO \r\n\r\n".getBytes());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        String resp = "";
        try {
        	String line = null;
			while ((line = is.readLine()) != null)
			{
			    resp += line;
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        try {
			is.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        
        System.out.println(resp);
		
		return false;
	}
	
	public boolean SendMail(String to, String content) 
	{
		Properties properties = new Properties();
		
		properties.put("mail.smtps.ssl.enable", "true");
		properties.put("mail.smtps.host", smtp_);
		properties.put("mail.smtps.auth", "true");
		//properties.put("mail.smtps.ssl.trust", "*");		
		properties.put("mail.smtps.ssl.socketFactory.class", "com.bitbaba.TSLSocketConnectionFactory");	// use SSL in JSSE instead of default socket factory	
		properties.put("mail.smtps.socketFactory.fallback", "false");// process SSL-only request
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
	
	public static String GetHostFromEmailAddress(String email)
	{
		if (email.indexOf("@") == -1){
			return "";
		}
		return email.substring(email.indexOf("@")+1);
	}	
}


