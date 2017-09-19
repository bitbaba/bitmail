//
// Created by Administrator on 9/11/2017.
//
#include <string>
#include <map>
#include <cstdint>
#include <fstream>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <json/json.h>


#include "BitMailWrapper.h"

#include "x509cert.h"

// global variables
static CX509Cert * gs_profile = NULL;
static std::string * gs_contacts = NULL;
static pthread_mutex_t * gs_lock = NULL;



class ScopedLock{
public:
    ScopedLock(){
        if (gs_lock != NULL) pthread_mutex_lock(gs_lock);
    };
    ~ScopedLock(){
        if (gs_lock != NULL) pthread_mutex_unlock(gs_lock);
    }
};

/**
* Contacts APIs
*/
std::vector<std::string> contacts()
{
    ScopedLock scope;
    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        return std::vector<std::string>();
    }
    return contacts.getMemberNames();
}

bool addContact(const std::string & emails)
{
    ScopedLock scope;

    if (emails.empty()) return false;

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        contacts = Json::objectValue;
    }

    if (contacts.isMember(emails)){
        return true;
    }

    Json::Value contact;
    reader.parse("{}", contact);
    contacts[emails] = contact;

    *gs_contacts = contacts.toStyledString();
    return true;
}

bool hasContact(const std::string & emails)
{
    ScopedLock scope;
    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        contacts = Json::objectValue;
    }

    return contacts.isMember(emails);
}

bool removeContact(const std::string & emails)
{
    ScopedLock scope;

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        contacts = Json::objectValue;
    }

    if (!contacts.isMember(emails)){
        return true;
    }

    if (!contacts.removeMember(emails)){
        return false;
    }

    *gs_contacts = contacts.toStyledString();
    return true;
}

