#include <bitmailcore/bitmail.h>
#include <bitmailcore/email.h>
#include <bitmailcore/x509cert.h>

#define CURL_STATICLIB
#include <curl/curl.h>

# include <openssl/cms.h>
# include <openssl/bio.h>
# include <openssl/err.h>
# include <openssl/bn.h>
# include <openssl/rsa.h>
# include <openssl/evp.h>
# include <openssl/x509.h>
# include <openssl/pem.h>
# include <openssl/rand.h>
# include <openssl/pkcs7.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <string.h>

BitMail::BitMail()
: m_cb(NULL), m_cbp(NULL)
{
    OpenSSL_add_all_ciphers();
    OPENSSL_load_builtin_modules();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

BitMail::~BitMail()
{
    if (m_mc != NULL){
        delete (m_mc);
        m_mc = NULL;
    }
}

/**
* Callback & params
*/
int BitMail::OnBitMailEvent( BMEventCB cb, void * cbparam)
{
    m_cb = cb;
    m_cbp = cbparam;
    return 0;
}

/**
* Network initialize
*/
int BitMail::InitNetwork( const std::string & txurl
                    , const std::string & txuser
                    , const std::string & txpass
                    , const std::string & rxurl
                    , const std::string & rxuser
                    , const std::string & rxpass)
{
    m_mc = new CMailClient(EmailHandler, this);
    if (m_mc == NULL){
        return bmOutMem;
    }
    if (m_mc->InitTx(txurl, txuser, txpass)){
        return bmTxFail;
    }

    if (m_mc->InitRx(rxurl, rxuser, rxpass)){
        return bmRxFail;
    }
    return bmOk;
}

int BitMail::SendMsg(const std::string &email_to, const std::string &msg)
{
    std::string sSignedMsg = m_profile->Sign(msg);
    if (sSignedMsg.empty()){
        return bmSignFail;
    }

    std::string sEncryptedMsg = "";
    if (m_buddies.find(email_to) != m_buddies.end()){
        CX509Cert buddy;
        buddy.LoadCertFromPem(m_buddies[email_to]);
        if (buddy.IsValid()){
            sEncryptedMsg = buddy.Encrypt(sSignedMsg);
            if (sEncryptedMsg.empty()){
                return bmEncryptFail;
            }
        }
    }

    if (m_mc->SendMessage( m_profile->GetEmail()
                            , email_to
                            , sEncryptedMsg.empty()
                            ? sSignedMsg
                            : sEncryptedMsg)){
        return bmTxFail;
    }
    return bmOk;
}

int BitMail::GroupMsg(const std::vector<std::string> &email_to, const std::string &msg)
{
    /**
     * Note:
     * GroupMsg(msg, vector<bob>) != SendMsg(msg, bob);
     * GroupMsg must encrypt msg before send it out.
     */
    std::string sSignedMsg = m_profile->Sign(msg);
    if (sSignedMsg.empty()){
        return bmSignFail;
    }

    std::vector<CX509Cert> vecTo;
    for (std::vector<std::string>::const_iterator it = email_to.begin();
            it != email_to.end();
            ++it)
    {
        if (m_buddies.find(*it) != m_buddies.end()){
            CX509Cert buddy;
            buddy.LoadCertFromPem(m_buddies[*it]);
            if (!buddy.IsValid()){
                return bmInvalidCert;
            }
            vecTo.push_back(buddy);
        }else{
            //TODO: Ignore, and send signed-only message?
            // Or, Abort the entire process.
            // It seems abnormal to Group-Send out Signed-only message.
            return bmNoBuddy;
        }
    }

    std::string sEncryptedMsg = "";
    if (vecTo.size())
    {
        sEncryptedMsg = CX509Cert::MEncrypt(sSignedMsg, vecTo);
        if (sEncryptedMsg.empty()){
            return bmEncryptFail;
        }
    }

    if (m_mc->SendMessage( m_profile->GetEmail()
                            , email_to
                            , sEncryptedMsg)){
        return bmTxFail;
    }
    return bmOk;
}

int BitMail::CheckInbox()
{
    if (m_mc->CheckInbox()){
        return bmRxFail;
    }
    return bmOk;
}

int BitMail::StartIdle(unsigned int timeout)
{
	if (m_mc->StartIdle(timeout)){
		return bmIdleFail;
	}
	return bmOk;
}

/**
* Profile
*/
int BitMail::CreateProfile(const std::string & commonName
        , const std::string & email
        , const std::string & passphrase
        , unsigned int bits)
{
    m_profile = new CX509Cert();
    if (m_profile == NULL){
        return bmOutMem;
    }
    if (m_profile->Create(commonName, email, passphrase, bits)){
        return bmNewProfile;
    }
    return bmOk;
}

int BitMail::LoadProfile(const std::string & passphrase
                         , const std::string & keyPem
                         , const std::string & certPem)
{
    m_profile = new CX509Cert();
    if (m_profile == NULL){
        return bmOutMem;
    }
    if (m_profile->LoadKeyFromEncryptedPem(keyPem, passphrase)){
        return bmInvalidProfile;
    }
    if (m_profile->LoadCertFromPem(certPem)){
        return bmInvalidCert;
    }
    return bmOk;
}

std::string BitMail::GetEmail() const
{
    return m_profile->GetEmail();
}

std::string BitMail::GetCommonName() const
{
    return m_profile->GetCommonName();
}

std::string BitMail::GetCert() const
{
    return m_profile->GetCertByPem();
}

/**
* Buddy
*/
int BitMail::AddBuddy(const std::string &certpem)
{
    CX509Cert cert;
    cert.LoadCertFromPem(certpem);
    if (!cert.IsValid()){
        return bmInvalidCert;
    }
    const std::string & email = cert.GetEmail();
    m_buddies[email] = certpem;
    return bmOk;
}

std::string BitMail::GetBuddyCert(const std::string &buddy)
{
    if (m_buddies.find(buddy) != m_buddies.end()){
        return m_buddies[buddy];
    }
    return "";
}

int BitMail::RemoveBuddy(const std::string & email)
{
    if (m_buddies.find(email)!=m_buddies.end()){
        m_buddies.erase(email);
        return bmOk;
    }
    return bmNoBuddy;
}

int BitMail::EmailHandler(BMEventHead * h, void * userp)
{
    BitMail * self = (BitMail *)userp;

    if (h->magic != BMMAGIC){
    	return bmInvalidParam;
    }

    if (h->bmef != bmefMessage){
    	if (self && self->m_cb){
    		return self->m_cb(h, self->m_cbp);
    	}else{
    		return bmInvalidParam;
    	}    	
    }

    BMEventMessage * bmeMsg = (BMEventMessage *)h;
    
    std::string mimemsg = bmeMsg->msg;
    
    //std::cout<<mimemsg<<std::endl;
    
    /**
    <...balabala...>

    To: <bob@aol.com>
    From: <alice@aol.com>
    Subject: BitMail

    <...balabala...>

    MIME-Version: 1.0
    Content-Disposition: attachment; filename="smime.p7m"
    Content-Type: application/pkcs7-mime; smime-type=enveloped-data; name="smime.p7m"
    Content-Transfer-Encoding: base64

    MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/M
    ...
    MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/MIMEBODY/M
    */

    /**
    * Very! Simple MIME Parse to get <To> <From> <Subject> & MimeBody
    */
    std::string sFrom, sSubject, sMimeBody;
    std::vector<std::string> vecTo;
    enum{
        StringBufferLength = 256,
    };
    std::stringstream sstrm;
    sstrm << mimemsg;
    std::string line;
    while(std::getline(sstrm, line)){
        if (line.empty()) continue;
        if (!line.compare(0, 3, "To:")){
            CMailClient::GetEmailAddrList(line.substr(3), vecTo);
        }else if (!line.compare(0, 5, "From:")){
            std::vector<std::string> vecFrom;
            CMailClient::GetEmailAddrList(line.substr(5), vecFrom);
            if (vecFrom.size()){
                sFrom = vecFrom[0];
            }
        }else if (!line.compare(0, 8, "Subject:")){
            char szTemp [StringBufferLength] = "";
            sscanf(line.c_str(), "Subject: %s", szTemp);
            sSubject = szTemp;
        }
    }

    if (sFrom.empty()){
        return bmNoFrom;
    }

    if (std::string::npos == mimemsg.find("MIME-Version:")){
        return bmNoMimeBody;
    }else{
        sMimeBody = mimemsg.substr(mimemsg.find("MIME-Version:"));
    }

    /**
    * Crypto filter
    */

    /**
     * TODO: consider pre-trust stranger's message,
     * and post decrypted message to callback handler,
     * and let UI to make decision.
     */

    std::string sBuddyCertPem = self->GetBuddyCert(sFrom);
    if (sBuddyCertPem.empty()){
        // Ignore bmStranger
    }

    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_enveloped){
        // Envelopped data
        sMimeBody = self->m_profile->Decrypt(sMimeBody);
        if (sMimeBody.empty()){
            return bmDecryptFail;
        }
    }

    CX509Cert buddyCert;
    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_signed){
        buddyCert.LoadCertFromSig(sMimeBody);
        if (buddyCert.IsValid()){
            sMimeBody = buddyCert.Verify(sMimeBody);
            if (sMimeBody.empty()){
                return bmVerifyFail;
            }
        }else{
            return bmInvalidCert;
        }
    }

    if (self && self->m_cb){
    	BMEventMessage bmeMessage;
    	bmeMessage.h = *h;
    	
    	if (!sFrom.empty()){
    		bmeMessage.from = sFrom;
    	}    	

        if (!sMimeBody.empty()){
            bmeMessage.msg = CX509Cert::b64enc(sMimeBody);
        }
        
        if (buddyCert.IsValid()){
        	bmeMessage.cert = CX509Cert::b64enc(buddyCert.GetCertByPem());
        }
        
        self->m_cb((BMEventHead *)&bmeMessage, self->m_cbp);
    }

    return bmOk;
}

