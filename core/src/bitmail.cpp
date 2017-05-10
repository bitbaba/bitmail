#if defined(WIN32)
# include <winsock2.h>
#endif

# include <bitmailcore/bitmail_internal.h>
# include <bitmailcore/bitmail.h>
# include <bitmailcore/email.h>
# include <bitmailcore/x509cert.h>
# include <bitmailcore/rpc.h>
# include <bitmailcore/multipart.h>
# include <bitmailcore/version.h>

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

extern "C" {
#include <lauxlib.h>
};

# include <string>
# include <fstream>
# include <sstream>
# include <iostream>
# include <algorithm>
# include <string.h>

# include <json/json.h>

ScopedLock::ScopedLock(ILock * lock)
    : m_lock(lock)
{
    if (m_lock) m_lock->Lock();
}

ScopedLock::~ScopedLock(){
    if (m_lock) m_lock->Unlock();
}


static void BitMailGlobalInit()
{
    OPENSSL_load_builtin_modules();
    ERR_load_crypto_strings();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
    #ifdef WIN32
    WSAData wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    #endif
}

BitMail::BitMail(ILockCraft * lockCraft)
: m_profile(NULL)
, m_mc(NULL)
, m_onMessageEvent(NULL), m_onMessageEventParam(NULL)
, m_lockCraft(lockCraft), m_lockContacts(NULL), m_lockNet(NULL), m_lockProfile(NULL)
, blockNoSig_(false), blockNoEnvelop_(false), blockNoFriend_(false)
{
    BitMailGlobalInit();

    if (m_lockCraft){
        m_lockContacts = m_lockCraft->CreateLock();
        m_lockNet = m_lockCraft->CreateLock();
        m_lockProfile = m_lockCraft->CreateLock();
    }

    // Create a profile object
    m_profile = new CX509Cert();

    // Create a email object
    m_mc = new CMailClient(m_lockNet, EmailHandler, this);
}

BitMail::~BitMail()
{
    if (m_profile != NULL){
        delete m_profile; m_profile = NULL;
    }
    if (m_mc != NULL){
        delete (m_mc); m_mc = NULL;
    }
    if (m_lockContacts){
        m_lockCraft->FreeLock(m_lockContacts); m_lockContacts = NULL;
    }
    if (m_lockNet){
        m_lockCraft->FreeLock(m_lockNet); m_lockNet = NULL;
    }
    if (m_lockProfile){
        m_lockCraft->FreeLock(m_lockProfile); m_lockProfile = NULL;
    }
}

static BitMail * global_Instance_of_Bitmail = NULL;

BitMail * BitMail::getInst(ILockCraft * lockCraft){
	if (global_Instance_of_Bitmail == NULL){
                global_Instance_of_Bitmail = new BitMail(lockCraft);
	}
    return global_Instance_of_Bitmail;;
}

void BitMail::freeInst(){
    if (global_Instance_of_Bitmail != NULL){
    	delete global_Instance_of_Bitmail;
		global_Instance_of_Bitmail = NULL;
    }
}

std::string BitMail::GetVersion()
{
    return BITMAIL_CORE_GIT_VERSION;
}

std::vector<std::string> BitMail::decodeReceips(const std::string & receips)
{
    std::istringstream iss(receips);
    std::set<std::string> uniq_set;
    std::string line;
    while (std::getline(iss, line, ';')){
        if (line.empty()) continue;
        if (line.find('@') != std::string::npos){
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);
            uniq_set.insert(line);
        }
    }
    std::vector<std::string> v;
    for(std::set<std::string>::const_iterator it = uniq_set.begin(); it != uniq_set.end(); ++it){
        v.push_back(*it);
    }
    std::sort(v.begin(), v.end());
    return v;
}

