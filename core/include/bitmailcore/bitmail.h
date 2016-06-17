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
	bmNoStranger     =     25,
	bmAlreadyExist   =     26,
	bmExpungeFail    =     27,
	bmFlagFail       =     28,
};

enum RTxState{
	RTS_Start = 0,
	RTS_Work  = 1,
	RTS_Done  = 2,
	RTS_Error = 3,
};

typedef int (* PollEventCB)(unsigned int count, void * p);

typedef int (* MessageEventCB)(const char * from, const char * msg, const char * certid, const char * cert, void * p);

typedef int (* RTxProgressCB)(RTxState, const char * info, void * userptr);

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
    
    int SetProxy(const std::string & ip
				, unsigned short port
				, const std::string & user
				, const std::string & password
				, bool fRemoteDNS);

    int EnableProxy(bool fEnable);

    /*UPNP feature should be external utility, e.g. forked from bittorrent*/

    int SetTxUrl(const std::string & u);
	
    std::string GetTxUrl() const;
	
    int SetTxLogin(const std::string & l);
    
	std::string GetTxLogin() const;
	
	int SetTxPassword(const std::string & p);
	
	std::string GetTxPassword() const;
	
	int SetRxUrl(const std::string & u);
	
	std::string GetRxUrl() const;
	
	int SetRxLogin(const std::string & l);
	
	std::string GetRxLogin() const;
	
	int SetRxPassword(const std::string & p);
	
	std::string GetRxPassword() const;

	// RTx Routines;
    int SendMsg(const std::string & to, const std::string & msg, RTxProgressCB cb = NULL, void * userp = NULL);

    int GroupMsg(const std::vector<std::string> & to, const std::string & msg, RTxProgressCB cb = NULL, void * userp = NULL);

    int CheckInbox(RTxProgressCB cb = NULL, void * userp = NULL);

    int StartIdle(unsigned int timeout/*Interval to re-idle*/, RTxProgressCB cb = NULL, void * userp = NULL);
    
    int Expunge(RTxProgressCB cb = NULL, void * userp = NULL);

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
					
	int SetPassphrase(const std::string & passphrase);

	std::string GetNick() const;

	std::string GetID() const;

    std::string GetEmail() const;

    std::string GetCert() const;

	std::string GetKey() const;
	
	int GetBits() const;
	
	std::string GetPassphrase() const;
	
	// Security routines
	std::string Encrypt(const std::string & text) const;
	
	std::string Decrypt(const std::string & code) const;

	// Certificate attributes
    std::string GetFriendNick(const std::string & email) const;

    std::string GetFriendCert(const std::string & email) const;

    std::string GetFriendID(const std::string & email) const;

    // Buddy
    int AddFriend(const std::string & email, const std::string & certpem);

    int RemoveFriend(const std::string & email);
	
	int GetFriends(std::vector<std::string> & vecEmails) const;

	bool HasFriend(const std::string & email) const;

	bool IsFriend(const std::string & email, const std::string & certpem) const;

protected:

    PollEventCB          m_onPollEvent;
    
    void               * m_onPollEventParam;
    
    MessageEventCB       m_onMessageEvent;

    void               * m_onMessageEventParam;
    
    CMailClient        * m_mc;

    // Key: email, Value: Certificate In Pem format
    std::map<std::string, std::string> m_buddies;
    
public:
    CX509Cert          * m_profile;

protected:
    static int EmailHandler(BMEventHead * h, void * userp);
};

#endif // BitMail_H