#ifdef TEST
/**
 * For sleep system utility
 */
#include <unistd.h>

int BitmailHandler(BMEventHead * h, void * userp)
{
	BitMail * self = (BitMail *)userp;
	
	if (h->magic != BMMAGIC){
		return bmInvalidParam;
	}
	
	switch(h->bmef){
	case bmefMsgCount:
		std::cout<<"MsgCount: "<< ((BMEventMsgCount*)h)->msgcount << std::endl;
		{
			static unsigned int lastMsgCount = 0;
			std::cout<<"LastMsgCount: " << lastMsgCount << std::endl;
			if (((BMEventMsgCount*)h)->msgcount > lastMsgCount){
				std::cout<<"CheckInbox"<<std::endl;
				self->CheckInbox();
			}	
			lastMsgCount = ((BMEventMsgCount*)h)->msgcount;
		}
		break;
	case bmefMessage:
		std::cout<<"Message: "<<std::endl
		             <<"From:\n" << ((BMEventMessage*)h)->from << std::endl
				     <<"Msg: \n" <<((BMEventMessage*)h)->msg << std::endl
				     <<"MsgLength:\n" <<CX509Cert::b64dec(((BMEventMessage*)h)->msg).length()<< std::endl					 
					 <<"MsgText:\n" <<CX509Cert::b64dec(((BMEventMessage*)h)->msg)<< std::endl
				     <<"Cert:\n" <<((BMEventMessage*)h)->cert << std::endl;
		break;
	case bmefSystem:
		std::cout<<"System: " << std::endl;
		break;
	default:
		std::cout<<"N/A"<<std::endl;
		break;
	}
    return 0;
}

