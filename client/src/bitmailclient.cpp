#define CURL_STATICLIB
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <bitmailcore/bitmail.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#if defined(__GNUC__)
    #include <unistd.h>
#endif

#if !defined(SLEEP) && !defined(WIN32)
#define SLEEP(x) ::Sleep((x) * 1000)
#endif

#if !defined(CRLF)
#define CRLF ("\r\n")
#endif


static std::string guessTxUrl(const std::string & email);
static std::string guessRxUrl(const std::string & email);
static int onNewMessage(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, const char * sigtime, bool encrypted, void * p);
static int onRTx(RTxState, const char * info, void * userptr);
std::string readTxt(const std::string &path);
int writeTxt(const std::string &path, const std::string & s);

static struct option long_options[] = {
   {"gen",  no_argument,       0,  0 },
   {"tx",   no_argument,       0,  0 },
   {"rx",   no_argument,       0,  0 },
   {"help", no_argument,       0,  0 },
   {0,      0,                 0,  0 }
};

enum BMCMode{
    undef = 0,
    gen = 1,
    tx = 7,
    rx = 8,
    help = 9,
};

static int GenerateProfile(const std::string & profile
                           , unsigned int bits
                           , const std::string & common_name
                           , const std::string & email
                           , const std::string & passphrase
                           , const std::string & username
                           , const std::string & password);

static int Tx(const std::string & profile, const std::string & passphrase
            , const std::string & receips
            , const std::string & message);

static int Rx(const std::string & profile, const std::string & passphrase);

/**
 * @brief printHelp
 * @param void
 * @return void
 */
static void PrintHelp();

