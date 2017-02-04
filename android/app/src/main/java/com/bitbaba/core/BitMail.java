/**
 * 
 */
package com.bitbaba.core;

import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

/**
 * @author Administrator
 *
 */
public class BitMail {

	private EMailClient emailClient_ = null;
	private X509Cert    profile_     = null;
	private String      passphrase_  = null;

	public void UnitTest() throws IOException {
		// TODO Auto-generated method stub

		/**
		 * 256bit AES/CBC/PKCS5Padding with Bouncy Castle
		 * http://stackoverflow.com/questions/5641326/256bit-aes-cbc-pkcs5padding-with-bouncy-castle
		 * http://stackoverflow.com/questions/2957513/how-to-use-bouncy-castle-lightweight-api-with-aes-and-pbe
		 */
		/**
		 * Bouncy Castle provider test
		 */
		java.security.Security.addProvider(new org.bouncycastle.jce.provider.BouncyCastleProvider());

		java.security.Provider [] providers = java.security.Security.getProviders();
		for(int i = 0; i < providers.length; i++){
			java.security.Provider provider = providers[i];
			Log.d("BitMail", provider.getName());
		}

		profile_ = new X509Cert("nick", "nick@example.com", 2048);
		Log.d("BitMail", profile_.GetCertificate());

		X509Cert cert1 = new X509Cert("nick1", "nick1@example.com", 1024);
		X509Cert cert2 = new X509Cert("nick2", "nick2@example.com", 1024);

		System.out.println(cert1.GetCertificate());
		Log.d("BitMail", cert1.GetPrivateKey(passphrase_));

		System.out.println(cert2.GetCertificate());
		Log.d("BitMail", cert2.GetPrivateKey(null));

		/*
		*/
		ArrayList<String> certs = new ArrayList<>();
		certs.add(cert1.GetCertificate());
		certs.add(cert2.GetCertificate());

		String account = "10000@qq.com", password = "10000", recipt = "10000@qq.com";
		emailClient_ = new EMailClient(account, password);
		String sig = cert1.Sign("hello, world");
		Log.d("BitMail", sig);
		String enc = cert1.Encrypt(sig);
		Log.d("BitMail", enc);
		emailClient_.Send(recipt, enc);
		List<String> messages = emailClient_.Receive();
		if (messages == null || messages.isEmpty()){
			return ;
		}
		for (String msg : messages){
			System.out.println(msg);
			HashMap<String, String> result = cert1.Verify(cert1.Decrypt(msg));
			if (result != null){
				for (Entry<String, String> elt : result.entrySet()){
					Log.d("BitMail", elt.getKey());
					Log.d("BitMail", elt.getValue());
				}
			}
		}
	}
	
	public BitMail()
	{
		passphrase_ = "secret";
	}
}