void * RxThread(void * args)
{
	BitMail * alice = (BitMail *)args;
	while(true){
	    alice->StartIdle(120000);
	    std::cout<<"Idle timeout, re-idle"<<std::endl;
	    alice->CheckInbox();
	}
	return NULL;
}

int main(int argc, char * argv [])
{
    if (argc != 7){
    printf("Usage:\n");
        printf("\t./bmc smtpsurl smtpuser smtppass imapsurl imapuser imappass\n");
        printf("\tNote: smtpuser or imapuser usually is email\n");
        return 0;
    }
    
    /**
     * Ping-Pong Test
     */
    BitMail * alice = new BitMail();

    alice->OnBitMailEvent( BitmailHandler, alice);

    alice->InitNetwork(  argv[1]// tx url
                    , argv[2]// tx user
                    , argv[3]// tx pass
                    , argv[4]// rx url
                    , argv[5]// rx user
                    , argv[6]// rx pass
                    );

    alice->CreateProfile("Alice", argv[2], "alicealice", 1024);
    
    /**
     * Add herself! for PING-PONG test
     */
    alice->AddBuddy(alice->GetCert());
    
    pthread_t tid;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_create(&tid, &tattr, RxThread, alice);
    
    while(true){
    	char szmsg[256] = "";
    	std::string smsg = fgets(szmsg, sizeof(szmsg), stdin);
    	alice->SendMsg(alice->GetEmail(), smsg);
    }

    delete alice;

    return 0;
}

#endif
