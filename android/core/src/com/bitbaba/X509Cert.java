/**
 * 
 */
package com.bitbaba;

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.math.BigInteger;
import java.security.InvalidKeyException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
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

import org.bouncycastle.asn1.oiw.OIWObjectIdentifiers;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.AlgorithmIdentifier;
import org.bouncycastle.asn1.x509.BasicConstraints;
import org.bouncycastle.asn1.x509.ExtendedKeyUsage;
import org.bouncycastle.asn1.x509.Extension;
import org.bouncycastle.asn1.x509.GeneralName;
import org.bouncycastle.asn1.x509.GeneralNames;
import org.bouncycastle.asn1.x509.KeyPurposeId;
import org.bouncycastle.asn1.x509.KeyUsage;
import org.bouncycastle.asn1.x509.SubjectPublicKeyInfo;
import org.bouncycastle.asn1.x509.X509Extensions;
import org.bouncycastle.cert.CertIOException;
import org.bouncycastle.cert.X509CertificateHolder;
import org.bouncycastle.cert.X509ExtensionUtils;
import org.bouncycastle.cert.X509v3CertificateBuilder;
import org.bouncycastle.cert.jcajce.JcaX509CertificateConverter;
import org.bouncycastle.cert.jcajce.JcaX509v3CertificateBuilder;
import org.bouncycastle.crypto.Digest;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.PEMEncryptedKeyPair;
import org.bouncycastle.openssl.PEMException;
import org.bouncycastle.openssl.PEMKeyPair;
import org.bouncycastle.openssl.PEMParser;
import org.bouncycastle.openssl.PEMWriter;
import org.bouncycastle.openssl.jcajce.JcaMiscPEMGenerator;
import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.openssl.jcajce.JcaPEMWriter;
import org.bouncycastle.openssl.jcajce.JcePEMDecryptorProviderBuilder;
import org.bouncycastle.openssl.jcajce.JcePEMEncryptorBuilder;
import org.bouncycastle.operator.DigestCalculator;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;
import org.bouncycastle.x509.X509V3CertificateGenerator;

/**
 * @author Administrator
 *
 */
public class X509Cert {
	private PrivateKey key_;
	private X509Certificate cert_;

	public X509Cert(String nick, String email, int bits){
		/*
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
			cert_ = (sw.toString());	
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
			key_ = (sw.toString());
		}
		*/
  
	}
	/*
	public boolean MakeCertDeprecated(String nick, String email, int bits)
	{
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
	    
	    kpGen.initialize(bits != 0 ? bits : 2048, new SecureRandom());
	    
	    // Protect Private Key with a passphrase 
	    //http://stackoverflow.com/questions/5127379/how-to-generate-a-rsa-keypair-with-a-privatekey-encrypted-with-password
	    KeyPair pair = kpGen.generateKeyPair();	   

	    X509V3CertificateGenerator certGen = new X509V3CertificateGenerator();
	    
	    certGen.setSerialNumber(new BigInteger("1"));
	    
	    certGen.setIssuerDN(new X500Principal("CN=" + nick + ",EmailAddress=" + email));
	    
	    certGen.setNotBefore(new Date(System.currentTimeMillis()));
	    
	    certGen.setNotAfter(new Date(System.currentTimeMillis() + 30000*86400000));
	    
	    certGen.setSubjectDN(new X500Principal("CN=" + nick + ",EmailAddress=" + email));
	    
	    certGen.setPublicKey(pair.getPublic());
	    
	    certGen.setSignatureAlgorithm("SHA256WithRSAEncryption");
	    
	    certGen.addExtension(X509Extensions.BasicConstraints
	    		, true
	    		, new BasicConstraints(false));
	    
	    certGen.addExtension(X509Extensions.KeyUsage
	    		, true
	    		, new KeyUsage(KeyUsage.digitalSignature 
	    				| KeyUsage.cRLSign
	    				| KeyUsage.dataEncipherment
	    				//| KeyUsage.decipherOnly
	    				//| KeyUsage.encipherOnly
	    				| KeyUsage.keyAgreement
	    				| KeyUsage.keyCertSign
	    				| KeyUsage.keyEncipherment
	    				| KeyUsage.nonRepudiation ));
	    
	    certGen.addExtension(X509Extensions.ExtendedKeyUsage
	    		, true
	    		, new ExtendedKeyUsage( KeyPurposeId.anyExtendedKeyUsage));
	    
	    certGen.addExtension(X509Extensions.SubjectAlternativeName
	    		, false
	    		, new GeneralNames(new GeneralName(GeneralName.rfc822Name, email)));
	    
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
		
		return false;
	}
	*/
	
