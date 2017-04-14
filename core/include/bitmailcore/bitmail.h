#ifndef BitMail_H
#define BitMail_H
#include <string>
#include <map>
#include <set>
#include <vector>

#define BMVER_MAJOR (1)
#define BMVER_MINOR (0)
#define BMVER_TINY  (0)

//https://tools.ietf.org/html/rfc822
#define RECIP_SEPARATOR (";")


/* Forward decleration */
class CMailClient;
class CX509Cert;
struct BMEventHead;
class Brac;
class Brad;

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
    bmGrpExist       =     29,
    bmNoGrp          =     30,
    bmMemberExist    =     31,
    bmNoMember       =     32,
    bmSubExist       =     33,
    bmNoSub          =     34,
    bmWaitFail       =     35,
    bmWaitTimeout    =     36,
    bmNetworkError   =     37,
};

enum RTxState{
    RTS_Start = 0,
    RTS_Work  = 1,
    RTS_Done  = 2,
    RTS_Error = 3,
};

typedef int (* PollEventCB)(unsigned int count, void * p);

typedef int (* MessageEventCB)(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, void * p);

typedef int (* RTxProgressCB)(RTxState, const char * info, void * userptr);

class ILock{
public:
	virtual void Lock() = 0;
	virtual void TryLock(unsigned int ms) = 0;
	virtual void Unlock() = 0;
};

class ILockFactory{
public:
	virtual ILock * CreateLock() = 0;
	virtual void FreeLock(ILock * lock) = 0;
};


typedef int (* IRxOnDataCallback)(void * d, unsigned int l, void * userptr);

class IRx{
public:
	virtual void onData(IRxOnDataCallback cb, void * userptr) = 0;
};

class ITx{
public:
	virtual int Send(void * d, unsigned int l) = 0;
};

class IRTxFactory{
public:
	virtual IRx * CreateRx() = 0;
	virtual void FreeRx(IRx * irx) = 0;
	virtual ITx * CreateTx() = 0;
	virtual void FreeTx(ITx * itx) = 0;
};

class BitMail
{
public:
    BitMail(ILockFactory * lock = NULL, IRTxFactory * net = NULL);

    ~BitMail();

    unsigned int GetVersion() const;

public:
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
                , const std::string & password);

    void SetProxyIp(const std::string & ip);

    std::string GetProxyIp() const;

    void SetProxyPort(unsigned short port);

    unsigned short GetProxyPort() const;

    void SetProxyUser(const std::string & user);

    std::string GetProxyUser() const;

    void SetProxyPassword(const std::string & password);

    std::string GetProxyPassword() const;

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

    // Mulitpart
    static int splitMultiparts(const std::string & in, std::vector<std::string> & out);

    static std::string partType(const std::string & in);

    static std::string partParam(const std::string & in, const std::string & header, const std::string & pName);

    static std::string partContent(const std::string & in);

    static std::string partEncoding(const std::string & in);

    // Base64 codecs
    static std::string md5(const std::string & s);

    static std::string sha1(const std::string & s);

    static std::string sha256(const std::string & s);

    static std::string toBase64(const std::string & s);

    static std::string fromBase64(const std::string & s);

    static std::string toBase64Line(const std::string & s);

    static std::string fromBase64Line(const std::string & s);


    // RTx Routines
    int SendMsg(const std::vector<std::string> & friends, const std::string & msgs, RTxProgressCB cb = NULL, void * userptr = NULL);

    int OnPollEvent( PollEventCB cb, void * userp);

    int OnMessageEvent( MessageEventCB cb, void * userp);

    std::string EncMsg(const std::vector<std::string> & friends, const std::string & msg, bool fSignOnly);

    int DecMsg(const std::string & smime, std::string & from, std::string & nick, std::string & msg, std::string & certid, std::string & cert);

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

    // Friends
    int GetFriends(std::vector<std::string> & vecEmails) const;

    int AddFriend(const std::string & email, const std::string & certpem);

    int RemoveFriend(const std::string & email);

    bool HasFriend(const std::string & email) const;

    bool IsFriend(const std::string & email, const std::string & certpem) const;

    std::string GetFriendNick(const std::string & email) const;

    std::string GetFriendCert(const std::string & email) const;

    std::string GetFriendID(const std::string & email) const;

    // Groups
    int GetGroups(std::vector<std::string> & group) const;

    int AddGroup(const std::string & group);

    int RemoveGroup(const std::string & group);

    // Receips
   static std::vector<std::string> decodeReceips(const std::string & receips);

   static std::string serializeReceips(const std::vector<std::string> & vec_receips);

   // Session Key
   static std::string toSessionKey(const std::string & receip);

   static std::string toSessionKey(const std::vector<std::string> & vec_receips);

   static std::vector<std::string> fromSessionKey(const std::string & sessKey);

   static bool isGroupSession(const std::string & sessKey);

   std::string sessionName(const std::string & sessKey) const;

   void sessionName(const std::string & sessKey, const std::string & sessName);

   std::map<std::string, std::string> sessNames() const;

   std::string sessionLogo(const std::string & sessKey) const;

   void sessionLogo(const std::string & sessKey, const std::string & sessLogo);

   std::map<std::string, std::string> sessLogos() const;
protected:

    PollEventCB          m_onPollEvent;
    
    void               * m_onPollEventParam;
    
    MessageEventCB       m_onMessageEvent;

    void               * m_onMessageEventParam;
    
    CMailClient        * m_mc;

    // Key: email, Value: Certificate In Pem format
    std::map<std::string, std::string> m_buddies;
    
    // Groups, Key: GroupID, Value: vector of friends' email
    std::set<std::string> m_groups;

    // Sessions, local storage for all sessions.
    // Key: sessionKey
    // Value: session Names
    std::map<std::string, std::string> m_sessNames;

    // Key: session Key
    // Value: session logo image encoded in base64
    std::map<std::string, std::string> m_sessLogos;

    // Lock
    ILock * m_lock1;
    ILock * m_lock2;
    ILock * m_lock3;
    ILock * m_lock4;
    ILockFactory * m_lockCraft;

    // Custom network
    IRx * m_rx;
    ITx * m_tx;
    IRTxFactory * m_netCraft;

public:
    CX509Cert          * m_profile;

protected:
    static int EmailHandler(BMEventHead * h, void * userp);

    static std::string parseRFC822AddressList(const std::string & mime);
};


#endif // BitMail_H
