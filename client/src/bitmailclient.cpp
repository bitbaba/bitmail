#define CURL_STATICLIB
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <bitmailcore/x509cert.h>
#include <bitmailcore/bitmail.h>
#include <json/json.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#if defined(__GNUC__)
    #include <unistd.h>
#endif

#if defined(WIN32)
#define SLEEP(x) ::Sleep((x) * 1000)
#endif

#if defined(__GNUC__)
#define SLEEP(x) sleep((x))
#endif

using namespace std;

static struct option long_options[] = {
   {"generate",  no_argument,       0,  0 },
   {"getid",     no_argument,       0,  0 },
   {"sign",      no_argument,       0,  0 },
   {"verify",    no_argument,       0,  0 },
   {"encrypt",   no_argument,       0,  0 },
   {"decrypt",   no_argument,       0,  0 },
   {"sendmsg",   no_argument,       0,  0 },
   {"recvmsg",   no_argument,       0,  0 },
   {"help",      no_argument,       0,  0 },
   {0,           0,                 0,  0 }
};

enum work_mode{
    undefined = 0,
    generate = 1,
    getid = 2,
    sign = 3,
    verify = 4,
    encmode = 5,
    decmode = 6,
    sndmsg = 7,
    rcvmsg = 8,
    help = 9,
};

static int write_txt_file(const std::string & txt_path, const std::string & s);

static std::string read_txt_file(const std::string & txt_path);

static int onsendmsg(const std::string & info, void * userdata);

static int onrecvmsg(const std::string & info, void * userdata);

static int poll(const std::string & url, const std::string & param);

/**
 * Example:
 * ./bmc --generate -C commonName -E emailAddress -P passphrase
 */
static int generate_cert(const std::string & common_name, const std::string & email, const std::string & passphrase);

/**
 * Example:
 * ./bmc --getid -b buddyCertFile
 */
static int get_certid(const std::string & buddy_cert_file);

/**
 * Example:
 * ./bmc --sign -p profile_cert_file -k profile_key_file -P passphrase -m msg_file
 */
static int sign_msg(const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & keypass
        , const std::string & msg_file);

/**
 * Example:
 * ./bmc --verify -b buddy_cert_file -s sig_file
 */
static int verify_msg(const std::string & buddy_cert_file
        , const std::string & sig_file);

/**
 * Example:
 * ./bmc --encrypt -b buddy_cert_file -m msg_file
 */
static int encrypt_msg(const std::string & buddy_cert_file
        , const std::string & msg_file);

/**
 * Example:
 * ./bmc --decrypt -p profile_cert_file -k profile_key_file -P passphrase -m encrypted_msg_file
 */
static int decrypt_msg(const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , const std::string & enc_file);

/**
 * Example:
 * ./bmc --sendmsg -x net_config_file -b buddy_cert_file(optional) -t email_of_buddy(optional) -p profile_cert_file -k profile_key_file -P passphrase -m msg_file
 */
static int send_msg(const std::string & netconfig_file
        , const std::string & buddy_cert_file
        , const std::string & buddy_email
        , const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , const std::string & msg_file);

/**
 * Example:
 * ./bmc --recvmsg -x net_config_file -B buddy_index_file -p profile_cert_file -k profile_key_file -P passphrase -i interval -u poll_url
 */
static int recv_msg(const std::string & netconfig_file
        , const std::string & buddies_index_file
        , const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , unsigned int interval
        , const std::string & poll_url);

/**
 * @brief printHelp
 * @param void
 * @return void
 */
static void printHelp();

