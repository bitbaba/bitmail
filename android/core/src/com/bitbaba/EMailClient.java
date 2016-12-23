/**
 * 
 */
package com.bitbaba;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.SecureRandom;
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
import javax.net.ssl.SSLEngine;
import javax.net.ssl.SSLSocket;

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
	
	public boolean SendByTlsClient(String to, String content)
	{
		// http://stackoverflow.com/questions/7397855/smtp-client-using-bouncycastles-lightweight-tls-api
		// bcprov-jdk15on-154\src\org\bouncycastle\crypto\tls\test\TlsProtocolTest.java
		Socket socket = null;
		try {
			//socket = new Socket(smtp_, SMTP_SSL_DEFAULT_PORT);
			socket = new Socket("202.89.233.104", 443);
		} catch (UnknownHostException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		TlsClientProtocol handler = null;
		try {
			handler = new TlsClientProtocol(socket.getInputStream(), socket.getOutputStream(), new SecureRandom());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		try {			
			TlsClient client = new MockTlsClient(null);
			handler.connect(client);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
        InputStream is = handler.getInputStream();
        OutputStream os = handler.getOutputStream();

        try {
			os.write("GET / HTTP/1.1\r\n\r\n".getBytes());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        byte[] buf = new byte[4096];
        int read = 0;
        int total = 0;

        try {
			while ((read = is.read(buf, total, buf.length - total)) > 0)
			{
			    total += read;
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
		
		return true;
	}

	public boolean Send(String to, String content)
	{
		System.setProperty("javax.net.debug", "all");
		//System.setProperty("javax.net.debug", "ssl:handshake:verbose");
		System.setProperty("jdk.tls.client.protocols", "TLSv1");
		

		Properties properties = new Properties();
		properties.put("mail.smtp.ssl.enable", "true");
		properties.put("mail.smtps.host", smtp_);
		properties.put("mail.smtps.auth", "true");
		properties.put("mail.smtps.ssl.trust", "*");
		
		// use SSL in JSSE instead of default socket factory
		properties.put("mail.smtps.ssl.socketFactory.class", "javax.net.ssl.SSLSocketFactory");
		// process SSL-only request
		properties.put("mail.smtps.ssl.socketFactory.fallback", "true");
		properties.put("mail.smtps.port", SMTP_SSL_DEFAULT_PORT);
		properties.put("mail.smtps.socketFactory.port", SMTP_SSL_DEFAULT_PORT);
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
				// https://blogs.oracle.com/coffeys/entry/jdk_and_use_of_rc4
				// http://tools.ietf.org/html/rfc5246#appendix-A.3
			    /*
				enum { warning(1), fatal(2), (255) } AlertLevel;
			
			   	enum {
			       close_notify(0),
			       unexpected_message(10),
			       bad_record_mac(20),
			       decryption_failed_RESERVED(21),
			       record_overflow(22),
			       decompression_failure(30),
			       handshake_failure(40),
			       no_certificate_RESERVED(41),
			       bad_certificate(42),
			       unsupported_certificate(43),
			       certificate_revoked(44),
			       certificate_expired(45),
			       certificate_unknown(46),
			       illegal_parameter(47),
			       unknown_ca(48),
			       access_denied(49),
			       decode_error(50),
			       decrypt_error(51),
			       export_restriction_RESERVED(60),
			       protocol_version(70),
			       insufficient_security(71),
			       internal_error(80),
			       user_canceled(90),
			       no_renegotiation(100),
			       unsupported_extension(110),           // new 
			       (255)
			   } AlertDescription;
			
			   struct {
			       AlertLevel level;
			       AlertDescription description;
			   } Alert;
			   */
			trans.connect(smtp_, login_, password_);
			/*
			*** ClientHello, TLSv1.2
			RandomCookie:  GMT: 1482309535 bytes = { ... }
			Session ID:  {}
			Cipher Suites: [TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, TLS_RSA_WITH_AES_128_CBC_SHA256, TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256, TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256, TLS_DHE_RSA_WITH_AES_128_CBC_SHA256, TLS_DHE_DSS_WITH_AES_128_CBC_SHA256, TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA, TLS_RSA_WITH_AES_128_CBC_SHA, TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA, TLS_ECDH_RSA_WITH_AES_128_CBC_SHA, TLS_DHE_RSA_WITH_AES_128_CBC_SHA, TLS_DHE_DSS_WITH_AES_128_CBC_SHA, TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, TLS_RSA_WITH_AES_128_GCM_SHA256, TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256, TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256, TLS_DHE_RSA_WITH_AES_128_GCM_SHA256, TLS_DHE_DSS_WITH_AES_128_GCM_SHA256, TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA, TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA, SSL_RSA_WITH_3DES_EDE_CBC_SHA, TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA, TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA, SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA, SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA, TLS_EMPTY_RENEGOTIATION_INFO_SCSV]
			Compression Methods:  { 0 }
			Extension elliptic_curves, curve names: {secp256r1, sect163k1, sect163r2, secp192r1, secp224r1, sect233k1, sect233r1, sect283k1, sect283r1, secp384r1, sect409k1, sect409r1, secp521r1, sect571k1, sect571r1, secp160k1, secp160r1, secp160r2, sect163r1, secp192k1, sect193r1, sect193r2, secp224k1, sect239k1, secp256k1}
			Extension ec_point_formats, formats: [uncompressed]
			Extension signature_algorithms, signature_algorithms: SHA512withECDSA, SHA512withRSA, SHA384withECDSA, SHA384withRSA, SHA256withECDSA, SHA256withRSA, SHA224withECDSA, SHA224withRSA, SHA1withECDSA, SHA1withRSA, SHA1withDSA
			Extension server_name, server_name: [type=host_name (0), value=smtp.qq.com]
			***
			*/
			/*
			sometimes failed: 
			[Raw read]: length = 2
			0000: 02 28 .(
			main, READ: TLSv1.2 Alert, length = 2
			main, RECV TLSv1.2 ALERT:  fatal, handshake_failure				
			Note:
			fatal=0x02 and handshake_failure = 0x28
			*/
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
	
	public HashMap<String, String> Receive(String email)
	{
		HashMap<String, String> result = new HashMap<String, String>();
		return result;
	}
	
	public static String GetHostFromEmailAddress(String email)
	{
		if (email.indexOf("@") == -1){
			return "";
		}
		return email.substring(email.indexOf("@")+1);
	}
	
}

class MockTlsClient
    extends DefaultTlsClient
{
    TlsSession session;

    MockTlsClient(TlsSession session)
    {
        this.session = session;
    }

    public TlsSession getSessionToResume()
    {
        return this.session;
    }

    public void notifyAlertRaised(short alertLevel, short alertDescription, String message, Throwable cause)
    {
        PrintStream out = (alertLevel == AlertLevel.fatal) ? System.err : System.out;
        out.println("TLS client raised alert: " + AlertLevel.getText(alertLevel)
            + ", " + AlertDescription.getText(alertDescription));
        if (message != null)
        {
            out.println("> " + message);
        }
        if (cause != null)
        {
            cause.printStackTrace(out);
        }
    }

    public void notifyAlertReceived(short alertLevel, short alertDescription)
    {
        PrintStream out = (alertLevel == AlertLevel.fatal) ? System.err : System.out;
        out.println("TLS client received alert: " + AlertLevel.getText(alertLevel)
            + ", " + AlertDescription.getText(alertDescription));
    }

//    public int[] getCipherSuites()
//    {
//        return Arrays.concatenate(super.getCipherSuites(),
//            new int[]
//            {
//                CipherSuite.TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
//                CipherSuite.TLS_ECDHE_RSA_WITH_ESTREAM_SALSA20_SHA1,
//                CipherSuite.TLS_ECDHE_RSA_WITH_SALSA20_SHA1,
//                CipherSuite.TLS_RSA_WITH_ESTREAM_SALSA20_SHA1,
//                CipherSuite.TLS_RSA_WITH_SALSA20_SHA1,
//            });
//    }

    public Hashtable getClientExtensions() throws IOException
    {
        Hashtable clientExtensions = TlsExtensionsUtils.ensureExtensionsInitialised(super.getClientExtensions());
        TlsExtensionsUtils.addEncryptThenMACExtension(clientExtensions);
        TlsExtensionsUtils.addExtendedMasterSecretExtension(clientExtensions);
        {
            /*
             * NOTE: If you are copying test code, do not blindly set these extensions in your own client.
             */
            TlsExtensionsUtils.addMaxFragmentLengthExtension(clientExtensions, MaxFragmentLength.pow2_9);
            TlsExtensionsUtils.addTruncatedHMacExtension(clientExtensions);
        }
        return clientExtensions;
    }

    public void notifyServerVersion(ProtocolVersion serverVersion) throws IOException
    {
        super.notifyServerVersion(serverVersion);

        System.out.println("TLS client negotiated " + serverVersion);
    }

    public TlsAuthentication getAuthentication()
        throws IOException
    {
    	return  null;
    	/*
        return new TlsAuthentication()
        {
            public void notifyServerCertificate(org.bouncycastle.crypto.tls.Certificate serverCertificate)
                throws IOException
            {
                Certificate[] chain = serverCertificate.getCertificateList();
                System.out.println("TLS client received server certificate chain of length " + chain.length);
                for (int i = 0; i != chain.length; i++)
                {
                    Certificate entry = chain[i];
                    // TODO Create fingerprint based on certificate signature algorithm digest
                    System.out.println("    fingerprint:SHA-256 " 
                    					+ TlsTestUtils.fingerprint(entry) 
                    					+ " (" + entry.getSubject() + ")");
                }
            }

            public TlsCredentials getClientCredentials(CertificateRequest certificateRequest)
                throws IOException
            {
                short[] certificateTypes = certificateRequest.getCertificateTypes();
                if (certificateTypes == null || !Arrays.contains(certificateTypes, ClientCertificateType.rsa_sign))
                {
                    return null;
                }

                return TlsTestUtils.loadSignerCredentials(context, certificateRequest.getSupportedSignatureAlgorithms(),
                    SignatureAlgorithm.rsa, "x509-client.pem", "x509-client-key.pem");
            }
        };
        */
    }

    public void notifyHandshakeComplete() throws IOException
    {
        super.notifyHandshakeComplete();

        TlsSession newSession = context.getResumableSession();
        if (newSession != null)
        {
            byte[] newSessionID = newSession.getSessionID();
            String hex = Hex.toHexString(newSessionID);

            /*
            if (this.session != null && Arrays.areEqual(this.session.getSessionID(), newSessionID))
            {
                System.out.println("Resumed session: " + hex);
            }
            else
            {
                System.out.println("Established session: " + hex);
            }
            */

            this.session = newSession;
        }
    }
}

class TlsTestUtils
{
	/*
    static final byte[] rsaCertData = Base64
        .decode("MIICUzCCAf2gAwIBAgIBATANBgkqhkiG9w0BAQQFADCBjzELMAkGA1UEBhMCQVUxKDAmBgNVBAoMH1RoZSBMZWdpb2"
            + "4gb2YgdGhlIEJvdW5jeSBDYXN0bGUxEjAQBgNVBAcMCU1lbGJvdXJuZTERMA8GA1UECAwIVmljdG9yaWExLzAtBgkq"
            + "hkiG9w0BCQEWIGZlZWRiYWNrLWNyeXB0b0Bib3VuY3ljYXN0bGUub3JnMB4XDTEzMDIyNTA2MDIwNVoXDTEzMDIyNT"
            + "A2MDM0NVowgY8xCzAJBgNVBAYTAkFVMSgwJgYDVQQKDB9UaGUgTGVnaW9uIG9mIHRoZSBCb3VuY3kgQ2FzdGxlMRIw"
            + "EAYDVQQHDAlNZWxib3VybmUxETAPBgNVBAgMCFZpY3RvcmlhMS8wLQYJKoZIhvcNAQkBFiBmZWVkYmFjay1jcnlwdG"
            + "9AYm91bmN5Y2FzdGxlLm9yZzBaMA0GCSqGSIb3DQEBAQUAA0kAMEYCQQC0p+RhcFdPFqlwgrIr5YtqKmKXmEGb4Shy"
            + "pL26Ymz66ZAPdqv7EhOdzl3lZWT6srZUMWWgQMYGiHQg4z2R7X7XAgERo0QwQjAOBgNVHQ8BAf8EBAMCBSAwEgYDVR"
            + "0lAQH/BAgwBgYEVR0lADAcBgNVHREBAf8EEjAQgQ50ZXN0QHRlc3QudGVzdDANBgkqhkiG9w0BAQQFAANBAHU55Ncz"
            + "eglREcTg54YLUlGWu2WOYWhit/iM1eeq8Kivro7q98eW52jTuMI3CI5ulqd0hYzshQKQaZ5GDzErMyM=");

    static final byte[] dudRsaCertData = Base64
        .decode("MIICUzCCAf2gAwIBAgIBATANBgkqhkiG9w0BAQQFADCBjzELMAkGA1UEBhMCQVUxKDAmBgNVBAoMH1RoZSBMZWdpb2"
            + "4gb2YgdGhlIEJvdW5jeSBDYXN0bGUxEjAQBgNVBAcMCU1lbGJvdXJuZTERMA8GA1UECAwIVmljdG9yaWExLzAtBgkq"
            + "hkiG9w0BCQEWIGZlZWRiYWNrLWNyeXB0b0Bib3VuY3ljYXN0bGUub3JnMB4XDTEzMDIyNTA1NDcyOFoXDTEzMDIyNT"
            + "A1NDkwOFowgY8xCzAJBgNVBAYTAkFVMSgwJgYDVQQKDB9UaGUgTGVnaW9uIG9mIHRoZSBCb3VuY3kgQ2FzdGxlMRIw"
            + "EAYDVQQHDAlNZWxib3VybmUxETAPBgNVBAgMCFZpY3RvcmlhMS8wLQYJKoZIhvcNAQkBFiBmZWVkYmFjay1jcnlwdG"
            + "9AYm91bmN5Y2FzdGxlLm9yZzBaMA0GCSqGSIb3DQEBAQUAA0kAMEYCQQC0p+RhcFdPFqlwgrIr5YtqKmKXmEGb4Shy"
            + "pL26Ymz66ZAPdqv7EhOdzl3lZWT6srZUMWWgQMYGiHQg4z2R7X7XAgERo0QwQjAOBgNVHQ8BAf8EBAMCAAEwEgYDVR"
            + "0lAQH/BAgwBgYEVR0lADAcBgNVHREBAf8EEjAQgQ50ZXN0QHRlc3QudGVzdDANBgkqhkiG9w0BAQQFAANBAJg55PBS"
            + "weg6obRUKF4FF6fCrWFi6oCYSQ99LWcAeupc5BofW5MstFMhCOaEucuGVqunwT5G7/DweazzCIrSzB0=");

	*/
    static String fingerprint(org.bouncycastle.asn1.x509.Certificate c)
        throws IOException
    {
        byte[] der = c.getEncoded();
        byte[] sha1 = sha256DigestOf(der);
        byte[] hexBytes = Hex.encode(sha1);
        String hex = new String(hexBytes, "ASCII").toUpperCase();

        StringBuffer fp = new StringBuffer();
        int i = 0;
        fp.append(hex.substring(i, i + 2));
        while ((i += 2) < hex.length())
        {
            fp.append(':');
            fp.append(hex.substring(i, i + 2));
        }
        return fp.toString();
    }

    static byte[] sha256DigestOf(byte[] input)
    {
        SHA256Digest d = new SHA256Digest();
        d.update(input, 0, input.length);
        byte[] result = new byte[d.getDigestSize()];
        d.doFinal(result, 0);
        return result;
    }

    static TlsAgreementCredentials loadAgreementCredentials(TlsContext context,
        String[] certResources, String keyResource)
        throws IOException
    {
        Certificate certificate = loadCertificateChain(certResources);
        AsymmetricKeyParameter privateKey = loadPrivateKeyResource(keyResource);

        return new DefaultTlsAgreementCredentials(certificate, privateKey);
    }

    static TlsEncryptionCredentials loadEncryptionCredentials(TlsContext context,
        String[] certResources, String keyResource)
        throws IOException
    {
        Certificate certificate = loadCertificateChain(certResources);
        AsymmetricKeyParameter privateKey = loadPrivateKeyResource(keyResource);

        return new DefaultTlsEncryptionCredentials(context, certificate, privateKey);
    }

    static TlsSignerCredentials loadSignerCredentials(TlsContext context, String[] certResources,
        String keyResource, SignatureAndHashAlgorithm signatureAndHashAlgorithm)
        throws IOException
    {
        Certificate certificate = loadCertificateChain(certResources);
        AsymmetricKeyParameter privateKey = loadPrivateKeyResource(keyResource);

        return new DefaultTlsSignerCredentials(context, certificate, privateKey, signatureAndHashAlgorithm);
    }

    static TlsSignerCredentials loadSignerCredentials(TlsContext context, Vector supportedSignatureAlgorithms,
        short signatureAlgorithm, String certResource, String keyResource)
        throws IOException
    {
        /*
         * TODO Note that this code fails to provide default value for the client supported
         * algorithms if it wasn't sent.
         */
     
        SignatureAndHashAlgorithm signatureAndHashAlgorithm = null;
        if (supportedSignatureAlgorithms != null)
        {
            for (int i = 0; i < supportedSignatureAlgorithms.size(); ++i)
            {
                SignatureAndHashAlgorithm alg = (SignatureAndHashAlgorithm)
                    supportedSignatureAlgorithms.elementAt(i);
                if (alg.getSignature() == signatureAlgorithm)
                {
                    signatureAndHashAlgorithm = alg;
                    break;
                }
            }

            if (signatureAndHashAlgorithm == null)
            {
                return null;
            }
        }

        return loadSignerCredentials(context, new String[]{ certResource, "x509-ca.pem" },
            keyResource, signatureAndHashAlgorithm);
    }

    static Certificate loadCertificateChain(String[] resources)
        throws IOException
    {
        org.bouncycastle.asn1.x509.Certificate[] chain = new org.bouncycastle.asn1.x509.Certificate[resources.length];
        for (int i = 0; i < resources.length; ++i)
        {
            chain[i] = loadCertificateResource(resources[i]);
        }
        return new Certificate(chain);
    }

    static org.bouncycastle.asn1.x509.Certificate loadCertificateResource(String resource)
        throws IOException
    {
        PemObject pem = loadPemResource(resource);
        if (pem.getType().endsWith("CERTIFICATE"))
        {
            return org.bouncycastle.asn1.x509.Certificate.getInstance(pem.getContent());
        }
        throw new IllegalArgumentException("'resource' doesn't specify a valid certificate");
    }

    static AsymmetricKeyParameter loadPrivateKeyResource(String resource)
        throws IOException
    {
        PemObject pem = loadPemResource(resource);
        if (pem.getType().endsWith("RSA PRIVATE KEY"))
        {
            RSAPrivateKey rsa = RSAPrivateKey.getInstance(pem.getContent());
            return new RSAPrivateCrtKeyParameters(rsa.getModulus(), rsa.getPublicExponent(),
                rsa.getPrivateExponent(), rsa.getPrime1(), rsa.getPrime2(), rsa.getExponent1(),
                rsa.getExponent2(), rsa.getCoefficient());
        }
        if (pem.getType().endsWith("PRIVATE KEY"))
        {
            return PrivateKeyFactory.createKey(pem.getContent());
        }
        throw new IllegalArgumentException("'resource' doesn't specify a valid private key");
    }

    static PemObject loadPemResource(String resource)
        throws IOException
    {
        InputStream s = TlsTestUtils.class.getResourceAsStream(resource);
        PemReader p = new PemReader(new InputStreamReader(s));
        PemObject o = p.readPemObject();
        p.close();
        return o;
    }
}

