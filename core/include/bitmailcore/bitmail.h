#ifndef BitMail_H
#define BitMail_H
#include <string>
#include <map>
#include <set>
#include <vector>

/* Forward decleration */
class  CMailClient;
class  CX509Cert  ;
struct BMEventHead;

enum RTxState{
    Rx_start    = 0,
    Rx_msglist  = 1,
    Rx_download = 2,
    Rx_doneload = 3,
    Rx_msg      = 4,
    Rx_delete   = 5,
    Rx_done     = 9,
    Rx_error    = 99,

    Tx_start    = 100,
    Tx_upload   = 103,
    Tx_updone   = 104,
    Tx_done     = 105,
    Tx_error    = 199,
};

class ILock{
public:
	virtual void Lock() = 0;
	virtual void TryLock(unsigned int ms) = 0;
	virtual void Unlock() = 0;
};

class ILockCraft{
public:
	virtual ILock * CreateLock() = 0;
	virtual void FreeLock(ILock * lock) = 0;
};

typedef int (* MessageEventCB)(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, const char * sigtime, bool encrypted, void * p);

typedef int (* RTxProgressCB)(RTxState, const char * info, void * userptr);

class BitMail
{
private:
    BitMail(ILockCraft * lockCraft = NULL);

    ~BitMail();

private:
    static bool EmailHandler(BMEventHead * h, void * userp);

    static std::string parseRFC822AddressList(const std::string & mime);

public:
    // Profile
    static BitMail * getInst(ILockCraft * lockCraft = NULL);

    static void freeInst();

    static std::string GetVersion();

    // Receips
    static std::vector<std::string> decodeReceips(const std::string & receips);

    static std::string serializeReceips(const std::vector<std::string> & vec_receips);

    static std::string serializeReceips(const std::string & receip);

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

    static std::string certId(const std::string & certpem);

    static std::string certCN(const std::string & certpem);

    static std::string certEmail(const std::string & certpem);

    static unsigned int certBits(const std::string & certpem);

public:
    bool Genesis(unsigned int bits
                , const std::string & nick
                , const std::string & email
                , const std::string & passphrase
                , const std::string & txurl
                , const std::string & rxurl
                , const std::string & login
                , const std::string & pass
                , const std::string & proxy);

    bool Import(const std::string & passphrase, const std::string & json);

    std::string Export() const;

    std::string email() const;

    std::string cert() const;

    std::string privKey() const;

    std::string pkcs12() const;

    bool UpdatePassphrase(const std::string & pass);

    std::string passphrase() const;

    bool SetupNetwork(const std::string & txurl, const std::string & rxurl, const std::string & login, const std::string & password, const std::string & proxy);

    std::string txUrl() const;

    std::string rxUrl() const;

    std::string login() const;

    std::string password() const;

    std::string proxy() const;

    bool blockNoSig() const;
    bool blockNoEnvelop() const;
    bool blockNoFriend() const;
    void blockNoSig(bool );
    void blockNoEnvelop(bool );
    void blockNoFriend(bool );
	
    // Contract
    std::string exec(const std::string & script);

    // Contacts
    std::vector<std::string> contacts() const;

    bool addContact(const std::string & emails);

    bool hasContact(const std::string & emails) const;

    bool removeContact(const std::string & emails);

    std::string contattrib(const std::string & emails, const std::string & att_name) const;

    bool contattrib(const std::string & emails, const std::string & att_name, const std::string & att_value);

    // RTx Routines
    bool Tx(const std::vector<std::string> & friends, const std::string & msgs, RTxProgressCB cb = NULL, void * userptr = NULL);

    bool Rx(MessageEventCB cb, void * msgcbp, RTxProgressCB rtxcb = NULL, void * rtxcbp = NULL);

    bool Expunge();

    // Security routines
    std::string Encrypt(const std::vector<std::string> & friends, const std::string & msg, bool fSignOnly);

    bool Decrypt(const std::string & smime, std::string & from, std::string & nick, std::string & msg, std::string & certid, std::string & cert, std::string & sigtime, bool * encrypted);

private:
    CX509Cert          * m_profile;
    CMailClient        * m_mc;
    MessageEventCB       m_onMessageEvent;
    void               * m_onMessageEventParam;
    ILockCraft         * m_lockCraft;
    ILock              * m_lockContacts;
    ILock              * m_lockNet;
    ILock              * m_lockProfile;
    std::string          contacts_;//TODO: reenter in mulithread
    bool                 blockNoSig_;
    bool                 blockNoEnvelop_;
    bool                 blockNoFriend_;
};


#endif // BitMail_H