std::string contattrib(const std::string & emails, const std::string & att_name)
{
    ScopedLock scope;

    if (emails.empty() || att_name.empty()) return "";

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        contacts = Json::objectValue;
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

bool contattrib(const std::string & emails, const std::string & att_name, const std::string & att_value)
{
    ScopedLock scope;

    if (emails.empty() || att_name.empty()) return false;

    Json::Reader reader; Json::Value contacts;
    if (!reader.parse(*gs_contacts, contacts)){
        contacts = Json::objectValue;
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

    *gs_contacts = contacts.toStyledString();
    return true;
}

// forward declarations
static void InitOpenSSL();
static Json::Value InitBitMailWrapper(const Json::Value & params);
static Json::Value CreateProfile(const Json::Value & params);
static Json::Value EncryptMessage(const Json::Value & params);
static Json::Value DecryptMessage(const Json::Value & params);


// Marshall API
JNIEXPORT jstring JNICALL
Java_com_hyphenate_easeui_EaseUI_NativeJsonRPC(JNIEnv *env, jclass type, jstring payload_) {
    const char *payload = env->GetStringUTFChars(payload_, 0);

    Json::Reader reader;
    Json::Value root, ret = Json::objectValue;
    if (reader.parse(payload, root)){
        if (root.isObject() && root.isMember("method") && root.isMember("params")){
            std::string method = root["method"].asString();
            Json::Value params = root["params"];
            if (!method.compare("Init")){
                ret = InitBitMailWrapper(params);
            }else if (!method.compare("CreateProfile")){
                ret = CreateProfile(params);
            }else if (!method.compare("EncryptMessage")){
                ret = EncryptMessage(params);
            }else if (!method.compare("DecryptMessage")){
                ret = DecryptMessage(params);
            }
        }
    }

    env->ReleaseStringUTFChars(payload_, payload);
    return env->NewStringUTF(ret.toStyledString().c_str());
}

void InitOpenSSL(){
    OPENSSL_load_builtin_modules();
    ERR_load_crypto_strings();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();
}

void WriteFile(const std::string & path, const std::string & content){
    std::ofstream fout;
    fout.open(path);
    fout << content;
    fout.close();
}

std::string ReadFile(const std::string & path){
    std::ifstream fin;
    fin.open(path);
    std::string line, ret;
    while(fin >> line){
        ret += line;
    }
    fin.close();
    return ret;
}

bool Import(const std::string & passphrase, const std::string & json)
{
    Json::Reader reader;
    Json::Value joRoot;
    if (!reader.parse(json, joRoot)){
        return false;
    }
    if (joRoot.isMember("Profile")){
        Json::Value profile = joRoot["Profile"];
        if (profile.isMember("cert")&& profile.isMember("key")){
            ScopedLock scope;
            gs_profile->ImportCert(profile["cert"].asString());
            gs_profile->ImportPrivKey(profile["key"].asString(), passphrase);
        }
    }

    if (joRoot.isMember("contacts")){
        ScopedLock scope;
        *gs_contacts = joRoot["contacts"].toStyledString();
    }
    return true;
}

std::string Export()
{
    Json::Value joRoot;
    do {
        ScopedLock scope;
        Json::Value profile;
        profile["cert"] = gs_profile->ExportCert();
        profile["key"] = gs_profile->ExportPrivKey();
        joRoot["Profile"] = profile;
    }while(0);

    Json::Reader reader; Json::Value contacts;
    do {
        ScopedLock scope;
        if (!reader.parse(*gs_contacts, contacts)){
            contacts = Json::Value(Json::objectValue);
        }
        joRoot["contacts"] = contacts;
    }while(0);

    return joRoot.toStyledString();
}

Json::Value InitBitMailWrapper(const Json::Value & params){
    (void)params;
    Json::Value ret = Json::objectValue;

    InitOpenSSL();

    if (gs_lock == NULL){
        gs_lock = new pthread_mutex_t;
    }
    pthread_mutex_init(gs_lock, NULL);
    //pthread_mutex_destroy(gs_lock);

    if (gs_profile == NULL){
        gs_profile = new CX509Cert();
    }

    if (gs_contacts == NULL){
        gs_contacts = new std::string();
    }
    /**
    CX509Cert xcert;
    xcert.Create(1024, "Harry Bot", "HarryBot@bitbaba.com", "secret");
    __android_log_print(ANDROID_LOG_INFO, "BitMailWrapper", "Certificate: %s", xcert.ExportCert().c_str());
    */

    return ret;
}

Json::Value CreateProfile(const Json::Value & params){
    Json::Value ret = Json::objectValue;
    if (gs_profile != NULL
            && params.isMember("user")
            && params.isMember("email")
            && params.isMember("passphrase"))
    {

        std::string user = params["user"].asString();
        std::string email = params["user"].asString();
        std::string passphrase = params["user"].asString();

        gs_profile->Create(1024, user, email, passphrase);
        ret["cert"] = gs_profile->ExportCert();

        /**
        if (!user.empty()){
            std::string profile = ReadFile(std::string("/bitmail-") + user);
            if (!profile.empty()){
                Import(user, profile);
            }else{
                gs_profile->Create(1024, user, email, passphrase);
                std::string profile = Export();
                WriteFile(std::string("/bitmail-") + user, profile);
            }
            ret["cert"] = gs_profile->ExportCert();
        }*/
    }
    return ret;
}


std::string Encrypt(const std::string & friendName, const std::string & msg)
{
    if (msg.empty()){ return "";}

    std::string smime;

    /**
     * Note:
     * GroupMsg(msg, vector<bob>) != SendMsg(msg, bob);
     * GroupMsg must encrypt msg before send it out.
     */
    do {
        ScopedLock scope;
        smime = gs_profile->Sign(msg);
    }while(0);

    if (!hasContact(friendName)){
        return smime;
    }

    std::string cert = contattrib(friendName, "cert");
    if (cert.empty()) {
        return smime;
    }

    std::set<std::string> certs;
    certs.insert(cert);

    // always encrypted a copy for yourself
    do {
        ScopedLock lock;
        certs.insert(gs_profile->ExportCert());
    }while(0);

    smime = CX509Cert::MEncrypt(smime, certs);

    return smime;
}

Json::Value EncryptMessage(const Json::Value & params){
    Json::Value ret = Json::objectValue;
    std::string to = params["to"].asString();
    std::string msg = params["msg"].asString();
    std::string smime = Encrypt(to, msg);
    ret["smime"] = smime;
    return ret;
}

bool Decrypt(const std::string & smime
        , std::string & from
        , std::string & nick
        , std::string & msg
        , std::string & certid
        , std::string & cert
        , std::string & sigtime
        , bool * encrypted)
{
    msg = smime;
    __android_log_print(ANDROID_LOG_INFO, "BitMailWrapper", "SMIME: %s", smime.c_str());

    if (CX509Cert::CheckMsgType(smime) == NID_pkcs7_enveloped){
        *encrypted = true;

        ScopedLock scope;
        msg = gs_profile->Decrypt(smime);
        if (msg.empty()){
            return false;
        }
    }

    if (CX509Cert::CheckMsgType(msg) == NID_pkcs7_signed)
    {
        /** signature time**/
        sigtime = CX509Cert::GetSigningTime(msg);
        /** signer certificate**/
        CX509Cert buddyCert;
        buddyCert.ImportCertFromSig(msg);
        if (buddyCert.IsValid())
        {
            msg = buddyCert.Verify(msg);
            from = buddyCert.GetEmail();
            nick = buddyCert.GetCommonName();
            certid = buddyCert.GetID();
            cert = buddyCert.ExportCert();
            contattrib(from, "cert", cert);
        }
    }
    return true;
}

Json::Value DecryptMessage(const Json::Value & params)
{
    Json::Value ret = Json::objectValue;
    std::string smime = params["smime"].asString();
    bool encrypted = false;
    std::string from, nick, msg, certid, cert, sigtime;
    if (Decrypt(smime, from, nick, msg, certid, cert, sigtime, &encrypted)){
        ret["from"] = from;
        ret["nick"] = nick;
        ret["msg"] = msg;
        ret["certid"] = certid;
        ret["cert"] = cert;
        ret["sigtime"] = sigtime;
        ret["encrypted"] = encrypted;
    }
    return ret;
}