std::string BitMail::serializeReceips(const std::vector<std::string> & vec_receips)
{
    std::string receips;
    for (std::vector<std::string>::const_iterator it = vec_receips.begin(); it != vec_receips.end(); ++it){
        receips += *it;
        if ((it+1) == vec_receips.end()) break;
        receips += ";";
    }
    std::vector<std::string> vec_copy = decodeReceips(receips);
    receips.clear();
    for (std::vector<std::string>::const_iterator it = vec_copy.begin(); it != vec_copy.end(); ++it){
        receips += *it;
        if ((it+1) == vec_copy.end()) break;
        receips += ";";
    }
    return receips;
}

std::string BitMail::serializeReceips(const std::string & receip)
{
	std::vector<std::string> receips; receips.push_back(receip);
	return serializeReceips(receips);
}

int BitMail::splitMultiparts(const std::string & in, std::vector<std::string> & out)
{
    return multipart_split(in, out);
}

std::string BitMail::partType(const std::string & in)
{
    return multipart_partType(in);
}

std::string BitMail::partParam(const std::string & in, const std::string & header, const std::string & pName)
{
    return multipart_partParam(in, header, pName);
}

std::string BitMail::partContent(const std::string & in)
{
    return multipart_partContent(in);
}

std::string BitMail::partEncoding(const std::string & in)
{
    return multipart_partEncoding(in);
}

std::string BitMail::md5(const std::string & s)
{
    return CX509Cert::hash(s, "md5");
}

std::string BitMail::sha1(const std::string & s)
{
    return CX509Cert::hash(s, "sha1");
}

std::string BitMail::sha256(const std::string & s)
{
    return CX509Cert::hash(s, "sha256");
}

std::string BitMail::toBase64(const std::string & s)
{
    return CX509Cert::b64enc(s, true);
}

std::string BitMail::fromBase64(const std::string & s)
{
    return CX509Cert::b64dec(s, true);
}

std::string BitMail::toBase64Line(const std::string & s)
{
    return CX509Cert::b64enc(s, false);
}

std::string BitMail::fromBase64Line(const std::string & s)
{
    return CX509Cert::b64dec(s, false);
}

std::string BitMail::certId(const std::string & certpem)
{
    return CX509Cert(certpem).GetID();
}

std::string BitMail::certCN(const std::string & certpem)
{
    return CX509Cert(certpem).GetCommonName();
}

std::string BitMail::certEmail(const std::string & certpem)
{
    return CX509Cert(certpem).GetEmail();
}

unsigned int BitMail::certBits(const std::string & certpem)
{
    return CX509Cert(certpem).GetBits();
}

bool BitMail::Genesis(unsigned int bits
                    , const std::string & nick
                    , const std::string & email
                    , const std::string & passphrase
                    , const std::string & txurl
                    , const std::string & rxurl
                    , const std::string & login
                    , const std::string & pass
                    , const std::string & proxy)
{
    do {
        ScopedLock scope(m_lockProfile);
        if (!m_profile->Create(bits, nick, email, passphrase)){
            return false;
        }
    }while(0);

    if (!m_mc->config(txurl, rxurl, login, pass, proxy)){
        return false;
    }
    return true;
}

bool BitMail::Import(const std::string & passphrase, const std::string & json)
{    
    Json::Reader reader;
    Json::Value joRoot;
    if (!reader.parse(json, joRoot)){
    	return false;
    }
    if (joRoot.isMember("Profile")){
        Json::Value profile = joRoot["Profile"];
        if (profile.isMember("cert")&& profile.isMember("key")){
            ScopedLock scope(m_lockProfile);
            m_profile->ImportCert(profile["cert"].asString());
            m_profile->ImportPrivKey(profile["key"].asString(), passphrase);
        }
    }
    if (joRoot.isMember("network")){
        Json::Value network = joRoot["network"];
        if (network.isMember("txUrl")
            && network.isMember("rxUrl")
            && network.isMember("login")
            && network.isMember("password")
            && network.isMember("proxy"))
        {
            std::string password;
            do {
                ScopedLock scope(m_lockProfile);
                password = m_profile->Decrypt( network["password"].asString() );
            }while(0);
            m_mc->config(network["txUrl"].asString()
                            , network["rxUrl"].asString()
                            , network["login"].asString()
                            , password
                            , network["proxy"].asString());
        }
    }
    if (joRoot.isMember("block")){
        Json::Value block = joRoot["block"];
        if (block.isMember("noSig")){
            blockNoSig_ = block["noSig"].asBool();
        }
        if (block.isMember("noEnvelop")){
            blockNoEnvelop_ = block["noEnvelop"].asBool();
        }
        if (block.isMember("noFriend")){
            blockNoFriend_ = block["noFriend"].asBool();
        }
    }
    if (joRoot.isMember("contacts")){
        ScopedLock scope(m_lockContacts);
    	contacts_ = joRoot["contacts"].toStyledString();
    }
    return true;
}

