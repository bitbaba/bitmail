/**
 * 
 */
package com.bitbaba;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map.Entry;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

/**
 * @author Administrator
 *
 */
public class BitMail {

	private EMailClient emailClient_ = null;
	private X509Cert    profile_     = null;
	private String      passphrase_  = null;
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		/**
		 * Bouncy Castle provider test
		 */
		java.security.Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
		
		java.security.Provider [] providers = java.security.Security.getProviders();
		for(int i = 0; i < providers.length; i++){
			java.security.Provider provider = providers[i];
			//System.out.println(provider.getName());
		}
		
		EMailClient ec = new EMailClient("123123@qq.com", "123123");
		ec.Send("123123@qq.com", "Hi");
		
/*
		X509Cert xcert = new X509Cert("nick", "user@example.com", 1024);
		X509Cert xcert2= new X509Cert("nick2", "user2@example.com", 1024);
			
		System.out.println(xcert.GetCertificate());
		System.out.println(xcert.GetPrivateKey(""));
		System.out.println(xcert.GetPrivateKey("secret"));
		
		HashMap<String, String> result = (xcert.Verify(xcert.Decrypt(xcert.Encrypt(xcert.Sign("Hello, world")))));
		if (result != null){
			for (Entry<String, String> elt : result.entrySet()){
				System.out.println(elt.getKey());
				System.out.println(elt.getValue());
			}
		}
		ArrayList<String> certs = new ArrayList<String>();
		certs.add(xcert.GetCertificate());
		certs.add(xcert2.GetCertificate());
		
		String code = (X509Cert.MEncrypt(certs, "hello"));
		String text = xcert.Decrypt(code);
		System.out.println(text);*/	
		// Failed to decrypt with xcert2; why? try to fix it later;
		
		//System.out.println(xcert.Sign("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
        
	}
	
	public BitMail()
	{
		//setEmailClient(new EMailClient());
	}
	
	public int LoadProfile()
	{
		return 0;
	}

	/**
	 * @return the emailClient
	 */
	public EMailClient getEmailClient() {
		return emailClient_;
	}

	/**
	 * @param emailClient the emailClient to set
	 */
	public void setEmailClient(EMailClient emailClient) {
		this.emailClient_ = emailClient;
	}

	/**
	 * @return the profile
	 */
	public X509Cert getProfile() {
		return profile_;
	}

	/**
	 * @param profile the profile to set
	 */
	public void setProfile(X509Cert profile) {
		this.profile_ = profile;
	}

}
