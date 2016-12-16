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
import java.io.UnsupportedEncodingException;
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
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import javax.mail.MessagingException;
import javax.mail.internet.MimeBodyPart;
import javax.mail.internet.MimeMessage;
import javax.mail.internet.MimeMultipart;
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
import org.bouncycastle.cert.jcajce.JcaCertStore;
import org.bouncycastle.cert.jcajce.JcaX509CertificateConverter;
import org.bouncycastle.cert.jcajce.JcaX509v3CertificateBuilder;
import org.bouncycastle.cms.CMSAlgorithm;
import org.bouncycastle.cms.CMSEnvelopedData;
import org.bouncycastle.cms.CMSEnvelopedDataGenerator;
import org.bouncycastle.cms.CMSException;
import org.bouncycastle.cms.CMSProcessableByteArray;
import org.bouncycastle.cms.CMSSignedData;
import org.bouncycastle.cms.CMSSignedDataGenerator;
import org.bouncycastle.cms.CMSTypedData;
import org.bouncycastle.cms.RecipientInformation;
import org.bouncycastle.cms.RecipientInformationStore;
import org.bouncycastle.cms.SignerInformation;
import org.bouncycastle.cms.SignerInformationStore;
import org.bouncycastle.cms.jcajce.JcaSignerInfoGeneratorBuilder;
import org.bouncycastle.cms.jcajce.JcaSimpleSignerInfoGeneratorBuilder;
import org.bouncycastle.cms.jcajce.JcaSimpleSignerInfoVerifierBuilder;
import org.bouncycastle.cms.jcajce.JceCMSContentEncryptorBuilder;
import org.bouncycastle.cms.jcajce.JceKeyTransEnvelopedRecipient;
import org.bouncycastle.cms.jcajce.JceKeyTransRecipientInfoGenerator;
import org.bouncycastle.crypto.Digest;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.mail.smime.SMIMEEnvelopedGenerator;
import org.bouncycastle.mail.smime.SMIMEException;
import org.bouncycastle.mail.smime.SMIMESignedGenerator;
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
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.DigestCalculator;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;
import org.bouncycastle.operator.jcajce.JcaDigestCalculatorProviderBuilder;
import org.bouncycastle.util.Store;
import org.bouncycastle.x509.X509V3CertificateGenerator;

import com.sun.org.apache.xml.internal.security.exceptions.Base64DecodingException;
import com.sun.org.apache.xml.internal.security.utils.Base64;

/**
 * @author Administrator
 *
 */
public class X509Cert {
	private PrivateKey key_ = null;
	private X509Certificate cert_ = null;

	public X509Cert()
	{
		
	}
	
	public X509Cert(String nick, String email, int bits){
		  MakeCertificate(nick, email, bits);
	}
		
	public boolean MakeCertificate(String nick, String email, int bits)
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
        
        X509v3CertificateBuilder v3CertGen = 
        		new JcaX509v3CertificateBuilder(new X500Name(issuerDN),
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
			return false;
		}

        try {
			v3CertGen.addExtension(
			    Extension.authorityKeyIdentifier,
			    false,
			    extUtils.createAuthorityKeyIdentifier(SubjectPublicKeyInfo.getInstance(issPub.getEncoded())));
		} catch (CertIOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}

        try {
			v3CertGen.addExtension(
			    Extension.basicConstraints,
			    false,
			    new BasicConstraints(true));
		} catch (CertIOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
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
			return false;
		}
        
        try {
			v3CertGen.addExtension(Extension.extendedKeyUsage
								, true
								, new ExtendedKeyUsage( KeyPurposeId.anyExtendedKeyUsage));
		} catch (CertIOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return false;
		}

