#ifndef BitMail_H
#define BitMail_H
#include <string>
#include <map>
#include <vector>

/* Forward decleration */
class CMailClient;
class CX509Cert;
struct BMEventHead;

enum BMError{
    bmOk             =     0,
    bmOutMem         =     1,
    bmTxInitFail     =     2,
    bmRxInitFail     =     3,
    bmSignFail       =     4,
    bmInvalidCert    =     5,
    bmEncryptFail    =     6,
    bmTxFail         =     7,
    bmRxFail         =     8,
    bmNewProfile     =     9,
    bmInvalidProfile =     10,
    bmNoBuddy        =     11,
    bmNoFrom         =     12,
    bmNoTo           =     14,
    bmNoSubject      =     15,
    bmNoMimeBody     =     16,
    bmNoMatchTo      =     17,
    bmStranger       =     18,
    bmDecryptFail    =     19,
    bmNotSigned      =     20,
    bmVerifyFail     =     21,
    bmInvalidParam   =     22,
    bmPushCert       =     23,
	bmIdleFail       =     24,
};

typedef int (* PollEventCB)(unsigned int count, void * p);

typedef int (* MessageEventCB)(const char * from, const char * msg, const char * cert, void * p);

class BitMail
{
public:
    BitMail();
    ~BitMail();
public:
    // Event Callback
    int OnPollEvent( PollEventCB cb, void * userp);
    
    int OnMessageEvent( MessageEventCB cb, void * userp);

    // Network
    int InitNetwork(const std::string & txurl
                    , const std::string & txuser
                    , const std::string & txpass
                    , const std::string & rxurl
                    , const std::string & rxuser
                    , const std::string & rxpass);
					
	int AllowStranger(bool fYes);

    int SendMsg(const std::string & to, const std::string & msg);

    int GroupMsg(const std::vector<std::string> & to, const std::string & msg);

    int CheckInbox();

    int StartIdle(unsigned int timeout/*Interval to re-idle*/);
    
    // Profile
    int CreateProfile(const std::string & commonName
                    , const std::string & email
                    , const std::string & passphrase
                    , unsigned int bits);

    int LoadProfile(const std::string & passphrase
                    , const std::string & keyPem
                    , const std::string & certPem);
    
    int SaveProfile(const std::string & passphrase
    				, std::string & keypem
					, std::string & certpem);

    std::string GetEmail() const;

    std::string GetCommonName() const;

    std::string GetCert() const;

    // Buddy
    int AddBuddy(const std::string & certpem);

    int RemoveBuddy(const std::string & email);

    std::string GetBuddyCert(const std::string & buddy);

protected:

    PollEventCB          m_onPollEvent;
    
    void               * m_onPollEventParam;
    
    MessageEventCB       m_onMessageEvent;

    void               * m_onMessageEventParam;
    
    CMailClient        * m_mc;

    std::map<std::string, std::string> m_buddies;
    
public:
    CX509Cert          * m_profile;

protected:
    static int EmailHandler(BMEventHead * h, void * userp);
};

#endif // BitMail_H