int main(int argc, char **argv)
{
   BitMail mail;(void)mail;
   (void )write_txt_file;
   (void) onsendmsg;
   (void) onrecvmsg;
   int c;
   FILE * fin = stdin,* fout = stdout;
   (void) fin; (void)fout;
   std::string common_name = "", email = "", passphrase = "";
   std::string buddy_cert_file = "";
   std::string profile_cert_file = "";
   std::string profile_key_file = "";
   std::string msg_file = "";
   std::string sig_file = "";
   std::string poll_url = "";
   std::string netconfig_file = "";
   std::string buddies_index_file = "";
   std::string buddy_email = "";
   unsigned int recv_interval = 15; // in seconds.
   work_mode mode = undefined;

   while (1) {
       int option_index = 0;
       c = getopt_long(argc
               , argv
               , "B:b:C:E:P:p:i:k:m:t:s:u:x:"  // short options
               , long_options // long options
               , &option_index);
       if (c == -1){
           break;
       }

       switch (c) {
       case 0:
           if (!strcmp("generate", long_options[option_index].name)){
               mode = generate;
           }else if (!strcmp("getid", long_options[option_index].name)){
               mode = getid;
           }else if (!strcmp("sign", long_options[option_index].name)){
               mode = sign;
           }else if (!strcmp("verify", long_options[option_index].name)){
               mode = verify;
           }else if (!strcmp("encrypt", long_options[option_index].name)){
               mode = encmode;
           }else if (!strcmp("decrypt", long_options[option_index].name)){
               mode = decmode;
           }else if (!strcmp("sendmsg", long_options[option_index].name)){
               mode = sndmsg;
           }else if (!strcmp("recvmsg", long_options[option_index].name)){
               mode = rcvmsg;
           }else if (!strcmp("help", long_options[option_index].name)){
               mode = help;
           }else{
               mode = undefined;
           }
           break;
       case 'B': // buddies' index file
           buddies_index_file = optarg;
           break;
       case 'b': // buddy's certificate
           buddy_cert_file = optarg;
           break;
       case 'p':// profile certificate
           profile_cert_file = optarg;
           break;
       case 'k':// profile_key_file
           profile_key_file = optarg;
           break;
       case 'i':// receiving interval in seconds
           recv_interval = atoi(optarg);
           break;
       case 'P':// passphrase
           passphrase = optarg;
           break;
       case 'C':// common name
           common_name = optarg;
           break;
       case 'E':// email
           email = optarg;
           break;
           // marriage is terrible thing.
           // be careful, dead-lock happens.
       case 'm':// message
           msg_file = optarg;
           break;
       case 't': // buddy_email
           buddy_email = optarg;
           break;
       case 's':// signature
           sig_file = optarg;
           break;
       case 'u':// poll, here a localhost url
           poll_url = optarg;
           break;
       case 'x':// network configuration file
           netconfig_file = optarg;
           break;
       default:
           printf("?? getopt returned character code 0%o ??\n", c);
       }
   }

   if (optind < argc) {
       printf("non-option ARGV-elements: ");
       while (optind < argc)
           printf("%s ", argv[optind++]);
       printf("\n");
   }

   switch(mode){
   case generate:
       return generate_cert(common_name, email, passphrase);
       break;
   case getid:
       return get_certid(buddy_cert_file);
       break;
   case sign:
       return sign_msg(profile_cert_file, profile_key_file, passphrase, msg_file);
       break;
   case verify:
       return verify_msg(buddy_cert_file, sig_file);
       break;
   case encmode:
       return encrypt_msg(buddy_cert_file, msg_file);
       break;
   case decmode:
       return decrypt_msg(profile_cert_file, profile_key_file, passphrase, msg_file);
       break;
   case sndmsg:
       return send_msg(netconfig_file, buddy_cert_file, buddy_email, profile_cert_file, profile_key_file, passphrase, msg_file);
       break;
   case rcvmsg:
       return recv_msg(netconfig_file, buddies_index_file, profile_cert_file, profile_key_file, passphrase, recv_interval, poll_url);
       break;
   case help:
   case undefined:
       printHelp();
   default:
       break;
   }

   exit(EXIT_SUCCESS);
}

std::string read_txt_file(const std::string &path)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ios_base::binary | std::ios_base::in);
    if (!ifs.is_open()){
        return "";
    }

    std::string txt;
    while(!ifs.eof()){
        char c;
        ifs.get(c);
        txt += c;
    }
    ifs.close();

    return txt;
}

int write_txt_file(const std::string &path, const std::string & s)
{
    std::ofstream ofs;
    ofs.open(path.c_str(), std::ios_base::binary | std::ios_base::out);
    if (!ofs.is_open()){
        return 0;
    }
    ofs.write(s.c_str(), s.length());
    ofs.close();

    return s.length();
}

/**
 *
 */
