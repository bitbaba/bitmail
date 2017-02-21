/**
 * 
 */
package com.bitbaba.core;

import android.os.Environment;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;

/**
 * @author Administrator
 *
 */
public class BitMail {

	private EMailClient emailClient_ = null;
	private X509Cert    profile_     = null;
	private String      email_       = null;
	private String      passphrase_  = null;
	private String      password_    = null;

	/**
	 * TODO: try to use a JSONObject to hold all configurations
	 */
	JSONObject config_ = new JSONObject();

	private static final BitMail __gsInstance = new BitMail();

	private BitMail()
	{

	}

	public static BitMail GetInstance(){
		return  __gsInstance;
	}

	public void SetEMail(String email){
		email_ = email;
	}

	public void SetPassphrase(String passphrase){
		passphrase_ = passphrase;
	}

	public void SetPassword(String password){
		password_ = password;
	}

	public boolean CreateProfile(){
		if (email_.isEmpty() || email_.indexOf("@") == -1){
			return  false;
		}
		if (passphrase_.isEmpty()){
			return  false;
		}
		if (password_.isEmpty()){
			return  false;
		}
		String nick = email_.split("@")[0];
		profile_ =  new X509Cert(nick, email_, 2048);

		/**
		 * Populate with test configurations
		 */

		PopulateTestConfiguration();

		return  true;
	}

	public boolean Init(){
		if (emailClient_ == null){
			emailClient_ = new EMailClient(email_, password_);
		}
		if (profile_ == null){
			profile_ = new X509Cert();
			JSONObject joProfile = null;
			if (!config_.has("Profile")){
				return  false;
			}
			joProfile = config_.optJSONObject("Profile");
			if (!joProfile.has("cert")){
				return  false;
			}
			String sCert = joProfile.optString("cert");
			profile_.LoadCertificate(sCert);
			if (!joProfile.has("key")){
				return  false;
			}
			String sKey = joProfile.optString("key");
			profile_.LoadPrivateKey(sKey, passphrase_);
		}
		return true;
	}

