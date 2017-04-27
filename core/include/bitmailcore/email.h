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
    bool config( const std::string & txUrl
              , const std::string & rxUrl
              , const std::string & login
              , const std::string & password
              , const std::string & proxy);

    std::string txUrl() const;
    std::string rxUrl() const;
    std::string login() const;
    std::string password() const;
    std::string proxy() const;

    /**
     * RTx Routines
     */
public:
    bool Tx( const std::string & from
               , const std::vector<std::string> & to
               , const std::string & encSignedMail
               , RTxProgressCB cb = NULL, void * userp = NULL);

    bool Rx(RTxProgressCB cb = NULL, void * userp = NULL);

    bool Expunge();

public:
    static std::vector<std::string> toAddrList(const std::string & addr);
    
    static std::vector<std::string> toLines(const std::string & str);
    
protected:
    bool GetUnseen(std::vector<MessageNo> & msgnolist);

    bool GetMsg(MessageNo msgno, std::string & bitmsg, RTxProgressCB cb = NULL, void * userp = NULL);
    
    bool StoreFlag(MessageNo msgno, const std::string & flag);

protected:
    static size_t OnTxfer(void *ptr, size_t size, size_t nmemb, void * userp);

    static size_t OnRxfer(void *ptr, size_t size, size_t nmemb, void * userp);

private:
    std::string m_txurl;
    std::string m_rxurl;
    std::string m_login;
    std::string m_password;
    std::string m_proxy;

    void *      m_tx;
    void *      m_rx;
    BMEventCB   m_cb;
    void *      m_cbp;
};


#endif