	public boolean MakeCert(String nick, String email, int bits)
	{
		KeyPairGenerator kpg = null;
		try {
			kpg = KeyPairGenerator.getInstance("RSA", BouncyCastleProvider.PROVIDER_NAME);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
        kpg.initialize(bits, new SecureRandom());
        KeyPair kp = kpg.generateKeyPair();
        
        PublicKey  subPub  = kp.getPublic();
        key_ = kp.getPrivate();
        PublicKey  issPub  = kp.getPublic();
        
        String issuerDN = "CN=" + nick + ",EmailAddress=" + email;
        String subjectDN = issuerDN; // self-signed
        
        X509v3CertificateBuilder v3CertGen = new JcaX509v3CertificateBuilder(
            new X500Name(issuerDN),
            new BigInteger("1"),
            new Date(System.currentTimeMillis()),
            new Date(System.currentTimeMillis() + (1000L * 60 * 60 * 24 * (30000/*days*/))),
            new X500Name(subjectDN),
            subPub);

        JcaContentSignerBuilder contentSignerBuilder = new JcaContentSignerBuilder("SHA1WithRSA");

        contentSignerBuilder.setProvider(BouncyCastleProvider.PROVIDER_NAME);
        
        X509ExtensionUtils extUtils = new X509ExtensionUtils(new SHA1DigestCalculator());
               
        try {
			v3CertGen.addExtension(
			    Extension.subjectKeyIdentifier,
			    false,
			    extUtils.createSubjectKeyIdentifier(SubjectPublicKeyInfo.getInstance(subPub.getEncoded())));
		} catch (CertIOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

        try {
			v3CertGen.addExtension(
			    Extension.authorityKeyIdentifier,
			    false,
			    extUtils.createAuthorityKeyIdentifier(SubjectPublicKeyInfo.getInstance(issPub.getEncoded())));
		} catch (CertIOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        try {
			v3CertGen.addExtension(
			    Extension.basicConstraints,
			    false,
			    new BasicConstraints(true));
		} catch (CertIOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        
        try {
			v3CertGen.addExtension(Extension.keyUsage
					, true
					, new KeyUsage(KeyUsage.digitalSignature 
							| KeyUsage.cRLSign
							| KeyUsage.dataEncipherment
							//| KeyUsage.decipherOnly
							//| KeyUsage.encipherOnly
							| KeyUsage.keyAgreement
							| KeyUsage.keyCertSign
							| KeyUsage.keyEncipherment
							| KeyUsage.nonRepudiation));
		} catch (CertIOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        
        try {
			v3CertGen.addExtension(Extension.extendedKeyUsage
					, true
					, new ExtendedKeyUsage( KeyPurposeId.anyExtendedKeyUsage));
		} catch (CertIOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		try {
			cert_ = new JcaX509CertificateConverter().setProvider("BC").getCertificate(v3CertGen.build(contentSignerBuilder.build(key_)));
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (OperatorCreationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        try {
        	cert_.checkValidity(new Date());
		} catch (CertificateExpiredException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateNotYetValidException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        try {
        	cert_.verify(issPub);
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
        
		return true;
	}
	
	public boolean Load(String key, String passphrase, String cert)
	{
		// Ref:
		// bcpkix-jdk15on-155/bcpkix-jdk15on-155/org/bouncycastle/openssl/test/WriterTest.java
    
        /*
		StringWriter sw = new StringWriter();
		PEMWriter out = new PEMWriter(sw);
		try {
			out.writeObject(privKey);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		try {
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		*/
		return true;
	}
	
	public X509Certificate LoadCertificate(String certPEMData)
	{
		PEMParser pemParser = new PEMParser(new StringReader(certPEMData));
		Object o = null;
		try {
			o = pemParser.readObject();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		}
        if (o == null || !(o instanceof X509CertificateHolder))
        {
            return null;
        }
        X509CertificateHolder x509CertificateHolder = (X509CertificateHolder) o;
        X509Certificate cert = null;
		try {
			cert = new JcaX509CertificateConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getCertificate(x509CertificateHolder);
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
        return cert;
	}
	
	public PrivateKey LoadPrivateKey(String keyPEMData, String passphrase)
	{
		PEMParser pr = new PEMParser(new StringReader(keyPEMData));
		Object o = null;
		try {
			o = pr.readObject();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		}
		if (o == null || !(o instanceof PEMEncryptedKeyPair) || !(o instanceof PEMKeyPair))
		{
		    return null;
		}
		KeyPair kp = null;
		try {
			if (o instanceof PEMEncryptedKeyPair && passphrase != null && !passphrase.isEmpty()){
				kp = new JcaPEMKeyConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getKeyPair(((PEMEncryptedKeyPair)o).decryptKeyPair(new JcePEMDecryptorProviderBuilder().setProvider("BC").build(passphrase.toCharArray())));
			}else if (o instanceof PEMKeyPair){
				kp = new JcaPEMKeyConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getKeyPair(((PEMKeyPair)o));
			}else{
				return null;
			}
		} catch (PEMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		if (kp == null){
			return null;
		}	
		PrivateKey key = kp.getPrivate();	
		return key;
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
	
	public void Dump()
	{
		System.out.println(cert_);
		System.out.println(key_);
		//String encKey = this.GetEncryptedKey();
		//System.out.println(encKey);
		//this.Load(encKey, passphrase_, cert_);
		System.out.println(cert_);
		System.out.println(key_);
	}
	
	public String GetPrivateKey(String passphrase)
	{
		/*
        PEMParser pr = new PEMParser(new StringReader(key_));
        Object o = null;
		try {
			o = pr.readObject();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
        if (o == null || !(o instanceof PEMKeyPair))
        {
            return "";
        }
        KeyPair kp = null;
		try {
			kp = new JcaPEMKeyConverter().setProvider("BC").getKeyPair((PEMKeyPair)o);
		} catch (PEMException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		if (kp == null){
			return "";
		}
		
        PrivateKey privKey = kp.getPrivate();
        */
        
        StringWriter sw = new StringWriter();
        JcaPEMWriter pw = new JcaPEMWriter(sw);
        try {
			pw.writeObject(new JcaMiscPEMGenerator(key_
					, new JcePEMEncryptorBuilder("AES-128-ECB").setSecureRandom(new SecureRandom()).build(passphrase.toCharArray())));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        try {
			pw.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        return sw.toString();
	}
}

/**
 * @version 1.0
 * @author harry
 *
 */
class SHA1DigestCalculator implements DigestCalculator
{
    private ByteArrayOutputStream bOut = new ByteArrayOutputStream();

    public AlgorithmIdentifier getAlgorithmIdentifier()
    {
        return new AlgorithmIdentifier(OIWObjectIdentifiers.idSHA1);
    }

    public OutputStream getOutputStream()
    {
        return bOut;
    }

    public byte[] getDigest()
    {
        byte[] bytes = bOut.toByteArray();

        bOut.reset();

        Digest sha1 = new SHA1Digest();

        sha1.update(bytes, 0, bytes.length);

        byte[] digest = new byte[sha1.getDigestSize()];

        sha1.doFinal(digest, 0);

        return digest;
    }
}
