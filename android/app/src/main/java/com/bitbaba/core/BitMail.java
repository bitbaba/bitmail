/**
 * 
 */
package com.bitbaba.core;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
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
		
		String xcert1 = "-----BEGIN CERTIFICATE-----\r\n"
		+"MIICUDCCAbmgAwIBAgIBATANBgkqhkiG9w0BAQUFADAyMQ4wDAYDVQQDDAVuaWNr\r\n"
		+"MTEgMB4GCSqGSIb3DQEJARYRbmljazFAZXhhbXBsZS5jb20wIBcNMTcwMTA2MDYz\r\n"
		+"MzI2WhgPMjA5OTAyMjUwNjMzMjZaMDIxDjAMBgNVBAMMBW5pY2sxMSAwHgYJKoZI\r\n"
		+"hvcNAQkBFhFuaWNrMUBleGFtcGxlLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAw\r\n"
		+"gYkCgYEAm+v8vDhEWpNad+mz1qh4zOrKpXz66WzF5lx3evtb+uwsHCa1kVpFBOBO\r\n"
		+"p11WKftY0O1p+A7zIPiWJ3S4Y5HxA8U7700q+orblpsh3+qp5RAh/da0UQda8LkO\r\n"
		+"O/hdYtYbISzX6AqSKMQtaAlr4t/9d88111Ko2J9ahs3HxvvRRvcCAwEAAaN0MHIw\r\n"
		+"HQYDVR0OBBYEFJDFZzwhEYGvrQdZLj4GkGVwUNHrMB8GA1UdIwQYMBaAFJDFZzwh\r\n"
		+"EYGvrQdZLj4GkGVwUNHrMAwGA1UdEwQFMAMBAf8wDgYDVR0PAQH/BAQDAgH+MBIG\r\n"
		+"A1UdJQEB/wQIMAYGBFUdJQAwDQYJKoZIhvcNAQEFBQADgYEAfH6W0l/dkcFK90CX\r\n"
		+"SSQy0mmsni2M0jBRaGzIefbVZYFt5BTsSKaJHY3wOYorJ5OPynEwH/Q6+ftQ5/V3\r\n"
		+"mtH6sARZfUGKI7+xu0yy0+AtY82kLlGa7IFaJqT6/1H9bWN066Sg9UCEEuqJzg+w\r\n"
		+"ebitnwZYunE2BjXnc8XO+jrgLTo=\r\n"
		+"-----END CERTIFICATE-----\r\n";

		String xkey1 = "-----BEGIN RSA PRIVATE KEY-----\r\n"
		+"Proc-Type: 4,ENCRYPTED\r\n"
		+"DEK-Info: AES-128-ECB,0F185364A1F888D59D28344C68E62503\r\n"
		+"\r\n"
		+"+yvWvUy70Nv3wj9KrMVQ3WdGuF0m1Onk2gkQoPAPqWIpcHoQhy6Xm/ao64kAC1XW\r\n"
		+"jAzkuhX05VZvnU8Wt11TIoFDozASU9vYja+chzp9BnAEu/IMHGhqdhQgywpCf09R\r\n"
		+"JkA07Aczk2kwoBvJXCv5qzARx+ANUvdyAa5dfhKOss60cCbNdCrHxHFuWNWv241G\r\n"
		+"Sid1wvS+SL7cvzRc/DFdAoOHQ8SrzS/G+gOn9FsXA101N67typOhgSe7mU8CZF28\r\n"
		+"n629ydVO1+VN6W3Nie3AzeOdARy/4e6kseHNdFEzO6jUeoWNV3/ba3p72X4i5HsQ\r\n"
		+"vqrj8BWhtEFEF0n3XQ7m8ZLjCIs/H0eIL8UE9kacVyVlzSTF3tB5tHFTxXh0+E42\r\n"
		+"9zzDLgHy6WYCjOnBddylTLlYZREzu+9+KB7qNL3UloHfy1QlJtSLwdxHb6FNidIR\r\n"
		+"gTXyf7+X9dNTVZLGH+qLoMUp30X8coLgqjVt7zfmS1MscZGpm4zWNw2sza0CkEP8\r\n"
		+"xucYWTA4t4j9aylbdwRcPSfSyEvazi8Jyj0g66HaCSlr8JHii1iPnG1TSjB5LBTO\r\n"
		+"fV6byuVRgd+kpixDGhtmFzbR0k608ipo2hRni2hOxYYoIX0hhVWILnJftdtMZEjg\r\n"
		+"DVHtgi8tjs5CjxopDmXgT2c6pm+XywYTqIwLr+x9zkMuigEg9p9t4mT8nem5xbt4\r\n"
		+"RooophBBqGNda75T1EqgPou6njgi0zh5plTJjnP3pi0Vrb5GLTgozhswaL8TGzKm\r\n"
		+"TLH9pQfm03ephLTqGqtwrUOuEJlyFIBeskZAZ8pLc/7P/AlD4oVI+quUhG3G3/aP\r\n"
		+"-----END RSA PRIVATE KEY-----\r\n";

		String xcert2 = "-----BEGIN CERTIFICATE-----\r\n"
		+"MIICUDCCAbmgAwIBAgIBATANBgkqhkiG9w0BAQUFADAyMQ4wDAYDVQQDDAVuaWNr\r\n"
		+"MjEgMB4GCSqGSIb3DQEJARYRbmljazJAZXhhbXBsZS5jb20wIBcNMTcwMTA2MDYz\r\n"
		+"MzI2WhgPMjA5OTAyMjUwNjMzMjZaMDIxDjAMBgNVBAMMBW5pY2syMSAwHgYJKoZI\r\n"
		+"hvcNAQkBFhFuaWNrMkBleGFtcGxlLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAw\r\n"
		+"gYkCgYEA0RTHLW7j7yjVIigMgIBPwBjtT185iskKHnBaxq9Q5VJZa0VlkHhB2oCl\r\n"
		+"msGRyuUXl2U75vwzoHk/L1gNEuoPaCiiunF9JzHhHEZKWpb+p3Edy7iknkqeSvaz\r\n"
		+"SFN7BfvdrschYbht/mCAenHec8b6TzwxyP+pucxiji0vn5J8CzECAwEAAaN0MHIw\r\n"
		+"HQYDVR0OBBYEFJApZzuMuQtr/QT9Barf8b5fciGVMB8GA1UdIwQYMBaAFJApZzuM\r\n"
		+"uQtr/QT9Barf8b5fciGVMAwGA1UdEwQFMAMBAf8wDgYDVR0PAQH/BAQDAgH+MBIG\r\n"
		+"A1UdJQEB/wQIMAYGBFUdJQAwDQYJKoZIhvcNAQEFBQADgYEADlmEGiCm4B5IrF7I\r\n"
		+"SC3d4dhvn2r8/oHhPdJetE5zQBXDqg/TMGdvveHxh2/l4KZj/cVPvXAfGiJRha0N\r\n"
		+"9xt7YsTnQggpgekqMBOSFiySL9dMzmK7BS7Dwxp09VCcmKkeXeP5PLB2RGP8vtRb\r\n"
		+"fCe+aOnjsQcYkZL3Px7/EwrkbbA=\r\n"
		+"-----END CERTIFICATE-----\r\n";

		String xkey2 = "-----BEGIN RSA PRIVATE KEY-----\r\n"
		+"MIICWwIBAAKBgQDRFMctbuPvKNUiKAyAgE/AGO1PXzmKyQoecFrGr1DlUllrRWWQ\r\n"
		+"eEHagKWawZHK5ReXZTvm/DOgeT8vWA0S6g9oKKK6cX0nMeEcRkpalv6ncR3LuKSe\r\n"
		+"Sp5K9rNIU3sF+92uxyFhuG3+YIB6cd5zxvpPPDHI/6m5zGKOLS+fknwLMQIDAQAB\r\n"
		+"AoGABajYU2SDCwBB9LC81eWjQwHpupJLTnRur7A+5M3uFKcMAJlaTHTU44ND+FJx\r\n"
		+"Ddrxrolw+6msBBYEHdX2VKK9U/ntSgHx0eMJaY4lWSrjwnNY/LXDxBVz10HxW94/\r\n"
		+"5d2N9dreMjrI9jIEFlmJx7Lb7mFsQvKvsU+Sez9aaNLki3kCQQDvdiJk9SXyvHCV\r\n"
		+"ffx2N7/pRCwb+Q89IZUrxdtJ0eLI3KkV3Dj4NG3h2rM6sO7rlkLUblev4ztexk4c\r\n"
		+"X12+U6I3AkEA34V/DNhDgQxXerJ911Xc+xaDOCn+SBYmYhJlTTre4HxYZ6qXubQV\r\n"
		+"Z4SI7dnuZbCkBUDvlmSnhHDXCMgmgtwp1wJAeWyd5DOrGnsTW7qW2htK5S38b/LL\r\n"
		+"F2SAD8I0gCo9pTy/QKZ6/OEZFv2fBJPTkwKOwMlzDW1cHaLQ2PJCxtAB4QJAIA5u\r\n"
		+"PTJa5AwYE7G7mIFo8t20rf5fyIjcsl3/B0d1JW7P4vY1Ul3AYBtC5BHIPe9KZrJ8\r\n"
		+"ae4/lB/2xvylgUj/+wJAMHXR0DQUNc4R482reLqQIsP+864MmTdBwSITVWj/mtMf\r\n"
		+"bQQWRzcc+blYYhvmFEPWQyRR8ESAHu/yEI8WnbFrlg==\r\n"
		+"-----END RSA PRIVATE KEY-----\r\n";
		
		//X509Cert cert1 = new X509Cert("nick1", "nick1@example.com", 1024);
		//X509Cert cert2 = new X509Cert("nick2", "nick2@example.com", 1024);
		
		X509Cert cert1 = new X509Cert();
		X509Cert cert2 = new X509Cert();
		
		cert1.LoadCertificate(xcert1);
		cert1.LoadPrivateKey(xkey1, "secret");
		
		cert2.LoadCertificate(xcert2);
		cert2.LoadPrivateKey(xkey2, null);
		
		System.out.println(cert1.GetCertificate());
		System.out.println(cert1.GetPrivateKey("secret"));

		System.out.println(cert2.GetCertificate());
		System.out.println(cert2.GetPrivateKey(null));
		
		ArrayList<String> certs = new ArrayList<String>();
		certs.add(cert1.GetCertificate());
		certs.add(cert2.GetCertificate());
		
		/**/
		String account = null, password = null, recipt = null;
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Account:");
		try {
			account = reader.readLine();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("Password:");
		try {
			password = reader.readLine();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		System.out.println("Recipt:");
		try {
			recipt = reader.readLine();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		EMailClient ec = new EMailClient(account, password);
		ec.Send(recipt, cert1.Encrypt(cert1.Sign("hello, world")));
		
		List<String> msgs = ec.Receive();		

		if (msgs == null || msgs.isEmpty()){
			return ;
		}
		
		for (String msg : msgs){
			System.out.println(msg);
			HashMap<String, String> result = cert1.Verify(cert1.Decrypt(msg));
			if (result != null){
				for (Entry<String, String> elt : result.entrySet()){
					System.out.println(elt.getKey());
					System.out.println(elt.getValue());
				}
			}			
		}		
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