int generate_cert(const std::string & common_name, const std::string & email, const std::string & passphrase)
{
    CX509Cert x;
    if (x.Create(common_name, email, passphrase))
    {
        return -1;
    }
    std::string sCertPem = x.GetCertByPem();
    std::string sEncKeyPem = x.GetPrivateKeyByEncryptedPem();
    printf("%s\n", sCertPem.c_str());
    printf("%s\n", sEncKeyPem.c_str());
    return 0;
}

int get_certid(const std::string & cert_file)
{
    std::string sCertPem = read_txt_file(cert_file);
    if (sCertPem.empty()) return -1;
    CX509Cert x;
    x.LoadCertFromPem(sCertPem);
    std::string sCertID = x.GetID();
    printf("%s\n", sCertID.c_str());
    return 0;
}

int sign_msg(const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & keypass
        , const std::string &msg_file)
{
    std::string sCertPem = read_txt_file(profile_cert_file);
    std::string sKeyPem = read_txt_file(profile_key_file);
    std::string msg = read_txt_file(msg_file);

    CX509Cert x;

    x.LoadCertFromPem(sCertPem);

    x.LoadKeyFromEncryptedPem(sKeyPem, keypass);

    std::string sig = x.Sign(msg);

    printf("%s\n", sig.c_str());
    // to see more details of sig, use http://lapo.it/asn1js/

    return 0;
}

int verify_msg(const std::string & buddy_cert_file,const std::string & sig_file)
{
    std::string sCertPem = read_txt_file(buddy_cert_file);
    std::string sig = read_txt_file(sig_file);

    CX509Cert x;
    x.LoadCertFromPem(sCertPem);
    std::string msg = x.Verify(sig);
    printf("Message:\n%s\n", msg.c_str());

    CX509Cert x0;
    x0.LoadCertFromSig(sig);
    std::string sCertId = x0.GetID();
    printf("Singer's certID:\n%s\n", sCertId.c_str());
    return 0;
}

int encrypt_msg(const std::string & buddy_cert_file, const std::string & msg_file)
{
    std::string sCertPem = read_txt_file(buddy_cert_file);
    std::string msg = read_txt_file(msg_file);

    CX509Cert x;
    x.LoadCertFromPem(sCertPem);

    std::string encMsg = x.Encrypt(msg);

    printf("Encrypted Message:\n%s\n", encMsg.c_str());

    return 0;
}

int decrypt_msg(const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , const std::string & enc_file)
{
    std::string sCertPem = read_txt_file(profile_cert_file);
    std::string sKeyPem = read_txt_file(profile_key_file);
    std::string encMsg = read_txt_file(enc_file);

    CX509Cert x;
    x.LoadCertFromPem(sCertPem);
    x.LoadKeyFromEncryptedPem(sKeyPem, passphrase);

    std::string msg = x.Decrypt(encMsg);
    printf("Decrypted Message:\n%s\n", msg.c_str());
    return 0;
}

int send_msg(const std::string & net_config_file
        , const std::string & buddy_cert_file
        , const std::string & buddy_email
        , const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , const std::string & msg_file)
{
    std::string sBuddyCertPem = read_txt_file(buddy_cert_file);
    std::string sCertPem = read_txt_file(profile_cert_file);
    std::string sEncKeyPem = read_txt_file(profile_key_file);
    std::string msg = read_txt_file(msg_file);
    std::string sNetConfig = read_txt_file(net_config_file);

    BitMail bm;

    CX509Cert xbuddy;
    if (!sBuddyCertPem.empty()){
        xbuddy.LoadCertFromPem(sBuddyCertPem);
        bm.AddBuddy(xbuddy);
    }

    bm.LoadProfile(passphrase, sEncKeyPem, sCertPem);

    Json::Reader reader;
    Json::Value jsonNetConfig;
    reader.parse(sNetConfig, jsonNetConfig);
    Json::Value jsonTxConfig = jsonNetConfig["smtp"];
    bm.InitTx(jsonTxConfig.toStyledString());

    return bm.SendMsg(xbuddy.GetEmail().empty() ? buddy_email : xbuddy.GetEmail(), msg, onsendmsg, &bm);
}

