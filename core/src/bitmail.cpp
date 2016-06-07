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
        delete (m_mc); m_mc = NULL;
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

int BitMail::SetTxUrl(const std::string & u)
{
	m_mc->SetTxUrl(u);
	return bmOk;
}

std::string BitMail::GetTxUrl() const
{
	return m_mc->GetTxUrl();
}

int BitMail::SetTxLogin(const std::string & l)
{
	m_mc->SetTxLogin(l);
	return bmOk;
}

std::string BitMail::GetTxLogin() const
{
	return m_mc->GetTxLogin();
}

int BitMail::SetTxPassword(const std::string & p)
{
	m_mc->SetTxPassword(p);
	return bmOk;
}

std::string BitMail::GetTxPassword() const
{
	return m_mc->GetTxPassword();
}

int BitMail::SetRxUrl(const std::string & u)
{
	m_mc->SetRxUrl(u);
	return bmOk;
}

std::string BitMail::GetRxUrl() const
{
	return m_mc->GetRxUrl();
}

int BitMail::SetRxLogin(const std::string & l)
{
	m_mc->SetRxLogin(l);
	return bmOk;
}

std::string BitMail::GetRxLogin() const
{
	return m_mc->GetRxLogin();
}

int BitMail::SetRxPassword(const std::string & p)
{
	m_mc->SetRxPassword(p);
	return bmOk;
}

std::string BitMail::GetRxPassword() const
{
	return m_mc->GetRxPassword();
}

int BitMail::SendMsg(const std::string &email_to, const std::string &msg)
{
	if (msg.empty()){
		return bmInvalidParam;
	}

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
	if (msg.empty()){
		return bmInvalidParam;
	}
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

int BitMail::Expunge()
{
	// Clear all <deleted> messages
	m_mc->Expunge();
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

int BitMail::SetPassphrase(const std::string & passphrase)
{
	m_profile->SetPassphrase(passphrase);
	return bmOk;
}

std::string BitMail::GetEmail() const
{
    return m_profile->GetEmail();
}

std::string BitMail::GetKey() const
{
	return m_profile->GetPrivateKeyAsEncryptedPem();
}

int BitMail::GetBits() const{
	return m_profile->GetBits();
}

std::string BitMail::GetPassphrase() const
{
	return m_profile->GetPassphrase();
}

std::string BitMail::Encrypt(const std::string & text) const
{
	return m_profile->Encrypt(text);
}

std::string BitMail::Decrypt(const std::string & code) const
{
	return m_profile->Decrypt(code);
}

std::string BitMail::GetCommonName(const std::string & e) const
{
	if (e == GetEmail()){
		return m_profile->GetCommonName();
	}
	std::string sCert = m_buddies.find(e)->second;
	CX509Cert x;
	x.LoadCertFromPem(sCert);
	return x.GetCommonName();
}

std::string BitMail::GetCert(const std::string & e) const
{
	if (e == GetEmail()){
		return m_profile->GetCertByPem();
	}
	std::string sCert = m_buddies.find(e)->second;
	return sCert;
}

std::string BitMail::GetCertID(const std::string & e) const
{
	if (e == GetEmail()){
		return m_profile->GetID();
	}
	std::string sCert = m_buddies.find(e)->second;
	CX509Cert x;
	x.LoadCertFromPem(sCert);
	return x.GetCommonName();
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

bool BitMail::IsBuddy(const std::string & certpem) const
{
	CX509Cert x;
	x.LoadCertFromPem(certpem);
	if (!x.IsValid()){
		return false;
	}
	std::string email = x.GetEmail();
	if (email.empty()){
		return false;
	}
	std::string certid = GetCertID(email);
	if (!certid.empty() && certid == x.GetID()){
		return true;
	}
	return false;
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

