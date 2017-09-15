//
// Created by Administrator on 9/11/2017.
//
#include <string>
#include <map>
#include <cstdint>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <json/json.h>


#include "BitMailWrapper.h"

#include "x509cert.h"

// global variables
static CX509Cert * gs_profile = NULL;
static std::map<std::string, std::string> * gs_friends = NULL;
static pthread_mutex_t * gs_lock = NULL;

// forward declarations
static void InitOpenSSL();
static Json::Value InitBitMailWrapper(const Json::Value & params);
static Json::Value CreateProfile(const Json::Value & params);
static Json::Value EncryptMessage(const Json::Value & params);
static Json::Value DecryptMessage(const Json::Value & params);

class ScopedLock{
public:
    ScopedLock(){
        if (gs_lock != NULL) pthread_mutex_lock(gs_lock);
    };
    ~ScopedLock(){
        if (gs_lock != NULL) pthread_mutex_unlock(gs_lock);
    }
};

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

    if (gs_friends == NULL){
        gs_friends = new std::map<std::string, std::string>();
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
            && params.isMember("passphrase")){
        std::string cn = params["user"].asString();
        std::string email = params["email"].asString();
        std::string passphrase = params["passphrase"].asString();
        gs_profile->Create(1024, cn, email, passphrase);
        ret["cert"] = gs_profile->ExportCert();
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

    if (gs_friends->find(friendName) == gs_friends->end()){
        return smime;
    }

    std::set<std::string> certs;
    do {
        ScopedLock socpe;
        certs.insert(gs_friends->find(friendName)->second);
    }while(0);

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
    std::string sMimeBody = smime;
    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_enveloped){
        ScopedLock scope;
        sMimeBody = gs_profile->Decrypt(sMimeBody);
        if (sMimeBody.empty()){
            return false;
        }
        *encrypted = true;
    }

    if (CX509Cert::CheckMsgType(sMimeBody) == NID_pkcs7_signed)
    {
        /** signature time**/
        sigtime = CX509Cert::GetSigningTime(sMimeBody);
        /** signer certificate**/
        CX509Cert buddyCert;
        buddyCert.ImportCertFromSig(sMimeBody);
        if (buddyCert.IsValid())
        {
            sMimeBody = buddyCert.Verify(sMimeBody);
            if (!sMimeBody.empty()){
                from = buddyCert.GetEmail();
                nick = buddyCert.GetCommonName();
                msg = sMimeBody;
                certid = buddyCert.GetID();
                cert = buddyCert.ExportCert();
                return true;
            }
        }
    }

    return false;
}

Json::Value DecryptMessage(const Json::Value & params)
{
    Json::Value ret = Json::objectValue;
    return ret;
}