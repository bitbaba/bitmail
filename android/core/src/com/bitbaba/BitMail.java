/**
 * 
 */
package com.bitbaba;

import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

/**
 * @author Administrator
 *
 */
public class BitMail {

	private EMailClient emailClient = null;
	private X509Cert    profile = null;
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		BitMail bm = new BitMail();
		bm.LoadProfile();
		
		/**
		 * Bouncy Castle provider test
		 */
		//java.security.Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());
		
		java.security.Provider [] providers = java.security.Security.getProviders();
		for(int i = 0; i < providers.length; i++){
			java.security.Provider provider = providers[i];
			System.out.println(provider.getName());
		}
		

		String input= "helloworld";
		String pass = "passphrase";
		java.security.SecureRandom random = new java.security.SecureRandom(pass.getBytes());
		javax.crypto.KeyGenerator keyGenerator = null;
		try {
			keyGenerator = javax.crypto.KeyGenerator.getInstance("AES");
		} catch (NoSuchAlgorithmException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		keyGenerator.init(256, random);
		javax.crypto.SecretKey key = keyGenerator.generateKey();
		byte [] keyBytes = key.getEncoded();
		javax.crypto.spec.SecretKeySpec keySpec = new javax.crypto.spec.SecretKeySpec(keyBytes, "AES");
		javax.crypto.Cipher ciper = null;
		try {
			try {
				//ciper = javax.crypto.Cipher.getInstance("AES", "BC");
				ciper = javax.crypto.Cipher.getInstance("AES", "SunJCE");
			} catch (NoSuchProviderException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} catch (NoSuchAlgorithmException | NoSuchPaddingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			ciper.init(javax.crypto.Cipher.ENCRYPT_MODE, keySpec);
		} catch (InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		byte[] result = null;
		try {
			result = ciper.doFinal(input.getBytes("utf-8"));
		} catch (IllegalBlockSizeException | BadPaddingException | UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println(result);
	}
	
	public BitMail()
	{
		setEmailClient(new EMailClient());
		setProfile(new X509Cert());
	}
	
	public int LoadProfile()
	{
		return 0;
	}

	/**
	 * @return the emailClient
	 */
	public EMailClient getEmailClient() {
		return emailClient;
	}

	/**
	 * @param emailClient the emailClient to set
	 */
	public void setEmailClient(EMailClient emailClient) {
		this.emailClient = emailClient;
	}

	/**
	 * @return the profile
	 */
	public X509Cert getProfile() {
		return profile;
	}

	/**
	 * @param profile the profile to set
	 */
	public void setProfile(X509Cert profile) {
		this.profile = profile;
	}

}