int recv_msg(const std::string & netconfig_file
        , const std::string & buddies_index_file
        , const std::string & profile_cert_file
        , const std::string & profile_key_file
        , const std::string & passphrase
        , unsigned int interval
        , const std::string & poll_url)
{
    std::string sCertPem = read_txt_file(profile_cert_file);
    std::string sEncKeyPem = read_txt_file(profile_key_file);
    std::string sNetConfig = read_txt_file(netconfig_file);
    std::string sBuddyIndex = read_txt_file(buddies_index_file);

    BitMail bm;
    bm.LoadProfile(passphrase, sEncKeyPem, sCertPem);
    if (!bm.m_profile.IsValid()){
        perror("Invalid profile!");
        return -1;
    }

    Json::Value jsonNetConfig;
    do {
        Json::Reader reader;
        reader.parse(sNetConfig, jsonNetConfig);
    }while(0);
    Json::Value jsonRxConfig = jsonNetConfig["pop3"];
    bm.InitRx(jsonRxConfig.toStyledString());

    if (sBuddyIndex.empty()){
        perror("Invalid buddies index");
        return -1;
    }

    Json::Value jsonBuddiesIndex;
    do {
        Json::Reader reader;
        reader.parse(sBuddyIndex, jsonBuddiesIndex);
    }while(0);

    for (unsigned int i = 0; i < jsonBuddiesIndex.size(); ++i)
    {
        Json::Value jsonBuddy = jsonBuddiesIndex[i];
        std::string buddy_email = "";
        std::string buddy_certid = "";
        std::string buddy_nick = "";
        if (jsonBuddy.isMember("email")){
            buddy_email = jsonBuddy["email"].asString();
        }
        if (jsonBuddy.isMember("nick")){
            buddy_nick = jsonBuddy["nick"].asString();
        }
        if (jsonBuddy.isMember("id")){
            buddy_certid = jsonBuddy["id"].asString();
        }
        std::string buddy_cert_file = "Buddies/";
        buddy_cert_file += buddy_email;
        buddy_cert_file += ".crt";
        std::string sBuddyCertPem = read_txt_file(buddy_cert_file);
        CX509Cert xbuddy;
        xbuddy.LoadCertFromPem(sBuddyCertPem);
        if (xbuddy.IsValid()){
            printf("Add buddy: %s\n", buddy_email.c_str());
            bm.AddBuddy(xbuddy);
        }
    }

    do {
        std::vector<std::string> vecMsg;
        bm.RecvMsg(vecMsg, NULL, NULL);
        for (std::vector<std::string>::iterator it = vecMsg.begin(); it != vecMsg.end(); it++){
            //printf("%s\n", it->c_str());
            std::string sJsonMsg = *it;
            poll(poll_url, sJsonMsg);
        }
        SLEEP(interval);

    }while(interval);

    return 0;
}

int onsendmsg(const std::string & info, void * userdata)
{
    (void)info; (void)userdata;
    return 0;
}

int onrecvmsg(const std::string & info, void * userdata)
{
    (void)info; (void)userdata;
    return 0;
}

