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
		
		/*		*/
		X509Cert xcert = new X509Cert("nick", "user@example.com", 1024);
		System.out.println(xcert.GetCertificate());
		System.out.println(xcert.GetPrivateKey(""));
		System.out.println(xcert.GetPrivateKey("secret"));	
		
		System.out.println(xcert.Sign("Hello, world"));
		System.out.println(xcert.Encrypt("Hello, World!"));
		
		System.out.println(xcert.Decrypt(xcert.Encrypt("Hello, World!")));
		
		System.out.println(xcert.Verify(xcert.Sign("Hello, world")));
	}
	
	public BitMail()
	{
		setEmailClient(new EMailClient());
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