std::string BitMail::Export() const
{
    Json::Value joRoot;
    do {
        ScopedLock scope(m_lockProfile);
        Json::Value profile;
        profile["cert"] = m_profile->ExportCert();
        profile["key"] = m_profile->ExportPrivKey();
        joRoot["Profile"] = profile;
    }while(0);

    Json::Value network;
    network["txUrl"] = m_mc->txUrl();
    network["rxUrl"] = m_mc->rxUrl();
    network["login"] = m_mc->login();

    std::string password;
    do {
        ScopedLock scope(m_lockProfile);
        password = m_profile->Encrypt( m_mc->password() );
    }while(0);

    network["password"]  = password;
    network["proxy"] = m_mc->proxy();
    joRoot["network"] = network;

    Json::Value block;
    block["noSig"] = blockNoSig_;
    block["noEnvelop"] = blockNoEnvelop_;
    block["noFriend"] = blockNoFriend_;
    joRoot["block"] = block;

    Json::Reader reader; Json::Value contacts;
    do {
        ScopedLock scope(m_lockContacts);
        if (!reader.parse(contacts_, contacts)){
            contacts = Json::Value(Json::objectValue);
        }
        joRoot["contacts"] = contacts;
    }while(0);

    return joRoot.toStyledString();
}

std::string BitMail::email() const{
    ScopedLock scope(m_lockProfile);
    return m_profile->GetEmail();
}

std::string BitMail::cert() const{
    ScopedLock scope(m_lockProfile);
    return m_profile->ExportCert();
}

std::string BitMail::privKey() const{
    ScopedLock scope(m_lockProfile);
    return m_profile->ExportPrivKey();
}

std::string BitMail::pkcs12() const{
    ScopedLock scope(m_lockProfile);
    return m_profile->ExportPKCS12();
}

std::string BitMail::passphrase() const {
    ScopedLock scope(m_lockProfile);
    return m_profile->GetPassphrase();
}

bool BitMail::UpdatePassphrase(const std::string & passphrase){
    ScopedLock scope(m_lockProfile);
    return m_profile->SetPassphrase(passphrase);
}

bool BitMail::SetupNetwork(const std::string & txurl
                            , const std::string & rxurl
                            , const std::string & login
                            , const std::string & password
                            , const std::string & proxy)
{
    return m_mc->config(txurl, rxurl, login, password, proxy);
}

std::string BitMail::txUrl() const{
    return m_mc->txUrl();
}

std::string BitMail::rxUrl() const{
    return m_mc->rxUrl();
}

std::string BitMail::login() const{
    return m_mc->login();
}

std::string BitMail::password() const{
    return m_mc->password();
}

std::string BitMail::proxy() const{
    return m_mc->proxy();
}

bool BitMail::blockNoSig() const{
    return blockNoSig_;
}

bool BitMail::blockNoEnvelop() const{
    return blockNoEnvelop_;
}

bool BitMail::blockNoFriend() const{
    return blockNoFriend_;
}

void BitMail::blockNoSig(bool yes) {
    blockNoSig_ = yes;
}

void BitMail::blockNoEnvelop(bool yes) {
    blockNoEnvelop_ = yes;
}

