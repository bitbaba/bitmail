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
		
		X509Cert cert1 = new X509Cert();
		if(cert1.LoadCertificate(xcert.GetCertificate())){
			System.out.println(cert1.GetCertificate());
		}
		if (cert1.LoadPrivateKey(xcert.GetPrivateKey("secret"), "secret")){
			System.out.println(cert1.GetPrivateKey(""));
			System.out.println(cert1.GetPrivateKey("secret"));
		}
		if (cert1.LoadPrivateKey(xcert.GetPrivateKey(""), "")){
			System.out.println(cert1.GetPrivateKey(""));
			System.out.println(cert1.GetPrivateKey("secret"));
		}

		X509Cert xcert2 = new X509Cert();
		
		String pemCert = "-----BEGIN CERTIFICATE-----\n"
		+"MIICTDCCAbWgAwIBAgIBATANBgkqhkiG9w0BAQUFADAwMQ0wCwYDVQQDDARuaWNr\n"
		+"MR8wHQYJKoZIhvcNAQkBFhB1c2VyQGV4YW1wbGUuY29tMCAXDTE2MTIxNDA3NTIw\n"
		+"M1oYDzIwOTkwMjAyMDc1MjAzWjAwMQ0wCwYDVQQDDARuaWNrMR8wHQYJKoZIhvcN\n"
		+"AQkBFhB1c2VyQGV4YW1wbGUuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKB\n"
		+"gQDBNCXpPtLNNX/au6SnJt7NQhbyUb5ENcqjSctNuRpKOU7zd1+39PxOSD07GkMz\n"
		+"xu88AnT7iC2Rd+d/coAG3ue8cUlr5RE5u8RGxtEM4i5zWT32WJD269nljCKE/uVa\n"
		+"sb5OM7rB7l/RASe/nOGD2oVUQByGAsmgCKieV79xkeTV9wIDAQABo3QwcjAdBgNV\n"
		+"HQ4EFgQUvc3aPMvqUayRUpORMGrNpvV9QQIwHwYDVR0jBBgwFoAUvc3aPMvqUayR\n"
		+"UpORMGrNpvV9QQIwDAYDVR0TBAUwAwEB/zAOBgNVHQ8BAf8EBAMCAf4wEgYDVR0l\n"
		+"AQH/BAgwBgYEVR0lADANBgkqhkiG9w0BAQUFAAOBgQARYxV8aNiBWkRMemt6Q0BA\n"
		+"0C/dDcZUfVl+WdODtO7awxD/ZPAN+jK4RFStIn11uYUnId2MQztHHl4IqpjT/2JU\n"
		+"N+lyfSFVJTkzrSxuCOCJGjNA+1dCAnIX0cIKv5N9wY27jRpyRFO88vqgTMsh2Wxr\n"
		+"HuYilCgQxCsUNfWeLFAPnA==\n"
		+"-----END CERTIFICATE-----\n";
		
		if (xcert2.LoadCertificate(pemCert)){
			System.out.println(xcert2.GetCertificate());
		}

		String pemKey = "-----BEGIN RSA PRIVATE KEY-----\n"
		+"MIICWwIBAAKBgQDBNCXpPtLNNX/au6SnJt7NQhbyUb5ENcqjSctNuRpKOU7zd1+3\n"
		+"9PxOSD07GkMzxu88AnT7iC2Rd+d/coAG3ue8cUlr5RE5u8RGxtEM4i5zWT32WJD2\n"
		+"69nljCKE/uVasb5OM7rB7l/RASe/nOGD2oVUQByGAsmgCKieV79xkeTV9wIDAQAB\n"
		+"AoGAVuflq7sKKKuJhPMF9/hInUy+tKZnbxhBO90cWaxLMpr/tA183WsSOQZOoLDK\n"
		+"DNbN0xEJ3QEe+kHUvKPdCfong3f82Ye/kEZtG5Ae4C6+LqSiLxenHX5hE4FtTwPh\n"
		+"QD2NCJCUnLuaDOhkaFtAdSNb1eFs3z/BERuclkvjyHbDm8ECQQD0XF86YaynjT74\n"
		+"5+0oc1P0PqmCryCqXMCMyJd9L+paciqqdFtBSHnqpUcwIJA1QJRkAqK7azk8iXqe\n"
		+"xqt869zBAkEAymf9HONivGG0KZM8jQ/Gd7Vc0vrbzhCZkBJfQbs9G2xWjKWu/YP9\n"
		+"b087jql5umfYANzWeAJq33jk0ffNa2QItwJAcQk509vS7j7cXm+KuSeAMn+6EgXI\n"
		+"6oW1XGp+CR7dHbGqIudoPy6zluOVpqGpk7in7At8E45qD6LY4hGsykChAQJAPg8Q\n"
		+"PlFBrrzOPokUYbPaiE2lPlMIHfBdBR1882xzWfsntkv+kWBszjoe6qjv18WLJwiP\n"
		+"AZDML+SjU/WEmE69lQJAQKGYINilp4MxUG/zOH+1Q7A90+iNXYlEX0OSEJDyPvHB\n"
		+"5d4QMvLKR4CDsPCRBQY3i0+Ht6Nd8+vX26CYZqbh7w==\n"
		+"-----END RSA PRIVATE KEY-----\n";
		
		if (xcert2.LoadPrivateKey(pemKey, "")){
			System.out.println(xcert2.GetPrivateKey(""));
		}
		
		String encPemKey = "-----BEGIN RSA PRIVATE KEY-----\n"
		+"Proc-Type: 4,ENCRYPTED\n"
		+"DEK-Info: AES-128-ECB,6980780160E773C8F2BED85B36913CD3\n"
		+"\n"
		+"NPqL07o/tqLR12L9rGBw+3SDkPb+tsGGVop47mxhOe1+4uY0AajRD6TDgfmZjf2a\n"
		+"6iQcrbb4tzv1ZM6gO8etQyRJxeVxuvX6Ho4ScoHsGR4OcV2vdLz7dtpvgQaogQrH\n"
		+"7Wv7SACdQHGd3tAFfIdjVUXEW6jcXp3gh0jLBONeR8QQQgPdl2up71ywLsGkXO9I\n"
		+"ZXlKJJFQcC90dAem4RwgdlqcmNXxoauMGL9tV9b7O/rAlEY1yRROTfS3KjDAXHu7\n"
		+"U4iFeLmN4d1zUQYPMNf2YozPT3lNF9urlL/7sFr6Hmqf+W8I1zwX5Kx1BnrdflHn\n"
		+"/yePc7/MYvK/t2JB73RY+RWPrEolJbltaGai5IgwRVMs0aoWO/pMZKCwceqGJ4S0\n"
		+"/HQQhhpumSOTp5xFyob303lvz9jtQq0eo2gxaU0BxJpZzh75guMQdmJhWBbzNaiw\n"
		+"W1f5/6SjTbGw1mSb2GluKeiCXwmV7vgmjD9ArAXA1NRBZBdncw+C5D3mcu9zfXyK\n"
		+"c5PHlNgoJen5zLUqO83pPG555N91CcQpST3rXz1M0murlFA7z8mof5P83hTzJn/Q\n"
		+"xIG/4iS85rKnt7O83IYKnA2eCMr1y0BH9M4GsAztkgUrBEBeX7X8Fy6NEL7lIlJU\n"
		+"hmBpOfZvk+91MNA3K3dKFiLLYk3xgXFb4p40Q07m/imbTfhc6E1vAEXaIu/RRAGL\n"
		+"yetoE9Im/los/jthfLysSVazNCHXx2gyx/ddOHpYhBcrTexjnaj2EvY+1etJVGkA\n"
		+"JsiTMDlHJ0zXhJ8I+9V12/XJmRmvEIrNEO/hXuuND+w=\n"
		+"-----END RSA PRIVATE KEY-----\n";
		
		X509Cert xcert3 = new X509Cert();
		if (xcert3.LoadPrivateKey(encPemKey, "secret")){
			System.out.println(xcert3.GetPrivateKey(""));
		}
		
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
