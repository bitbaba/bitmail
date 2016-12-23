/**
 * 
 */
package com.bitbaba;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
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
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLEngine;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;

import org.bouncycastle.asn1.pkcs.RSAPrivateKey;
import org.bouncycastle.crypto.digests.SHA256Digest;
import org.bouncycastle.crypto.params.AsymmetricKeyParameter;
import org.bouncycastle.crypto.params.RSAPrivateCrtKeyParameters;
import org.bouncycastle.crypto.tls.AlertDescription;
import org.bouncycastle.crypto.tls.AlertLevel;
import org.bouncycastle.crypto.tls.Certificate;
import org.bouncycastle.crypto.tls.CertificateRequest;
import org.bouncycastle.crypto.tls.ClientCertificateType;
import org.bouncycastle.crypto.tls.DefaultTlsAgreementCredentials;
import org.bouncycastle.crypto.tls.DefaultTlsClient;
import org.bouncycastle.crypto.tls.DefaultTlsEncryptionCredentials;
import org.bouncycastle.crypto.tls.DefaultTlsSignerCredentials;
import org.bouncycastle.crypto.tls.MaxFragmentLength;
import org.bouncycastle.crypto.tls.ProtocolVersion;
import org.bouncycastle.crypto.tls.SignatureAlgorithm;
import org.bouncycastle.crypto.tls.SignatureAndHashAlgorithm;
import org.bouncycastle.crypto.tls.TlsAgreementCredentials;
import org.bouncycastle.crypto.tls.TlsAuthentication;
import org.bouncycastle.crypto.tls.TlsClient;
import org.bouncycastle.crypto.tls.TlsClientProtocol;
import org.bouncycastle.crypto.tls.TlsContext;
import org.bouncycastle.crypto.tls.TlsCredentials;
import org.bouncycastle.crypto.tls.TlsEncryptionCredentials;
import org.bouncycastle.crypto.tls.TlsExtensionsUtils;
import org.bouncycastle.crypto.tls.TlsProtocol;
import org.bouncycastle.crypto.tls.TlsSession;
import org.bouncycastle.crypto.tls.TlsSignerCredentials;
import org.bouncycastle.crypto.util.PrivateKeyFactory;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.jsse.provider.BouncyCastleJsseProvider;
import org.bouncycastle.util.encoders.Hex;
import org.bouncycastle.util.io.pem.PemObject;
import org.bouncycastle.util.io.pem.PemReader;

import com.sun.org.apache.xml.internal.security.utils.Base64;

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
		System.setProperty("javax.net.debug", "all");
		//System.setProperty("javax.net.debug", "ssl:handshake:verbose");
		//System.setProperty("jdk.tls.client.protocols", "TLSv1.2");
		
        if (Security.getProvider(BouncyCastleJsseProvider.PROVIDER_NAME) == null)
        {
            Security.addProvider(new BouncyCastleJsseProvider());
        }
        
        /* */
        TrustManagerFactory trustMgrFact = null;
		try {
			trustMgrFact = TrustManagerFactory.getInstance("X509",
			        BouncyCastleJsseProvider.PROVIDER_NAME);
		} catch (NoSuchAlgorithmException e3) {
			// TODO Auto-generated catch block
			e3.printStackTrace();
		} catch (java.security.NoSuchProviderException e3) {
			// TODO Auto-generated catch block
			e3.printStackTrace();
		}

        KeyStore ts = null;
		try {
			ts = KeyStore.getInstance("JKS");
		} catch (KeyStoreException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
        try {
			ts.load(null, null);
		} catch (NoSuchAlgorithmException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		} catch (CertificateException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
        try {
			trustMgrFact.init(ts);
		} catch (KeyStoreException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
       
        
        SSLContext clientContext = null;
        try {
			clientContext = SSLContext.getInstance("TLS", BouncyCastleJsseProvider.PROVIDER_NAME);
		} catch (NoSuchAlgorithmException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (java.security.NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

        try {
			clientContext.init(null, trustMgrFact.getTrustManagers(),
			        SecureRandom.getInstance("DEFAULT", BouncyCastleProvider.PROVIDER_NAME));
		} catch (KeyManagementException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (NoSuchAlgorithmException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (java.security.NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		Properties properties = new Properties();
		properties.put("mail.smtps.ssl.enable", "true");
		properties.put("mail.smtps.host", smtp_);
		properties.put("mail.smtps.auth", "true");
		properties.put("mail.smtps.ssl.trust", "*");
		
		//properties.put("mail.smtps.ssl.socketFactory.class", "javax.net.ssl.SSLSocketFactory");	// use SSL in JSSE instead of default socket factory	
		properties.put("mail.smtps.ssl.socketFactory", clientContext.getSocketFactory());	// use SSL in JSSE instead of default socket factory	
		properties.put("mail.smtps.socketFactory.fallback", "true");// process SSL-only request
		properties.put("mail.smtps.ssl.socketFactory.port", SMTP_SSL_DEFAULT_PORT);
		//properties.put("mail.smtps.ssl.ciphersuites", "TLS_RSA_WITH_RC4_128_SHA");
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
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {				
			trans.connect(smtp_, login_, password_);
		} catch (MessagingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}		
		
		try {
			Transport.send(message);
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

class TSLSocketConnectionFactory extends SSLSocketFactory {  
    @Override
    public Socket createSocket(Socket socket, final String host, int port, boolean arg3) throws IOException {
        if (socket == null) {
            socket = new Socket();
        }
        if (!socket.isConnected()) {
            socket.connect(new InetSocketAddress(host, port));
        }

        TlsClientProtocol tlsClientProtocol = new TlsClientProtocol(socket.getInputStream(), socket.getOutputStream(), new SecureRandom());
        return null;//_createSSLSocket(host, tlsClientProtocol);
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SOCKET FACTORY  METHODS  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public String[] getDefaultCipherSuites() {      
        return null;
    }

    @Override
    public String[] getSupportedCipherSuites(){ 
        return null;
    }

    @Override
    public Socket createSocket(String host, int port) throws IOException,UnknownHostException{  
        return null;
    }

    @Override
    public Socket createSocket(String host, int port, InetAddress localHost,
            int localPort) throws IOException, UnknownHostException {   
        return null;
    }

	@Override
	public Socket createSocket(InetAddress arg0, int arg1) throws IOException {
		// TODO Auto-generated method stubs
		return null;
	}

	@Override
	public Socket createSocket(InetAddress arg0, int arg1, InetAddress arg2, int arg3) throws IOException {
		// TODO Auto-generated method stub
		return null;
	}    
}

