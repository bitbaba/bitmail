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
	private String      email_       = null;
	private String      passphrase_  = null;
	private String      password_    = null;

	private HashMap<String, String> friends_ = new HashMap<String, String>();

	private static final BitMail __gsInstance = new BitMail();

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
			HashMap<String, String> result = alice.Verify(bob.Decrypt(msg));
			if (result != null){
				for (Entry<String, String> elt : result.entrySet()){
					Log.d("BitMail", elt.getKey());
					Log.d("BitMail", elt.getValue());
				}
			}
		}
	}
	
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

	public void LoadProfile(){
		return ;
	}

	public void SaveProfile(){
		return ;
	}
}
