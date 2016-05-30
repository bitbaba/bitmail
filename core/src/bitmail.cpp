#if defined(WIN32)
# include <winsock2.h>
#endif

# include <bitmailcore/bitmail.h>
# include <bitmailcore/email.h>
# include <bitmailcore/x509cert.h>

# include <curl/curl.h>
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

# include <string>
# include <fstream>
# include <sstream>
# include <iostream>

#include <string.h>

BitMail::BitMail()
: m_onPollEvent(NULL), m_onPollEventParam(NULL)
, m_onMessageEvent(NULL), m_onMessageEventParam(NULL)
, m_fAllowStranger(false)
{
    OpenSSL_add_all_ciphers();
    OPENSSL_load_builtin_modules();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
	
	// Create a email object
	m_mc = new CMailClient(EmailHandler, this);
	
	// Create a profile object
	m_profile = new CX509Cert();
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
int BitMail::OnPollEvent( PollEventCB cb, void * userp)
{
    m_onPollEvent = cb;
    m_onPollEventParam = userp;
    return 0;
}

int BitMail::OnMessageEvent( MessageEventCB cb, void * userp)
{
    m_onMessageEvent = cb;
    m_onMessageEventParam = userp;
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

std::string BitMail::GetTxUrl() const
{
	return m_mc->GetTxUrl();
}

std::string BitMail::GetTxLogin() const
{
	return m_mc->GetTxLogin();
}

std::string BitMail::GetTxPassword() const
{
	return m_mc->GetTxPassword();
}

std::string BitMail::GetRxUrl() const
{
	return m_mc->GetRxUrl();
}

std::string BitMail::GetRxLogin() const
{
	return m_mc->GetRxLogin();
}

std::string BitMail::GetRxPassword() const
{
	return m_mc->GetRxPassword();
}

int BitMail::AllowStranger(bool fYes)
{
	m_fAllowStranger = fYes;
	return bmOk;
}

bool BitMail::AllowStranger() const
{
	return m_fAllowStranger;
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

    if (m_mc->SendMsg( m_profile->GetEmail()
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

    if (m_mc->SendMsg( m_profile->GetEmail()
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
	if (NULL != m_profile)
	{
		delete m_profile;
		m_profile = NULL;
	}
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

int BitMail::SaveProfile(const std::string & passphrase
    				, std::string & keypem
					, std::string & certpem)
{
	return 0;
}

std::string BitMail::GetEmail() const
{
    return m_profile->GetEmail();
}

std::string BitMail::GetCommonName() const
{
    return m_profile->GetCommonName();
}

std::string BitMail::GetKey() const
{
	return m_profile->GetPrivateKeyAsEncryptedPem();
}

std::string BitMail::GetCert() const
{
    return m_profile->GetCertByPem();
}

int BitMail::GetBits() const{
	return m_profile->GetBits();
}

std::string BitMail::GetPassphrase() const
{
	return m_profile->GetPassphrase();
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

int BitMail::GetBuddies(std::vector<std::string> & vecEmails) const
{
	for (std::map<std::string, std::string>::const_iterator it = m_buddies.begin();
		 it != m_buddies.end();
		 ++it){
		 vecEmails.push_back(it->first);
	}
	return bmOk;
}

	
std::string BitMail::GetBuddyCommonName(const std::string & e) const
{
	for (std::map<std::string, std::string>::const_iterator it = m_buddies.begin();
		 it != m_buddies.end();
		 ++it){
		if (it->first == e){
			CX509Cert x;
			x.LoadCertFromPem(it->second);
			return x.GetCommonName();
		}
	}
	return "";
}

int BitMail::EmailHandler(BMEventHead * h, void * userp)
{
    BitMail * self = (BitMail *)userp;

    if (h->magic != BMMAGIC){
    	return bmInvalidParam;
    }

    if (h->bmef == bmefMsgCount){
    	if (self && self->m_onPollEvent){
    		return self->m_onPollEvent(((BMEventMsgCount*)h)->msgcount
    									, self->m_onPollEventParam);
    	}else{
    		return bmInvalidParam;
    	}    	
    }else if (h->bmef == bmefSystem){
    	// TODO: not implemented.
    	return 0;
    }else if (h->bmef == bmefMessage){
    	// process it as following.
    }else{
    	return bmInvalidParam;
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

    if (self && self->m_onMessageEvent){
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
        
        self->m_onMessageEvent(bmeMessage.from.c_str()
        						, bmeMessage.msg.c_str()
								, bmeMessage.cert.c_str()
								, self->m_onMessageEventParam);
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
				     <<"Cert:\n" <<CX509Cert::b64dec(((BMEventMessage*)h)->cert) << std::endl;
		{
			std::string sCertPem = CX509Cert::b64dec(((BMEventMessage*)h)->cert);
			std::cout<<"Add Buddy: [" << ((BMEventMessage*)h)->from << "]" <<std::endl;
			self->AddBuddy(sCertPem);
		}
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

int PollEventHandler(unsigned int count, void * userp)
{
	return 0;
}

int MessageEventHandler(const char * from, const char * msg, const char * cert, void * userp)
{
	return 0;
}

void * RxThread(void * args)
{
	BitMail * alice = (BitMail *)args;
	while(true){
	    alice->StartIdle(60000);
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

    alice->OnPollEvent( PollEventHandler, alice);
    
    alice->OnMessageEvent( MessageEventHandler, alice);

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
    	printf("MsgTo:>");
    	std::string sto; std::cin>>sto;
    	printf("Msg:>");
    	std::string smsg;std::cin>>smsg;
    	alice->SendMsg(sto, smsg);
    }

    delete alice;

    return 0;
}

#endif