int main(int argc, char **argv)
{
   unsigned int bits = 1024;
   std::string common_name
           , email
           , passphrase
           , username
           , password
           , profile
           , msg
           , receips;

   BMCMode mode = BMCMode(undef);

   while (1) {
       int option_index = 0;
       int c = getopt_long(argc , argv , "c:b:P:C:E:u:p:m:t:"  // short options
               , long_options // long options
               , &option_index);
       if (c == -1){
           break;
       }

       switch (c) {
       case 0:
           if (!strcmp("gen", long_options[option_index].name)){
               mode = BMCMode(gen);
           }else if (!strcmp("tx", long_options[option_index].name)){
               mode = BMCMode(tx);
           }else if (!strcmp("rx", long_options[option_index].name)){
               mode = BMCMode(rx);
           }else if (!strcmp("help", long_options[option_index].name)){
               mode = BMCMode(help);
           }else{
               mode = BMCMode(undef);
           }
           break;
       case 'c':
           profile = optarg;
           break;
       case 'b':
           bits = atoi(optarg);
           break;
       case 'P':
           passphrase = optarg;
           break;
       case 'C':
           common_name = optarg;
           break;
       case 'E':
           email = optarg;
           break;
       case 'u':
           username = optarg;
           break;
       case 'p':
           password = optarg;
           break;
       case 'm':
           msg = optarg;
           break;
       case 't':
           receips = optarg;
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
   case BMCMode(gen):
       return GenerateProfile(profile, bits, common_name, email, passphrase, username, password);
   case BMCMode(tx):
       return Tx(profile, passphrase, receips, msg);
   case BMCMode(rx):
       return Rx(profile, passphrase);
   case BMCMode(help):
   case BMCMode(undef):
       PrintHelp(); return 0;
   default:
       return 1;
   }
}

int GenerateProfile(const std::string & profile
                   , unsigned int bits
                   , const std::string & common_name
                   , const std::string & email
                   , const std::string & passphrase
                   , const std::string & username
                   , const std::string & password)
{
    if (BitMail::getInst()->Genesis(bits, common_name, email, passphrase, guessTxUrl(email), guessRxUrl(email), username, password, "")){
        writeTxt(profile, BitMail::getInst()->Export());
    }
    return 0;
}

int Tx(const std::string & profile, const std::string & passphrase, const std::string & receips, const std::string & message)
{
    std::string config = readTxt(profile);

    if (!BitMail::getInst()->Import(passphrase, config)){
        std::cout<< "Import profile failure";
    }

    std::vector<std::string> friends = BitMail::decodeReceips(receips);
    if (friends.size()){
        BitMail::getInst()->Tx(friends, message, false, onRTx, NULL);
    }
    return 0;
}

int Rx(const std::string & profile, const std::string & passphrase)
{
    std::string config = readTxt(profile);

    if (!BitMail::getInst()->Import(passphrase, config)){
        std::cout<< "Import profile failure";
    }

    BitMail::getInst()->Rx(onNewMessage, NULL, onRTx, NULL);
    return 0;
}

void PrintHelp()
{
    std::stringstream sstrm;
    sstrm << "Usage: "<< "\r\n"
          << "BitmailClient <mode> [options]" << "\r\n"
          << "Mode enumeration:" << "\r\n"
            << "\t" << "--gen: generate certificates, and private key" << "\r\n"
            << "\t" << "--tx:  send a signed message, if possible, encrypt it"<< "\r\n"
            << "\t" << "--rx:  receive messages and poll it out"<< "\r\n"
          << "Options:" << "\r\n"
            <<"\t"<< "-b: bits of public key"<<"\r\n"
            <<"\t"<< "-C: common name of certificate" <<"\r\n"
            <<"\t"<< "-E: email address of certificate"<<"\r\n"
            <<"\t"<< "-P: passphrase for private key"<<"\r\n"
            <<"\t"<< "-u: login"<<"\r\n"
            <<"\t"<< "-p: login password"<<"\r\n"
            <<"\t"<< "-c: configuration, the profile"<<"\r\n"
            <<"\t"<< "-m: message"<<"\r\n"
            <<"\t"<< "-t: receips, if group, splitted by semi-commas"<<"\r\n"
          <<"Examples:" <<"\r\n"
            <<"\t"<<"[Generate]\r\n\t  BitmailClient  --gen \r\n\t\t\t -c profile -b bits -C commonName -E emailAddress -P passphrase -u username -p password"<< "\r\n"
            <<"\t"<<"[Send]    \r\n\t  BitmailClient  --tx  \r\n\t\t\t -c profile -P passphrase -t receips -m msg"<< "\r\n"
            <<"\t"<<"[Receive] \r\n\t  BitmailClient  --rx  \r\n\t\t\t -c profile -P passphrase"<< "\r\n"
          << "\r\n";
    fprintf(stdout, "%s", sstrm.str().c_str());
}

int onNewMessage(const char * from
                 , const char * receips
                 , const char * msg
                 , unsigned int msglen
                 , const char * certid
                 , const char * cert
                 , const char * sigtime
                 , bool encrypted
                 , void * /*p*/)
{
    std::cout<<"From: "<< from << CRLF;
    std::cout<<"Receips: "<< receips << CRLF;
    std::cout<<"Encrypted: "<< (encrypted ? "Yes" : "No") << CRLF;
    std::cout<<"SigTime: "<< sigtime << CRLF;
    std::cout<<"ID: "<< certid << CRLF;
    std::cout<<"Certificate: "<< cert << CRLF;
    std::cout<<"Message: "<< msg << CRLF;
    std::cout<<"Length: "<< msglen << CRLF;
    std::cout<<CRLF << CRLF;
    return 0;
}

int onRTx(RTxState st, const char * info, void * /*userptr*/)
{
    std::cout<<"State: " << st << CRLF;
    std::cout<<"Info: " << info << CRLF;
    std::cout<< CRLF << CRLF;
    return 0;
}

std::string guessTxUrl(const std::string & email)
{
    std::string txurl = "smtps://smtp.";
    if (email.find('@') != std::string::npos){
        txurl += email.substr(email.find("@") + 1);
    }
    return txurl;
}

std::string guessRxUrl(const std::string & email)
{
    std::string rxurl = "imaps://imap.";
    if (email.find('@') != std::string::npos){
        rxurl += email.substr(email.find("@") + 1);
    }
    return rxurl;
}

std::string readTxt(const std::string &path)
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

int writeTxt(const std::string &path, const std::string & s)
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