int poll(const std::string & url, const std::string & param)
{
    Json::Value jsonPoll;
    jsonPoll ["method"] = url;
    Json::Reader reader;
    Json::Value jsonParam;
    reader.parse(param, jsonParam);
    jsonPoll ["param"] = jsonParam;
    std::cout<< jsonPoll.toStyledString()<<std::endl;

    CURL *curl;
    CURLcode res;
    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
      /* First set the URL that is about to receive our POST. This URL can
         just as well be a https:// URL if that is what should receive the
         data. */
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

      /* we use a self-signed test server, skip verification during debugging */
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      /* Now specify the POST data */
      char * urlencodedparam = curl_easy_escape(curl, param.c_str(), param.length());
      std::stringstream sstrem;
      sstrem << "param=" << urlencodedparam;
      curl_free(urlencodedparam);urlencodedparam = NULL;
      std::string strpostfield = sstrem.str();
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strpostfield.c_str());

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK){
        fprintf(stderr
                , "curl_easy_perform() failed: %s\n"
                , curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return 0;
}

void printHelp()
{
        std::stringstream sstrm;
        sstrm << "Usage: "<< "\r\n"
              << "BitmailClient <mode> [options]" << "\r\n"
              << "Mode enumeration:" << "\r\n"
                << "\t" << "--generate: generate certificates, and private key" << "\r\n"
                << "\t" << "--getid:    get identifier of a certificate" << "\r\n"
                << "\t" << "--sign:     sign a message"<< "\r\n"
                << "\t" << "--verify:   verify a signature"<< "\r\n"
                << "\t" << "--encrypt:  encrypt a message with buddy's certificate"<< "\r\n"
                << "\t" << "--decrypt:  decrypt encrypted message with your own private key"<< "\r\n"
                << "\t" << "--sendmsg:  send a signed message, if possible, encrypt it"<< "\r\n"
                << "\t" << "--recvmsg:  receive messages and poll it out"<< "\r\n"
                << "\t" << "--decrypt:  print this help"<< "\r\n"
              << "Options:" << "\r\n"
                <<"\t"<< "-B: buddy index file" <<"\r\n"
                <<"\t"<< "-b: buddy certificate"<<"\r\n"
                <<"\t"<< "-C: common name of certificate" <<"\r\n"
                <<"\t"<< "-E: email address of certificate"<<"\r\n"
                <<"\t"<< "-P: passphrase for private key"<<"\r\n"
                <<"\t"<< "-p: certificate for your profile"<<"\r\n"
                <<"\t"<< "-i: interval in seconds to receive message"<<"\r\n"
                <<"\t"<< "-k: private key file of your profile"<<"\r\n"
                <<"\t"<< "-m: message file"<<"\r\n"
                <<"\t"<< "-t: buddy's email address"<<"\r\n"
                <<"\t"<< "-s: signature file to verify"<<"\r\n"
                <<"\t"<< "-u: poll url to notify received messages "<<"\r\n"
                <<"\t"<< "-x: network configuration while sending or receiving"<<"\r\n"
              <<"Examples:" <<"\r\n"
                <<"\t"<<"[Generate]\r\n\t  BitmailClient  --generate \r\n\t\t\t -C commonName -E emailAddress -P passphrase"<< "\r\n"
                <<"\t"<<"[GetId]   \r\n\t  BitmailClient  --getid    \r\n\t\t\t -b buddyCertFile"<< "\r\n"
                <<"\t"<<"[Sign]    \r\n\t  BitmailClient  --sign     \r\n\t\t\t -p profile_cert_file \r\n\t\t\t -k profile_key_file \r\n\t\t\t -P passphrase \r\n\t\t\t -m msg_file"<< "\r\n"
                <<"\t"<<"[Verify]  \r\n\t  BitmailClient  --verify   \r\n\t\t\t -b buddy_cert_file   \r\n\t\t\t -s sig_file"<< "\r\n"
                <<"\t"<<"[Encrypt] \r\n\t  BitmailClient  --encrypt  \r\n\t\t\t -b buddy_cert_file   \r\n\t\t\t -m msg_file"<< "\r\n"
                <<"\t"<<"[Decrypt] \r\n\t  BitmailClient  --decrypt  \r\n\t\t\t -p profile_cert_file \r\n\t\t\t -k profile_key_file          \r\n\t\t\t -P passphrase               \r\n\t\t\t -m encrypted_msg_file"<< "\r\n"
                <<"\t"<<"[Send]    \r\n\t  BitmailClient  --sendmsg  \r\n\t\t\t -x net_config_file   \r\n\t\t\t -b buddy_cert_file(optional) \r\n\t\t\t -t email_of_buddy(optional) \r\n\t\t\t -p profile_cert_file \r\n\t\t\t -k profile_key_file \r\n\t\t\t -P passphrase \r\n\t\t\t -m msg_file"<< "\r\n"
                <<"\t"<<"[Receive] \r\n\t  BitmailClient  --recvmsg  \r\n\t\t\t -x net_config_file   \r\n\t\t\t -B buddy_index_file          \r\n\t\t\t -p profile_cert_file \r\n\t\t\t -k profile_key_file          \r\n\t\t\t -P passphrase               \r\n\t\t\t -i interval          \r\n\t\t\t -u poll_url"<< "\r\n"
              << "\r\n";
        fprintf(stdout, "%s", sstrm.str().c_str());
}