		try {
			cert_ = new JcaX509CertificateConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getCertificate(v3CertGen.build(contentSignerBuilder.build(key_)));
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (OperatorCreationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}

        try {
        	cert_.checkValidity(new Date());
		} catch (CertificateExpiredException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (CertificateNotYetValidException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
        try {
        	cert_.verify(issPub);
		} catch (InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (SignatureException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
        
		return true;
	}
	
	/**
	 * Encrypt text with Public key in certificate and return `Enveloped data' in PEM format.
	 * @param text
	 * @return
	 */
	public String Encrypt(String text)
	{
		CMSTypedData msg = null;
		try {
			msg = new CMSProcessableByteArray(text.getBytes(java.nio.charset.StandardCharsets.UTF_8.name()));
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		}
		
		JceKeyTransRecipientInfoGenerator recipGen = null;
		
		try {
			recipGen = new JceKeyTransRecipientInfoGenerator(cert_);
		} catch (CertificateEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		if (recipGen == null){
			return "";
		}
		
		recipGen.setProvider(BouncyCastleProvider.PROVIDER_NAME);
		
	   	CMSEnvelopedDataGenerator edGen = new CMSEnvelopedDataGenerator();
	
		edGen.addRecipientInfoGenerator(recipGen);
	
	   	CMSEnvelopedData ed = null;
		try {
			ed = edGen.generate(msg, new JceCMSContentEncryptorBuilder(CMSAlgorithm.DES_EDE3_CBC).setProvider(BouncyCastleProvider.PROVIDER_NAME).build());
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
	   
		try {
			return Base64.encode(ed.getEncoded());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return "";
	}
	
	/**
	 * Decrypt `Enveloped data' in PEM format to String
	 * @param pemdata
	 * @return
	 */
	public String Decrypt(String pemdata)
	{
		CMSEnvelopedData ed = null;
		try {
			ed = new CMSEnvelopedData(Base64.decode(pemdata));// Base64 decode PEM to DER(asn.1) binary byte array;
		} catch (Base64DecodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		} catch (CMSException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		}

		RecipientInformationStore  recipients = ed.getRecipientInfos();

		Collection  c = recipients.getRecipients();
		
		Iterator    it = c.iterator();

		if (!it.hasNext()){
			return "";
		}
		
		RecipientInformation   recipient = (RecipientInformation)it.next();
			
		byte[] recData = null;
		try {
			recData = recipient.getContent(new JceKeyTransEnvelopedRecipient(key_).setProvider(BouncyCastleProvider.PROVIDER_NAME));
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
		
		try {
			return new String(recData, java.nio.charset.StandardCharsets.UTF_8.name());
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}	
		return "";
	}
	
	/**
	 * Sign a text and return PEM encoded `Signed data'
	 * @param text
	 * @return
	 */
	public String Sign(String text)
	{
		CMSTypedData msg = null;
		try {
			msg = new CMSProcessableByteArray(text.getBytes(java.nio.charset.StandardCharsets.UTF_8.name()));
		} catch (UnsupportedEncodingException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
			return "";
		}
		ArrayList<X509Certificate> certList = new ArrayList<X509Certificate>();
		certList.add(cert_);
		Store st = null;
		try {
			st = new JcaCertStore(certList);
		} catch (CertificateEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
		
		ContentSigner signer = null;
		try {
			signer = new JcaContentSignerBuilder("SHA1WithRSA").setProvider(BouncyCastleProvider.PROVIDER_NAME).build(key_);
		} catch (OperatorCreationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
		if (signer == null){
			return "";
		}
		
		CMSSignedDataGenerator gen = new CMSSignedDataGenerator();
		try {
			gen.addSignerInfoGenerator(new JcaSignerInfoGeneratorBuilder(new JcaDigestCalculatorProviderBuilder().setProvider("BC").build()).build(signer, cert_));
		} catch (CertificateEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		} catch (OperatorCreationException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		}
		try {
			gen.addCertificates(st);
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
		
		CMSSignedData sig = null;
		try {
			sig = gen.generate(msg, true);
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (sig == null){
			return "";
		}
		
		try {
			return Base64.encode(sig.getEncoded());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return "";
	}
	
	/**
	 * Verify PEM encoded `Signed data' and return certificate and message text, etc.;
	 * @param pemdata
	 * @return
	 */
	public HashMap<String, String> Verify(String pemdata)
	{
		HashMap<String, String> result = new HashMap<String, String>();
		
		CMSSignedData sig = null;
		try {
			sig = new CMSSignedData(Base64.decode(pemdata));
		} catch (Base64DecodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		} catch (CMSException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		}
		
		CMSTypedData typed = sig.getSignedContent();
		ByteArrayOutputStream bofs = new ByteArrayOutputStream();
		try {
			typed.write(bofs);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		} catch (CMSException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		}
		
		String text = null;
		try {
			text = bofs.toString(java.nio.charset.StandardCharsets.UTF_8.name());
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return null;
		}
		// Append <msg> field
		result.put("msg", text);
	
		Store                   st = sig.getCertificates();
		SignerInformationStore  signers = sig.getSignerInfos();
		Collection              c = signers.getSigners();
		Iterator                it = c.iterator();

		if (!it.hasNext()){
			return null;
		}
		
		SignerInformation signer = (SignerInformation)it.next();		
		Collection certCollection = st.getMatches(signer.getSID());
		Iterator certIt = certCollection.iterator();
		if (!certIt.hasNext()){
			return null;
		}
		
		X509CertificateHolder certHolder = (X509CertificateHolder)certIt.next();
		
		try {
			if (signer.verify(new JcaSimpleSignerInfoVerifierBuilder().setProvider(BouncyCastleProvider.PROVIDER_NAME).build(certHolder)))
			{
				X509Certificate cert = new JcaX509CertificateConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getCertificate(certHolder);
				result.put("cert", EncodeCertificate(cert));
				return result;
			}
		} catch (OperatorCreationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return null;
	}
	
	public boolean LoadCertificate(String certPEMData)
	{
		cert_ = DecodeCertificate(certPEMData);
		return cert_ != null;
	}
	
	public boolean LoadPrivateKey(String keyPEMData, String passphrase)
	{
		key_ = DecodePrivateKey(keyPEMData, passphrase);
		return key_ != null;
	}

	public String GetCertificate()
	{
		return EncodeCertificate(cert_);
	}
	
	public String GetPrivateKey(String passphrase)
	{  
        return EncodePrivateKey(key_, passphrase);
	}
	
////////////////////////////////////////////////////////////////////////////////////////
	public static String MEncrypt(ArrayList<String> certs, String text)
	{
		CMSTypedData msg = null;
		try {
			msg = new CMSProcessableByteArray(text.getBytes(java.nio.charset.StandardCharsets.UTF_8.name()));
		} catch (UnsupportedEncodingException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return "";
		}
				
	   	CMSEnvelopedDataGenerator edGen = new CMSEnvelopedDataGenerator();
		
		for (String pemCert : certs){
			X509Certificate cert = DecodeCertificate(pemCert);
			
			JceKeyTransRecipientInfoGenerator recipGen = null;			
			try {
				recipGen = new JceKeyTransRecipientInfoGenerator(cert);
			} catch (CertificateEncodingException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}			
			if (recipGen == null){
				continue;
			}
			recipGen.setProvider(BouncyCastleProvider.PROVIDER_NAME);
			
			edGen.addRecipientInfoGenerator(recipGen);
		}
			
	   	CMSEnvelopedData ed = null;
		try {
			ed = edGen.generate(msg, new JceCMSContentEncryptorBuilder(CMSAlgorithm.DES_EDE3_CBC).setProvider(BouncyCastleProvider.PROVIDER_NAME).build());
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return "";
		}
	   
		try {
			return Base64.encode(ed.getEncoded());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return "";
	}

	public static X509Certificate DecodeCertificate(String certPEMData)
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
		try {
			return new JcaX509CertificateConverter().setProvider(BouncyCastleProvider.PROVIDER_NAME).getCertificate(x509CertificateHolder);
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
        return null;
	}
	
	public static PrivateKey DecodePrivateKey(String keyPEMData, String passphrase)
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
		if (o == null || (!(o instanceof PEMEncryptedKeyPair) && !(o instanceof PEMKeyPair)))
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
		return kp.getPrivate();	
	}
	
	
	public static String EncodeCertificate(X509Certificate cert)
	{
		StringWriter sw = new StringWriter();
        JcaPEMWriter pw = new JcaPEMWriter(sw);
        try {
        	pw.writeObject(cert);
			pw.close();
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

	public static String EncodePrivateKey(PrivateKey key, String passphrase)
	{  
        StringWriter sw = new StringWriter();
        JcaPEMWriter pw = new JcaPEMWriter(sw);
        try {
        	if (passphrase != null && !passphrase.isEmpty()){
        		pw.writeObject(new JcaMiscPEMGenerator(key, new JcePEMEncryptorBuilder("AES-128-ECB").setSecureRandom(new SecureRandom()).build(passphrase.toCharArray())));
        	}else{
        		pw.writeObject(key);
        	}
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