	public void LoadProfile(){
		File file = new File(Environment.getExternalStorageDirectory() + "/" + "bitmail.profile");

		FileInputStream fis = null;
		try {
			fis = new FileInputStream(file);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		int length = 0;
		try {
			length = fis.available();
		} catch (IOException e) {
			e.printStackTrace();
		}

		byte [] buffer = new byte[length];
		try {
			fis.read(buffer);
		} catch (IOException e) {
			e.printStackTrace();
		}

		String res = null;
		try {
			res = new String(buffer, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		try {
			fis.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		Log.d("BitMail", res);

		try {
			config_ = new JSONObject(res);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}

	public void SaveProfile() {
		File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "bitmail.profile");

		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(file);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		try {
			fos.write(config_.toString().getBytes());
		} catch (IOException e) {
			e.printStackTrace();
		}

		try {
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return ;
	}

	public List<String> GetFriends(){
		JSONObject joFriends = config_.optJSONObject("friends");
		ArrayList<String> list = new ArrayList<>();
		Iterator<String> itKeys = joFriends.keys();
		while(itKeys.hasNext()){
			String key = itKeys.next();
			list.add(key);
			// Dump it
			JSONObject joFriend = joFriends.optJSONObject(key);
			Log.d("BitMail", key + " : " + joFriend.optString("cert"));
		}
		return list;
	}

	public boolean SendMessage(String recipEmail, String message)
	{
		X509Cert xcert = new X509Cert();
		if (!config_.has("friends")){
			return false;
		}
		JSONObject joFriends = config_.optJSONObject("friends");
		if (!joFriends.has(recipEmail)){
			return false;
		}
		JSONObject joRecip = joFriends.optJSONObject(recipEmail);
		if (!joRecip.has("cert")){
			return false;
		}
		String sCertInPem = joRecip.optString("cert");

		if (sCertInPem.isEmpty()){
			return  false;
		}

		xcert.LoadCertificate(sCertInPem);

		emailClient_.Send(recipEmail, xcert.Encrypt(profile_.Sign(message)));

		return  true;
	}

	public void ReceiveMessage()
	{
		JSONArray jaMsg = null;
		if (config_.has("msg")){
			jaMsg = config_.optJSONArray("msg");
		}else{
			jaMsg = new JSONArray();
		}

		List<String> results = emailClient_.Receive();

		for (String elt : results){
			jaMsg.put(elt);
		}

		try {
			config_.put("msg", jaMsg);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}


	private X509Cert LoadAlice(String passphrase){
		String cert = "-----BEGIN CERTIFICATE-----\r\n"
				+"MIICUDCCAbmgAwIBAgIBATANBgkqhkiG9w0BAQUFADAyMQ4wDAYDVQQDDAVuaWNr\r\n"
				+"MTEgMB4GCSqGSIb3DQEJARYRbmljazFAZXhhbXBsZS5jb20wIBcNMTcwMjA0MDc0\r\n"
				+"MDExWhgPMjA5OTAzMjYwNzQwMTFaMDIxDjAMBgNVBAMMBW5pY2sxMSAwHgYJKoZI\r\n"
				+"hvcNAQkBFhFuaWNrMUBleGFtcGxlLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAw\r\n"
				+"gYkCgYEArlGxqhWUp3Ry8juxyXQ1c567lO8/AWZklCLJmwGLYqZbiLVK6LT0WUPg\r\n"
				+"mg3gfO6UBjD3Ek0NSTtXcDjjlJgQ8kyrvjJRtLNal/bMl3Jqq7j0UWZ5DZfxeJwg\r\n"
				+"0T57Bj5WY5kZ2XIU0Fb4VGVHC8Jq5tMpI39FZWB+W5SdcTqRHwsCAwEAAaN0MHIw\r\n"
				+"HQYDVR0OBBYEFO44aq1DU7ilVECqipU4P2H6ASDtMB8GA1UdIwQYMBaAFO44aq1D\r\n"
				+"U7ilVECqipU4P2H6ASDtMAwGA1UdEwQFMAMBAf8wDgYDVR0PAQH/BAQDAgH+MBIG\r\n"
				+"A1UdJQEB/wQIMAYGBFUdJQAwDQYJKoZIhvcNAQEFBQADgYEAPJt1zIoNgrJoAF7P\r\n"
				+"Jg/DnTC/KLpBtcoKpU5XkSsvdvyuia8RiAXlMQw68+UcVUxirDPqey9AB4G4yuOQ\r\n"
				+"sABnJ3+Pd953jpUlail3kfkmDvlj6RV7ha6XUt0gaEAqDzQmDDDKEix4+NmGcVxP\r\n"
				+"PR7Kk8yDrmSzL/HwDo6e2pVDLNc=\r\n"
				+"-----END CERTIFICATE-----\r\n";

		// Protected by `secret' in Parameter `passphrase'
		String key = "-----BEGIN RSA PRIVATE KEY-----\r\n"
				+"Proc-Type: 4,ENCRYPTED\r\n"
				+"DEK-Info: AES-128-ECB,1738F84C7BD4828A5D7B63197CBE658B\r\n"
				+"\r\n"
				+"2idnB10nJwWlymDlQyZeqwbYGtJlX1Xl2iupQp/mYL7EwXXV3yiqXbi4EZLZ492w\r\n"
				+"PCHpsbqX+jW4hqAxpZA0L0JfeYsPVXQOkiCIklwB9K9327yCHcq12KWYAJ5eqvN3\r\n"
				+"/9Eab+ojZY3Bz6tiPZUXmwKByrZ3DvG2QJtdGeM09uq4J/T4/HFmMJRpxB7FKna1\r\n"
				+"eHOTMegWrL276lID4yxOzY28bVfO+7UPIGWEM6VaRnXdk0c3beXkrWTKLvteluGs\r\n"
				+"3HbvWmTwissbFpHhCqkvqLtdSIQZVBZggbo+X9PW+R8D/Kgl6V1a8k+8Zg2cu4Th\r\n"
				+"72BYQzvvoB+pk802Rl0chmctLOzlCx+8zevLHFb0seW34LQyVI/BcXHfCr20lXa/\r\n"
				+"oi+sbekfvzqknVNmxSpHifCF4Rl32O78a4tVvcuTq+CXH8slEFS2vEHaiUV9BXJR\r\n"
				+"7vRnx5T+bDyRrmxiGULrNNMOL0jB9nAe5yQNtER/Qmt++guyIEmkAqwiVD4OE9og\r\n"
				+"XjWwgYvbZUwF5936kFJpCIHITPxfTQE1lgTeJRcQFru9pcMN0gxWw77rpxDnWHqE\r\n"
				+"ZWsvrd23URa9P4nXQ3VlggSrSv7/bX98/9EZu3prqAUBqhoLSgAF0jT7jxNsrtrR\r\n"
				+"zJXUew3FUceL8wNtMDozPSCb19p3OYnOVDS5+HO4WB3ohVkv9AeAicyBZRwrSf8s\r\n"
				+"0mv3lyaa/2S71+xTxoKdh6y9UKkTtUpAjjWMG4TcV/wYOkqN+gRwZX9owLHw97QK\r\n"
				+"1H6chHKmA/ehbgD/YTzKTfwsEXMC4WfQ+X9KmE68D8RDMqlutQeqlmM6vnzsZXLo\r\n"
				+"-----END RSA PRIVATE KEY-----\r\n";

		X509Cert xcert = new X509Cert();
		xcert.LoadCertificate(cert);
		xcert.LoadPrivateKey(key, passphrase);

		return xcert;
	}

	private X509Cert LoadBob(String passphrase){
		String cert = "-----BEGIN CERTIFICATE-----\r\n"
				+"MIICUDCCAbmgAwIBAgIBATANBgkqhkiG9w0BAQUFADAyMQ4wDAYDVQQDDAVuaWNr\r\n"
				+"MjEgMB4GCSqGSIb3DQEJARYRbmljazJAZXhhbXBsZS5jb20wIBcNMTcwMjA0MDc0\r\n"
				+"MDEyWhgPMjA5OTAzMjYwNzQwMTJaMDIxDjAMBgNVBAMMBW5pY2syMSAwHgYJKoZI\r\n"
				+"hvcNAQkBFhFuaWNrMkBleGFtcGxlLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAw\r\n"
				+"gYkCgYEArnptHbF8GWRS5NwIfIf9uNRT4J5GGRlmFuraMpZrIIxbXAVxIj27mQhF\r\n"
				+"iWoKMXGba0iwDrYFFgq+kqpvRkmZgnkW7fND4F7atyoA9TrheRR18oN8eXi2NuV9\r\n"
				+"HtdKOG3bYpit/i9IgWaeVRuZqC6RcNO+iWb1OErj+5xrG3cvl+sCAwEAAaN0MHIw\r\n"
				+"HQYDVR0OBBYEFLDPZQHGWV/N2TmFpwSW0D1TLRmSMB8GA1UdIwQYMBaAFLDPZQHG\r\n"
				+"WV/N2TmFpwSW0D1TLRmSMAwGA1UdEwQFMAMBAf8wDgYDVR0PAQH/BAQDAgH+MBIG\r\n"
				+"A1UdJQEB/wQIMAYGBFUdJQAwDQYJKoZIhvcNAQEFBQADgYEAn5gbUn4z7BjqB3W3\r\n"
				+"FX+cire0IxP69jKSLHtLahb/+qxaZc3zwsLFkvs6eG1rYykoHvVl3Xp4kYAZ4UT0\r\n"
				+"jEW2IY7V1LEU+RplmdX1yguWFSJ4PFr9LMVvSov4csaf2yQkzOu3TXQOxl1ZpSTZ\r\n"
				+"kljyvHABJ1AjSZEOtqnRyGlSq8M=\r\n"
				+"-----END CERTIFICATE-----\r\n";

		String key = "-----BEGIN RSA PRIVATE KEY-----\r\n"
				+"MIICWwIBAAKBgQCuem0dsXwZZFLk3Ah8h/241FPgnkYZGWYW6toylmsgjFtcBXEi\r\n"
				+"PbuZCEWJagoxcZtrSLAOtgUWCr6Sqm9GSZmCeRbt80PgXtq3KgD1OuF5FHXyg3x5\r\n"
				+"eLY25X0e10o4bdtimK3+L0iBZp5VG5moLpFw076JZvU4SuP7nGsbdy+X6wIDAQAB\r\n"
				+"AoGAD+aHUCfvE89RQJsOyO6JwFDVTgiOgdYPODJx7R9JJv+zibjTPRgcIw+eaq33\r\n"
				+"vNbKH6ZRqR2MbBxRjsMqfeDSNOu3huZCDleqXRdufg+nW/gmXmZEyAUxys4Wxuu/\r\n"
				+"iD7Tc0+v5+8SkOzYnQC7aDcCvgZL2lNS0MNClx4H/1FP/1ECQQDUxEuyGk979LG4\r\n"
				+"7q7MKQvJt6lXEhzu8dY/knAkpdP87jQx1JORv1GtqsvwrYf/ivZ+nofy/O3ytYdu\r\n"
				+"B9BzskERAkEA0e5vr23+HQ4Q2iATzg0pe2vboYKAoUOd4hfRHabmbftiboMxLgGL\r\n"
				+"0VE9trlrwUEy5kPKiayMLdFLzIbEy8EJOwJAWDOHra16hchKZH+NttTNLjIU4SOB\r\n"
				+"Gwzl5DSwiIC2AIQ+3ulgw2qsUXIcWQmW9ziENrlBYlm3yoW2Psa+gzfCAQJAWC97\r\n"
				+"BwyDdJ0WAyr6ZtIsnMYAKuZxNkz7I6CxnW0g2w6AmL/Phu3rV/BypjFZzIKg6Q2N\r\n"
				+"JSwwboa++pmaTaU3RQJAFxzNki1n7SkSjOk+iF4f0ec2ewW2bwGyMtjKz5xNzyx3\r\n"
				+"1FfVS0e1UPQKxUIPUGxEwu7QQP576jNNFbPs4MMq2g==\r\n"
				+"-----END RSA PRIVATE KEY-----\r\n";

		X509Cert xcert = new X509Cert();
		xcert.LoadCertificate(cert);
		xcert.LoadPrivateKey(key, passphrase);

		return  xcert;
	}

	private void PopulateTestConfiguration(){
		do { // Profile
			JSONObject joProfile = new JSONObject();
			do {
				JSONObject joBrad = new JSONObject();
				try {
					joBrad.put("port", 10086);
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joProfile.put("brad", joBrad);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			} while (false);

			do {
				try {
					joProfile.put("cert", profile_.GetCertificate());
				} catch (JSONException e) {
					e.printStackTrace();
				}
			} while (false);

			do {
				try {
					joProfile.put("email", profile_.GetEmail());
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);

			do {
				try {
					joProfile.put("key", profile_.GetPrivateKey(passphrase_));
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);

			do {
				try {
					joProfile.put("nick", profile_.GetNick());
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while (false);

			try {
				config_.put("Profile", joProfile);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do {// Friends
			JSONObject joFriends = new JSONObject();
			do {
				JSONObject joFriend = new JSONObject();
				try {
					joFriend.put("cert", profile_.GetCertificate());
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joFriends.put(profile_.GetEmail(), joFriend);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);
			try {
				config_.put("friends", joFriends);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do {// Groups
			JSONObject joGroups = new JSONObject();
			do {
				JSONObject joGroup = new JSONObject();
				try {
					joGroup.put("name", "BitMailCore");
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joGroup.put("creator", profile_.GetEmail());
				} catch (JSONException e) {
					e.printStackTrace();
				}
				JSONArray jaMembers = new JSONArray();
				do {
					jaMembers.put(profile_.GetEmail());
				}while(false);
				try {
					joGroup.put("members", jaMembers);
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joGroups.put(profile_.GetID() + "." + Calendar.getInstance().getTimeInMillis(), joGroup);
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);
			try {
				config_.put("groups", joGroups);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do { // Subscribes
			JSONArray jaSubscribes = new JSONArray();
			do {
				jaSubscribes.put(profile_.GetEmail());
			}while(false);
			try {
				config_.put("subscribes", jaSubscribes);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do {// Proxy
			JSONObject joProxy = new JSONObject();
			do {
				try {
					joProxy.put("ip", "127.0.0.1");
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joProxy.put("port", 1080);
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joProxy.put("user", profile_.GetNick());
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joProxy.put("password", profile_.Encrypt("secret"));
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);
			try {
				config_.put("proxy", joProxy);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do {// Tx
			JSONObject joTx = new JSONObject();
			do {
				try {
					joTx.put("login", profile_.GetEmail());
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joTx.put("url", EMailClient.GetTxUrl(profile_.GetEmail()));
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joTx.put("password", profile_.Encrypt(password_));
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);
			try {
				config_.put("tx", joTx);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);

		do {// Rx
			JSONObject joRx = new JSONObject();
			do {
				try {
					joRx.put("login", profile_.GetEmail());
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joRx.put("url", EMailClient.GetRxUrl(profile_.GetEmail()));
				} catch (JSONException e) {
					e.printStackTrace();
				}
				try {
					joRx.put("password", profile_.Encrypt(password_));
				} catch (JSONException e) {
					e.printStackTrace();
				}
			}while(false);
			try {
				config_.put("rx", joRx);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}while(false);
	}

	public void UnitTest() {
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

		emailClient_ = new EMailClient(email_, password_);

		profile_ = new X509Cert("nick", "nick@example.com", 2048);
		Log.d("BitMail", profile_.GetCertificate());

		X509Cert alice, bob;

		boolean freshCert = false;

		if (freshCert) {
			alice = new X509Cert("nick1", "nick1@example.com", 1024);
			bob = new X509Cert("nick2", "nick2@example.com", 1024);
		}else{
			alice = LoadAlice("secret");
			bob = LoadBob(null);
		}

		System.out.println(alice.GetCertificate());
		Log.d("BitMail", alice.GetPrivateKey(passphrase_));
		System.out.println(bob.GetCertificate());
		Log.d("BitMail", bob.GetPrivateKey(null));

		/*
		*/
		ArrayList<String> certs = new ArrayList<>();
		certs.add(alice.GetCertificate());
		certs.add(bob.GetCertificate());

		// On alice side: sign, encrypt, and send.
		String sig = alice.Sign("hello, world");
		Log.d("BitMail", sig);
		//String enc = bob.Encrypt(sig);
		String enc = X509Cert.MEncrypt(certs, sig);
		Log.d("BitMail", enc);
		emailClient_.Send(email_, enc);

		// On Bob side: receive, decrypt, and verify.
		List<String> messages = emailClient_.Receive();
		if (messages == null || messages.isEmpty()){
			return ;
		}
		for (String msg : messages){
			System.out.println(msg);
			HashMap<String, String> result = X509Cert.Verify(bob.Decrypt(msg));
			if (result != null){
				for (Entry<String, String> elt : result.entrySet()){
					Log.d("BitMail", elt.getKey());
					if (elt.getKey().compareTo("cert") == 0){
						String certpem = elt.getValue();
						X509Cert testcert = new X509Cert();
						testcert.LoadCertificate(certpem);
						Log.d("BitMail", testcert.GetEmail());
						Log.d("BitMail", testcert.GetNick());
						Log.d("BitMail", testcert.GetID());
						Log.d("BitMail", elt.getValue());
					}
				}
			}
		}
	}
}