void BitMail::blockNoFriend(bool yes) {
    blockNoFriend_ = yes;
}

/**
* Contract
*/
std::string BitMail::exec(const std::string & script)
{
    lua_State *L = luaL_newstate();  /* create state */
    lua_close(L);
    return "";
}

/**
* Contacts APIs
*/
std::vector<std::string> BitMail::contacts() const
{
    ScopedLock scope(m_lockContacts);
    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
    	return std::vector<std::string>();
    }
    return contacts.getMemberNames();
}

bool BitMail::addContact(const std::string & emails)
{
    ScopedLock scope(m_lockContacts);

    if (emails.empty()) return false;

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
    	return false;
    }

    if (contacts.isMember(emails)){
    	return true;
    }

    Json::Value contact;
    reader.parse("{}", contact);
    contacts[emails] = contact;

    contacts_ = contacts.toStyledString();
    return true;
}

bool BitMail::hasContact(const std::string & emails) const
{
    ScopedLock scope(m_lockContacts);

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
    	return false;
    }

    return contacts.isMember(emails);
}

bool BitMail::removeContact(const std::string & emails)
{
    ScopedLock scope(m_lockContacts);

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
    	return false;
    }

    if (!contacts.isMember(emails)){
    	return true;
    }

    if (!contacts.removeMember(emails)){
    	return false;
    }

    contacts_ = contacts.toStyledString();
    return true;
}

std::string BitMail::contattrib(const std::string & emails, const std::string & att_name) const
{
    ScopedLock scope(m_lockContacts);

    if (emails.empty() || att_name.empty()) return "";

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
        return "";
    }

    Json::Value contact;
    if (!contacts.isMember(emails)){
    	return "";
    }

    contact = contacts[emails];

    if (contact.isMember(att_name))
    	return contact[att_name].asString();
    else
    	return "";
}

bool BitMail::contattrib(const std::string & emails, const std::string & att_name, const std::string & att_value)
{
    ScopedLock scope(m_lockContacts);

    if (emails.empty() || att_name.empty()) return false;

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(contacts_, contacts)){
    	return false;
    }

    Json::Value contact;
    if (contacts.isMember(emails)){
    	// fetch a copy
    	contact = contacts[emails];
    }else{
    	// construct a fresh object
    	reader.parse("{}", contact);
    }

    contact[att_name] = att_value;
    contacts[emails] = contact;

    contacts_ = contacts.toStyledString();
    return true;
}

bool BitMail::Tx(const std::vector<std::string> & friends
        , const std::string & msg
        , RTxProgressCB cb, void * userp)
{
    std::string smime = Encrypt(friends, msg, false);
    return m_mc->Tx( m_profile->GetEmail(), friends, smime, cb, userp);
}

bool BitMail::Rx(MessageEventCB cb, void * msgcbp, RTxProgressCB rtxcb, void * rtxcbp)
{
    // TODO: re-entry in mulithreads
    // but it seems no nessesary to refresh your inbox in multithreads.
    m_onMessageEvent = cb; m_onMessageEventParam = msgcbp;
    return m_mc->Rx(rtxcb, rtxcbp);
}

bool BitMail::Expunge()
{
    // Clear all <deleted> messages
    return m_mc->Expunge();
}

std::string BitMail::Encrypt(const std::vector<std::string> & emails, const std::string & msg, bool fSignOnly)
{
    std::string smime;
    if (msg.empty()){ return smime;}

    /**
     * Note:
     * GroupMsg(msg, vector<bob>) != SendMsg(msg, bob);
     * GroupMsg must encrypt msg before send it out.
     */
    do {
        ScopedLock scope(m_lockProfile);
        smime = m_profile->Sign(msg);
    }while(0);

    if (fSignOnly){ return smime; }

    std::set<std::string> vecTo;
    for (std::vector<std::string>::const_iterator it = emails.begin(); it != emails.end();++it){
        std::string certpem = (contattrib(*it, "cert"));
        if (!certpem.empty()){ vecTo.insert(certpem); }
    }

    if (!vecTo.size()){ return smime; }

    smime = CX509Cert::MEncrypt(smime, vecTo);

    return smime;
}

