/**
 * 
 */
package com.bitbaba.core;

import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.util.JsonReader;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Calendar;
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
	private String      email_       = null;
	private String      passphrase_  = null;
	private String      password_    = null;

	/**
	 * TODO: try to use a JSONObject to hold all configurations
	 */
	JSONObject config_ = new JSONObject();

	/**
	 * Key: email
	 * Value: certificate in pem encoding
	 */
	private HashMap<String, String> friends_ = new HashMap<String, String>();

	/**
	 * Messages cache
	 * element: raw s/mime message
	 */
	private ArrayList<String> messages_ = new ArrayList<>();

	/**
	 * Subscribes
	 * element: email to identify subscriber
	 */
	private ArrayList<String> subscribes_ = new ArrayList<>();

	/**
	 * Groups
	 * Key: groupId
	 * Value: json object contain details of group
	 *       {
	 *       	 name    : 'BitMailCore',
	 *           creator : 'someone@example.net',
	 *           members : [
	 *           			'friend1@example.net',
	 *           			'friend2@example.net'
	 *           ]
	 *       }
	 */
	private HashMap<String, JSONObject> groups_ = new HashMap<>();

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
		profile_ = new X509Cert(nick, email_, 2048);

		/**
		 * Make some test configuration
		 */
		friends_.put(profile_.GetEmail(), profile_.GetCertificate());

		JSONObject joGroup = new JSONObject();
		try {
			joGroup.put("creator", profile_.GetEmail());
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joGroup.put("name", "BitMailCore");
		} catch (JSONException e) {
			e.printStackTrace();
		}
		JSONArray jaMembers = new JSONArray();
		jaMembers.put(profile_.GetEmail())
		try {
			joGroup.put("members", jaMembers);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		groups_.put(profile_.GetID() + Calendar.getInstance().getTimeInMillis(), joGroup);

		subscribes_.add(profile_.GetEmail());

		return  true;
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
		Log.d("BitMail", res);

		JSONObject jroot = null;
		try {
			jroot = new JSONObject(res);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		if (jroot.has("Profile")){
			JSONObject joProfile = null;
			try {
				joProfile = jroot.getJSONObject("Profile");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("friends")){
			JSONObject joFriends = null;
			try {
				joFriends = jroot.getJSONObject("friends");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("groups")){
			JSONObject joGroups = null;
			try {
				joGroups = jroot.getJSONObject("groups");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("subscribes")){
			JSONObject joSubscribes = null;
			try {
				joSubscribes = jroot.getJSONObject("subscribes");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("proxy")){
			JSONObject joProxy = null;
			try {
				joProxy = jroot.getJSONObject("proxy");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("rx")){
			JSONObject joRx = null;
			try {
				joRx = jroot.getJSONObject("rx");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		if (jroot.has("tx")){
			JSONObject joTx = null;
			try {
				joTx = jroot.getJSONObject("tx");
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}

		try {
			fis.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void SaveProfile() {
		JSONObject jroot = new JSONObject();
		// Profile
		JSONObject jprofile = new JSONObject();
		try {
			JSONObject jbrad = new JSONObject();
			try {
				jbrad.put("port", 10086);
			} catch (JSONException e) {
				e.printStackTrace();
			}
			jprofile.put("brad", jbrad);
			jprofile.put("cert", profile_.GetCertificate());
			jprofile.put("email", email_);
			jprofile.put("key", profile_.GetPrivateKey(passphrase_));
			jprofile.put("nick", profile_.GetNick());
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			jroot.put("Profile", jprofile);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// Friends
		JSONObject jfriends = new JSONObject();
		for (Entry<String, String> elt : friends_.entrySet()){
			JSONObject jfriend = new JSONObject();
			try {
				jfriend.put("cert", elt.getValue());
			} catch (JSONException e) {
				e.printStackTrace();
			}
			try {
				jfriends.put(elt.getKey(), jfriend);
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}
		try {
			jroot.put("friends", jfriends);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// Groups
		JSONObject joGroups = new JSONObject();
		for (Entry<String, JSONObject> elt : groups_.entrySet()){
			try {
				joGroups.put(elt.getKey(), elt.getValue());
			} catch (JSONException e) {
				e.printStackTrace();
			}
		}
		try {
			jroot.put("groups", joGroups);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// Proxy
		JSONObject joProxy = new JSONObject();
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
			joProxy.put("user", "");
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joProxy.put("password", profile_.Encrypt("secret"));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			jroot.put("proxy", joProxy);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// Subscribes
		JSONArray jaSubscribes = new JSONArray();
		for (String elt : subscribes_){
			jaSubscribes.put(elt);
		}
		try {
			jroot.put("subscribes", jaSubscribes);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// rx
		JSONObject joRx = new JSONObject();
		try {
			joRx.put("login", email_);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joRx.put("password", profile_.Encrypt(password_));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joRx.put("url", EMailClient.GetRxUrl(email_));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			jroot.put("rx", joRx);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		// tx
		JSONObject joTx = new JSONObject();
		try {
			joTx.put("login", email_);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joTx.put("password", profile_.Encrypt(password_));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			joTx.put("url", EMailClient.GetTxUrl(email_));
		} catch (JSONException e) {
			e.printStackTrace();
		}
		try {
			jroot.put("tx", joTx);
		} catch (JSONException e) {
			e.printStackTrace();
		}

		File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "bitmail.profile");

		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(file);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		try {
			fos.write(jroot.toString().getBytes());
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
		ArrayList<String> list = new ArrayList<>();
		for (String elt : friends_.keySet()){
			list.add(elt);
		}
		return list;
	}

	public boolean SendMessage(String recipEmail, String message)
	{
		return  true;
	}

	public boolean SendMessage(List<String> recips, String message)
	{
		return  true;
	}

	public void ReceiveMessage()
	{

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
