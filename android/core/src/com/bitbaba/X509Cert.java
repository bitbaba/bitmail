/**
 * 
 */
package com.bitbaba;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.math.BigInteger;
import java.security.InvalidKeyException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.SecureRandom;
import java.security.Security;
import java.security.SignatureException;
import java.security.cert.Certificate;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.security.cert.CertificateNotYetValidException;
import java.security.cert.X509Certificate;
import java.util.Date;

import javax.security.auth.x500.X500Principal;

import org.bouncycastle.asn1.x509.BasicConstraints;
import org.bouncycastle.asn1.x509.ExtendedKeyUsage;
import org.bouncycastle.asn1.x509.GeneralName;
import org.bouncycastle.asn1.x509.GeneralNames;
import org.bouncycastle.asn1.x509.KeyPurposeId;
import org.bouncycastle.asn1.x509.KeyUsage;
import org.bouncycastle.asn1.x509.X509Extensions;
import org.bouncycastle.openssl.PEMWriter;
import org.bouncycastle.x509.X509V3CertificateGenerator;

/**
 * @author Administrator
 *
 */
public class X509Cert {

	public X509Cert(){
	    Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());

	    KeyPairGenerator kpGen = null;
		try {
			kpGen = KeyPairGenerator.getInstance("RSA", "BC");
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	    kpGen.initialize(2048, new SecureRandom());
	    
	    KeyPair pair = kpGen.generateKeyPair();	     

	    X509V3CertificateGenerator certGen = new X509V3CertificateGenerator();
	    
	    certGen.setSerialNumber(BigInteger.valueOf(System.currentTimeMillis()));
	    
	    certGen.setIssuerDN(new X500Principal("CN=Issuer,EmailAddress=issuer@gmail.com"));
	    
	    certGen.setNotBefore(new Date(System.currentTimeMillis() - 10000));
	    
	    certGen.setNotAfter(new Date(System.currentTimeMillis() + 86400000));
	    
	    certGen.setSubjectDN(new X500Principal("CN=Subject,EmailAddress=subject@gmail.com"));
	    
	    certGen.setPublicKey(pair.getPublic());
	    
	    certGen.setSignatureAlgorithm("SHA256WithRSAEncryption");
	    
	    certGen.addExtension(X509Extensions.BasicConstraints, true, new BasicConstraints(false));
	    
	    certGen.addExtension(X509Extensions.KeyUsage
	    		, true
	    		, new KeyUsage(KeyUsage.digitalSignature | KeyUsage.keyEncipherment ));
	    
	    certGen.addExtension(X509Extensions.ExtendedKeyUsage
	    		, true
	    		, new ExtendedKeyUsage(KeyPurposeId.id_kp_serverAuth));
	    
	    certGen.addExtension(X509Extensions.SubjectAlternativeName
	    		, false
	    		, new GeneralNames(new GeneralName(GeneralName.rfc822Name, "webmaster@gmail.com")));
	    
	    X509Certificate cert = null;
		try {
			cert = certGen.generateX509Certificate(pair.getPrivate(), "BC");
		} catch (InvalidKeyException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (NoSuchProviderException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (SecurityException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (SignatureException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		if (true){
			StringWriter sw = new StringWriter();
			PEMWriter out = new PEMWriter(sw);
			try {
				out.writeObject(cert);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println(sw.toString());	
		}
		
		if (true){
			StringWriter sw = new StringWriter();
			PEMWriter out = new PEMWriter(sw);
			try {
				out.writeObject(pair.getPrivate());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println(sw.toString());
		}


	    try {
			cert.checkValidity(new Date());
		} catch (CertificateExpiredException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateNotYetValidException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	    try {
			cert.verify(cert.getPublicKey());
		} catch (InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SignatureException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	    
	    KeyStore ks = null;
	    try {
			ks = KeyStore.getInstance("PKCS12", "BC");
		} catch (KeyStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	    
	    try {
			ks.load(null,null);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    try {
			ks.setCertificateEntry(cert.getSerialNumber().toString(), cert);
		} catch (KeyStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	    String passphrase = "secret";
	    
	    try {
			ks.setKeyEntry(cert.getSerialNumber().toString()
					, pair.getPrivate()
					, passphrase.toCharArray()
					, new Certificate[]{cert,cert});
		} catch (KeyStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    
	    FileOutputStream fos = null;
		try {
			fos = new FileOutputStream("keystore.pfx");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    try {
			ks.store(fos, passphrase.toCharArray());
		} catch (KeyStoreException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	    try {
			fos.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	    
	}
	public String Encrypt(String msg)
	{
		return msg;
	}
	
	public String Decrypt(String smime)
	{
		return smime;
	}
	
	public String Sign(String msg)
	{
		return msg;
	}
	
	public String Verify(String smime)
	{
		return smime;
	}
	
	public static String MEncrypt(String [] certs, String msg)
	{
		for (int i = 0; i < certs.length; i++){
			String cert = certs[i];
			System.out.println(cert);
		}
		return msg;
	}
}
