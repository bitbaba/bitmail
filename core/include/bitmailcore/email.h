#ifndef _EMAIL_H__
#define _EMAIL_H__

#include <string>
#include <vector>

typedef unsigned int MessageNo;

#define BMMAGIC (0xbeefbeef)

enum BMEventFlag{
	bmefSystem       =     0,
	bmefMsgCount     =     1,
	bmefMessage      =     2,
};

struct BMEventHead{
	unsigned int magic;
	BMEventFlag bmef;
};

struct BMEventSystem{
	BMEventHead h;
	unsigned int reserved;
};

struct BMEventMsgCount{
	BMEventHead h;
	unsigned int msgcount;
};

struct BMEventMessage{
	BMEventHead h;
	std::string from;
	std::string msg;
	std::string cert;
};

typedef int (* BMEventCB)(BMEventHead * h, void * userp);

class CMailClient
{
public:
    CMailClient( BMEventCB netcb, void * netcbparam);
    ~CMailClient();
public:
    int InitTx( const std::string & url
              , const std::string & user
              , const std::string & pass);

    int InitRx( const std::string & url
              , const std::string & user
              , const std::string & pass);

    int SendMsg( const std::string & from
               , const std::string & to
               , const std::string & msg);

    int SendMsg( const std::string & from
               , const std::vector<std::string> & to
               , const std::string & encSignedMail);

public:
    int CheckInbox();
    
    int StartIdle(unsigned int timeout);

public:
    static int GetEmailAddrList(const std::string & addr, std::vector<std::string> & vecAddrs);
    
    static int GetLines(std::string & str, std::vector<std::string> & lines);
    
protected:
    int GetUnseenMessageNoList(std::vector<MessageNo> & msgnolist);

    int GetUnseenMessageByMessageNo(MessageNo msgno, std::string & bitmsg);
    
    int StoreFlag(MessageNo msgno, const std::string & flag);
    
    int Expunge();

protected:
    /* Auxiliary function that waits on the socket. */     
    int  CurlWait(void *curl, bool forRead, unsigned int & timeout_ms);
     
	void CurlRecv(void *curl, std::string & resp);
     
	void CurlSend(void *curl, const std::string & req);

    
protected:
    static size_t OnTxfer(void *ptr, size_t size, size_t nmemb, void * userp);

    static size_t OnRxfer(void *ptr, size_t size, size_t nmemb, void * userp);

private:
    /**
    * Network callback and user param
    */
    BMEventCB   m_cb;
    void *      m_cbp;

    /**
    * Tx Hangle
    */
    void *      m_tx;
    std::string m_txurl;
    std::string m_txuser;
    std::string m_txpass;

    /**
    * Rx Handle
    */
    void *      m_rx;
    std::string m_rxurl;
    std::string m_rxuser;
    std::string m_rxpass;
    
    /**
     * Rx Handle for <IDLE> check
     */
    void *      m_rxIdle;
};


#endif
