#ifndef _EMAIL_H__
#define _EMAIL_H__

#include <string>
#include <vector>

#include <bitmailcore/bitmail.h>

class CMailClient
{
public:
    CMailClient( BMEventCB netcb, void * netcbparam);
    ~CMailClient();
public:
    int Init( const std::string & txUrl
              , const std::string & txUser
              , const std::string & txPass
              , const std::string & rxUrl
              , const std::string & rxLogin
              , const std::string & rxPassword
              , const std::string & proxy);

    std::string txUrl() const;
    std::string txLogin() const;
    std::string txPassword() const;

    std::string rxUrl() const;
    std::string rxLogin() const;
    std::string rxPassword() const;

    std::string proxy() const;


    /**
     * RTx Routines
     */
public:
    int SendMsg( const std::string & from
               , const std::vector<std::string> & to
               , const std::string & encSignedMail
               , RTxProgressCB cb = NULL, void * userp = NULL);

    int CheckInbox(RTxProgressCB cb = NULL, void * userp = NULL);
    
    int Expunge();

    static int GetEmailAddrList(const std::string & addr, std::vector<std::string> & vecAddrs);
    
    static int GetLines(std::string & str, std::vector<std::string> & lines);
    
protected:
    int GetUnseen(std::vector<MessageNo> & msgnolist);

    int GetMsg(MessageNo msgno, std::string & bitmsg, RTxProgressCB cb = NULL, void * userp = NULL);
    
    int StoreFlag(MessageNo msgno, const std::string & flag);

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
     * Socks5 proxy
     */
    std::string m_proxy;
};


#endif