bool BitMail::Decrypt(const std::string & smime
        , std::string & from
        , std::string & nick
        , std::string & msg
        , std::string & certid
        , std::string & cert
        , std::string & sigtime
        , bool * encrypted)
{
    std::string sMimeBody = smime;
    BitMail * self = this;
    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_enveloped){
        ScopedLock scope(m_lockProfile);
        sMimeBody = self->m_profile->Decrypt(sMimeBody);
        if (sMimeBody.empty()){
            return false;
        }
        *encrypted = true;
    }

    CX509Cert buddyCert;
    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_signed){
        /** signature time**/
        sigtime = CX509Cert::GetSigningTime(sMimeBody);
        /** signer certificate**/
        buddyCert.ImportCertFromSig(sMimeBody);
        if (buddyCert.IsValid()){
            sMimeBody = buddyCert.Verify(sMimeBody);
            if (sMimeBody.empty()){
                return false;
            }
        }else{
            return false;
        }
    }else{
        return false;
    }

    from = buddyCert.GetEmail();
    nick = buddyCert.GetCommonName();
    msg = sMimeBody;
    certid = buddyCert.GetID();
    cert = buddyCert.ExportCert();

    return true;
}

// Mime parser
bool BitMail::EmailHandler(BMEventHead * h, void * userp)
{
    BitMail * self = (BitMail *)userp;

    if (!self || h->magic != BMMAGIC || h->bmef != bmefMessage){
        return false;
    }

    BMEventMessage * bmeMsg = (BMEventMessage *)h;
    std::string receips = parseRFC822AddressList(bmeMsg->msg);
    std::string from, nick, msg, certid, cert, sigtime;
    bool encrypted = false;
    self->Decrypt(bmeMsg->msg, from, nick, msg, certid, cert, sigtime, &encrypted);
    if (self->m_onMessageEvent){
        self->m_onMessageEvent(from.c_str()
                                , receips.c_str()
                                , msg.data(), msg.length()
                                , certid.c_str()
                                , cert.c_str()
                                , sigtime.c_str()
                                , encrypted
                                , self->m_onMessageEventParam);
    }

    return true;
}

std::string BitMail::parseRFC822AddressList(const std::string & mimemsg)
{
    /**
      *TODO: parse address list
      * https://www.w3.org/Protocols/rfc822/#z8
      * https://tools.ietf.org/html/rfc5322#section-2.2.3
      * TODO: Split lines by LF now, but should be CRLF
    */
    std::string receips;
    do {
        std::istringstream iss(mimemsg);
        std::string line;
        while (std::getline(iss, line))
        {
            // clear CR
            while(line.find('\r') != std::string::npos){
                line.erase(line.find('\r'));
            }

            const char * p = line.c_str();

            // initial state
            if (receips.empty()){
                if (*p && (*p == 't' || *p == 'T')) p++;
                else continue;
                if (*p && (*p == 'o' || *p == 'O')) p++;
                else continue;
                if (*p && *p == ':') p++;
                else continue;
                receips = line;
            }else{
            // mulit-line mode: folding
                if (*p && !::isspace(*p)) break;
                else{
                    receips += line;
                }
            }
        }
    }while(0);


    // a weak RFC822 address parser
    do{
        if (receips.empty()) break;

        char * receips_buf = strdup(receips.c_str());
        receips.clear();

        if (!receips_buf) break;

        const char * delims = "~`!$%^&*-+=\\{}[]?<>()\"':,;|/ \t\r\n";
        const char * tok = strtok(receips_buf, delims);
        while(tok){
            if (*tok && strchr(tok, '@')){
                receips += tok;
                receips += ";";
            }
            tok = strtok(NULL, delims);
        }

        free(receips_buf);
    }while(0);

    return (receips);
}
