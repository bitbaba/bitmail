#include <bitmailcore/bitmail.h>
#include <bitmailcore/email.h>

#include <curl/curl.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

# include <iostream>
# include <sstream>

#define SPACE_CHAR (' ')

static bool verboseFlag = !false;

struct TxCallback_t{
    void        * src   ;
    size_t        length;
    size_t        offset;
    CMailClient * self  ;
};

struct RxCallback_t {
  char        * memory;
  size_t        size  ;
  CMailClient * self  ;
};


CMailClient::CMailClient(BMEventCB cb, void * cbp)
: m_cb( cb )
, m_cbp( cbp )
, m_tx(NULL)
, m_rx(NULL)
, m_rxIdle(NULL)
, m_fEnableProxy(false)
, m_proxyIp("127.0.0.1")
, m_proxyPort(1080)
, m_fRemoteDNS(false)
{

}

CMailClient::~CMailClient()
{
    if (m_tx){
        curl_easy_cleanup((CURL * )m_tx);
        m_tx = NULL;
    }
    if (m_rx){
        curl_easy_cleanup((CURL * )m_rx);
        m_rx = NULL;
    }
    if (false && m_rxIdle){
        curl_easy_cleanup((CURL * )m_rxIdle);
        m_rxIdle = NULL;
    }
}

int CMailClient::InitTx(const std::string & url
          , const std::string & user
          , const std::string & pass)
{
    m_txurl = url;
    if (!m_txurl.empty() && '/' != m_txurl.at(m_txurl.length() - 1)){
        m_txurl += "/";
    }
    m_txuser= user;
    m_txpass= pass;
    return bmOk;
}

void CMailClient::SetTxUrl(const std::string & u)
{
    m_txurl = u;
}
void CMailClient::SetTxLogin(const std::string & l)
{
    m_txuser = l;
}
void CMailClient::SetTxPassword(const std::string & p)
{
    m_txpass = p;
}
std::string CMailClient::GetTxUrl() const
{
    return m_txurl;
}

std::string CMailClient::GetTxLogin() const
{
    return m_txuser;
}

std::string CMailClient::GetTxPassword() const
{
    return m_txpass;
}

int CMailClient::InitRx(const std::string & url
          , const std::string & user
          , const std::string & pass)
{
    m_rxurl = url;
    if (!m_rxurl.empty() && '/' != m_rxurl.at(m_rxurl.length() - 1)){
        m_rxurl += "/";
    }
    m_rxuser= user;
    m_rxpass= pass;
    return bmOk;
}

void CMailClient::SetRxUrl(const std::string & u)
{
    m_rxurl = u;
}
void CMailClient::SetRxLogin(const std::string & l)
{
    m_rxuser = l;
}
void CMailClient::SetRxPassword(const std::string & p)
{
    m_rxpass = p;
}

std::string CMailClient::GetRxUrl() const
{
    return m_rxurl;
}

std::string CMailClient::GetRxLogin() const
{
    return m_rxuser;
}

std::string CMailClient::GetRxPassword() const
{
    return m_rxpass;
}

int CMailClient::SetProxy(const std::string & ip
                                , unsigned short port
                                , const std::string & user
                                , const std::string & pass
                                , bool fRemoteDNS)
{
    m_proxyIp = ip;
    m_proxyPort = port;
    m_proxyUser = user;
    m_proxyPassword = pass;
    m_fRemoteDNS = fRemoteDNS;
    return bmOk;
}

void CMailClient::SetProxyIp(const std::string & ip)
{
    m_proxyIp = ip;
}

std::string CMailClient::GetProxyIp() const
{
    return m_proxyIp;
}

void CMailClient::SetProxyPort(unsigned short port)
{
    m_proxyPort = port;
}

unsigned short CMailClient::GetProxyPort() const
{
    return m_proxyPort;
}

void CMailClient::SetProxyUser(const std::string & user)
{
    m_proxyUser = user;;
}

std::string CMailClient::GetProxyUser() const
{
    return m_proxyUser;
}

void CMailClient::SetProxyPassword(const std::string & password)
{
    m_proxyPassword  = password;
}

std::string CMailClient::GetProxyPassword() const
{
    return m_proxyPassword;
}

void CMailClient::RemoteDNS(bool fEnable)
{
    m_fRemoteDNS = fEnable;
}

bool CMailClient::RemoteDNS() const
{
    return m_fRemoteDNS;
}

void CMailClient::EnableProxy(bool fEnable)
{
    m_fEnableProxy = fEnable;
}

bool CMailClient::EnableProxy() const
{
    return m_fEnableProxy;
}

int CMailClient::SendMsg(const std::string & from, const std::string & to, const std::string & encSignedMail
                        , RTxProgressCB cb, void * userp)
{
    std::vector<std::string> vecTo;
    vecTo.push_back(to);
    return SendMsg(from, vecTo, encSignedMail, cb, userp);
}

int CMailClient::SendMsg( const std::string & from, const std::vector<std::string> & to, const std::string & encSignedMail
                        , RTxProgressCB cb, void * userp)
{
    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Try to Send Message";
        cb(RTS_Start, txinfo.str().c_str(), userp);
    }
    std::stringstream sstrmMail;
    /**
     * <To> Field
     */
    sstrmMail << "To: ";
    for (std::vector<std::string>::const_iterator it = to.begin(); ;){
        sstrmMail << "<" << *it << ">";
        ++it;
        if (it == to.end()){
            break;
        }
        sstrmMail << "," ;
    }
    sstrmMail << "\r\n";

    /*
     * <From> Field
     */
    sstrmMail << "From: <"<< from <<">"
              << "\r\n";

    /**
     * <Subject> field
     */
    sstrmMail << "Subject: BitMail"
              << "\r\n";

    /**
     * <HeadTail>
     */
    sstrmMail << "\r\n";

    /**
     * <MimeBody> Field
     */
    sstrmMail << encSignedMail
              << "\r\n";

    /**
     * <MimeBodyTail>
     */
    sstrmMail << "\r\n";

    if (!m_tx){
        m_tx = curl_easy_init();
    }

    CURL * curl = (CURL * ) m_tx;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct TxCallback_t txcb;

    txcb.src = strdup((char *)sstrmMail.str().c_str());
    txcb.length = sstrmMail.str().length();
    txcb.offset = 0;
    txcb.self = this;

    /* Set url, username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_txuser.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_txpass.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, m_txurl.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());

    for (std::vector<std::string>::const_iterator it = to.begin(); it != to.end(); ++it){
        recipients = curl_slist_append(recipients, it->c_str());
    }

    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, OnTxfer);
    curl_easy_setopt(curl, CURLOPT_READDATA, &txcb);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Sending Message";
        cb(RTS_Work, txinfo.str().c_str(), userp);
    }
    res = curl_easy_perform(curl);

    curl_slist_free_all(recipients);

    // AOL spam will block to send message, without any curl error,
    // do anti-bot here: http://challenge.aol.com/spam.html
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        if (cb ){
            std::stringstream txinfo;
            txinfo<< "Message sent Failed";
            cb(RTS_Error, txinfo.str().c_str(), userp);
        }
        return bmTxFail;
    }

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Message sent Ok";
        cb(RTS_Done, txinfo.str().c_str(), userp);
    }

    return bmOk;
}

int CMailClient::CheckInbox(RTxProgressCB cb, void * userp)
{
    if (cb ){
        cb(RTS_Start, "CheckInbox", userp);
    }

    std::vector<MessageNo> msgnolist;
    this->GetUnseenMessageNoList(msgnolist);
    std::stringstream proginfo;
    proginfo << "msgnolist: [";
    for (std::vector<MessageNo>::const_iterator it = msgnolist.begin();
            it != msgnolist.end();
            ++it)
    {
        proginfo<< *it << ",";
    }
    proginfo << "]";

    if (cb ){
        cb(RTS_Work, proginfo.str().c_str(), userp);
    }

    for (std::vector<MessageNo>::iterator it = msgnolist.begin();
            it != msgnolist.end();
            it ++)
    {
        MessageNo msgno = *it;

        if (cb ){
            std::stringstream rxinfo;
            rxinfo<< "Try to Receive Message: [" << *it << "]";
            cb(RTS_Work, rxinfo.str().c_str(), userp);
        }

        std::string smime;
        this->GetUnseenMessageByMessageNo(msgno, smime);


        if (cb ){
            std::stringstream rxinfo;
            rxinfo<< "Rx Message size: [" <<  smime.length() << "]";
            cb(RTS_Work, rxinfo.str().c_str(), userp);
        }

        if (m_cb){
            BMEventMessage bmeMsg;
            bmeMsg.h.magic = BMMAGIC;
            bmeMsg.h.bmef = bmefMessage;
            bmeMsg.from = "";
            bmeMsg.msg  = smime;
            bmeMsg.certid = "";
            bmeMsg.cert = "";
            m_cb((BMEventHead *)&bmeMsg, m_cbp);
        }
        
        this->StoreFlag(msgno, "\\Seen");

        if (cb ){
            std::stringstream rxinfo;
            rxinfo<<"Rx Store \\Seen Flag to MsgNo: [" <<  msgno << "]";
            cb(RTS_Work, rxinfo.str().c_str(), userp);
        }
    }
    
    if (cb ){
        std::stringstream rxinfo;
        rxinfo<<"Rx Done";
        cb(RTS_Done, rxinfo.str().c_str(), userp);
    }

    return msgnolist.size();
}

int CMailClient::GetUnseenMessageNoList(std::vector<MessageNo> & msgnolist)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    std::string inboxurl = m_rxurl;
    inboxurl += "INBOX";
    curl_easy_setopt(curl, CURLOPT_URL, inboxurl.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRxfer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    /**
    * RFC3051: SEARCH UNSEEN
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");

    // http://linux.die.net/man/3/libcurl-tutorial
    /**
     * libcurl is completely thread safe,
     * except for two issues: signals and SSL/TLS handlers.
     * Signals are used for timing out name resolves (during DNS lookup)
     * - when built without c-ares support and not on Windows.
     *
     * Harry: when disable DNS client on windows, crash happens on debug.
     */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return bmRxFail;
    }

    std::string rawResult;
    rawResult.append(chunk.memory, chunk.size);

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);
    
    if (rawResult.empty()){
        return bmRxFail;
    }
    
    std::vector<std::string> lines;
    CMailClient::GetLines(rawResult, lines);
    if (lines.empty()){
        return bmRxFail;
    }
    
    rawResult = "";
    for (std::vector<std::string>::const_iterator it = lines.begin();it!=lines.end();++it){
        if (it->find("* SEARCH") != std::string::npos){
            rawResult = *it;
            break;
        }
    }
    
    if (rawResult.empty()){
        return bmRxFail;
    }

    /**
    * Simple parse to get msg no list
    */
    do {
        std::stringstream sstrm;
        sstrm << rawResult;
        std::string line;
        while(std::getline(sstrm, line, SPACE_CHAR)){
            if (line.empty()) continue;
            const char cPrefix = line.at(0);
            if ('0' <= cPrefix && cPrefix <= '9'){
                MessageNo msgno = (MessageNo)atoi(line.c_str());
                std::cout<<msgno<<std::endl;
                msgnolist.push_back(msgno);
            }
        }
    }while(0);

    return bmOk;
}

int CMailClient::GetUnseenMessageByMessageNo(MessageNo msgno, std::string & smime)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    std::stringstream strmmsgurl;
    strmmsgurl << m_rxurl << "INBOX" << "/;UID=" << msgno;
    std::string msgurl = strmmsgurl.str();
    std::cout<<msgurl<<std::endl;
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRxfer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }    

    /**
    * Clear dirty custom request on PERSISTENT CURL.
    */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

    // http://linux.die.net/man/3/libcurl-tutorial
    /**
     * libcurl is completely thread safe,
     * except for two issues: signals and SSL/TLS handlers.
     * Signals are used for timing out name resolves (during DNS lookup)
     * - when built without c-ares support and not on Windows.
     *
     * Harry: when disable DNS client on windows, crash happens on debug.
     */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return bmRxFail;
    }

    smime.append(chunk.memory, chunk.size);

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return bmOk;
}

int CMailClient::StoreFlag(MessageNo msgno, const std::string & flag)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    
    std::string msgurl = m_rxurl;
    msgurl += "INBOX";
    std::cout<<msgurl<<std::endl;
    
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRxfer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    
 
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }    

    /* Set the STORE command with the Deleted flag for message 1. Note that
     * you can use the STORE command to set other flags such as Seen, Answered,
     * Flagged, Draft and Recent. */
    char cmdf [100] = "";
    //sprintf(cmdf, "STORE %u +Flags \\Deleted", msgno);
    sprintf(cmdf, "STORE %u +Flags %s", msgno, flag.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmdf);
   
    // http://linux.die.net/man/3/libcurl-tutorial
    /**
     * libcurl is completely thread safe,
     * except for two issues: signals and SSL/TLS handlers.
     * Signals are used for timing out name resolves (during DNS lookup)
     * - when built without c-ares support and not on Windows.
     *
     * Harry: when disable DNS client on windows, crash happens on debug.
     */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return bmFlagFail;
    }

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return bmOk;
}

int CMailClient::Expunge(RTxProgressCB cb, void * userp)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    
    std::string msgurl = m_rxurl;
    msgurl += "INBOX";
    std::cout<<msgurl<<std::endl;
    
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnRxfer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    
 
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }    

    /* Set the EXPUNGE command, although you can use the CLOSE command if you
     * don't want to know the result of the STORE */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

    // http://linux.die.net/man/3/libcurl-tutorial
    /**
     * libcurl is completely thread safe,
     * except for two issues: signals and SSL/TLS handlers.
     * Signals are used for timing out name resolves (during DNS lookup)
     * - when built without c-ares support and not on Windows.
     *
     * Harry: when disable DNS client on windows, crash happens on debug.
     */
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return bmExpungeFail;
    }

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return bmOk;
}

/* Auxiliary function that waits on the socket. */
int CMailClient::CurlWait(void * handle, bool forRead, unsigned int & timeout_ms)
{
    CURL * curl = (CURL*)handle;
    /**
     * Get raw socket for system wait utility: select;
     */
    long sockextr = 0;
    CURLcode res = curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &sockextr);
    if(CURLE_OK != res)
    {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        return 1;
    }
    curl_socket_t sockfd = (curl_socket_t)sockextr;
    
    struct timeval tv;
    fd_set infd, outfd, errfd;

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec= (timeout_ms % 1000) * 1000;

    FD_ZERO(&infd);
    FD_ZERO(&outfd);
    FD_ZERO(&errfd);

    FD_SET(sockfd, &errfd); /* always check for error */

    if(forRead)
    {
        FD_SET(sockfd, &infd);
    }
    else
    {
        FD_SET(sockfd, &outfd);
    }

    /* select() returns the number of signalled sockets or -1 */
    int ret = select(sockfd + 1, &infd, &outfd, &errfd, &tv);
    if (ret > 0){
        timeout_ms = tv.tv_sec * 1000 + tv.tv_usec/1000;
        return ret;
    }else if (ret < 0){
        //TODO: Error Handler, maybe-reconnect
        timeout_ms = 0;
        return ret;
    }else{
        //TODO: Timeout handler
        timeout_ms = 0;
        return ret;
    }
}

void CMailClient::CurlRecv(void *handle, std::string & resp) {
    CURL * curl = (CURL * )handle;
    /* read the response */
    for(;;)
    {
      size_t iolen = 0;
      char buf[1024];
      CURLcode res = curl_easy_recv(curl, buf, 1024, &iolen);
      if(CURLE_OK != res){
          break;
      }
      curl_off_t nread = (curl_off_t)iolen;
      resp.append(buf, nread);
    }
}

void CMailClient::CurlSend(void *handle, const std::string & req) 
{
    CURL * curl = (CURL*)handle;
    std::cout<<"C: "<< req << std::endl;
    //TODO: Check writable
    size_t outl = 0;
    CURLcode ret = curl_easy_send(curl, req.c_str(), req.length(), &outl);
    return ;
}

int CMailClient::StartIdle(unsigned int timeout, RTxProgressCB cb, void * userp)
{
    /**
     * Prepare for IDLE connection handle
     * http://tools.ietf.org/html/rfc2177
     */
    if (!m_rxIdle){
        m_rxIdle = curl_easy_init();
    }

    CURL * curl = (CURL *)m_rxIdle;
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, m_rxurl.c_str());

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    
    curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 1L);
    
    /**
     * Do the connnect only
     */ 
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L); 
    CURLcode res = curl_easy_perform(curl);
    if( res != CURLE_OK ) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return bmIdleFail;
    }

    /**
     * Start Idle Mode
     * http://tools.ietf.org/html/rfc3501
     * Section 2.2.1 & 2.2.2
     */
    CurlSend(curl, "A001 SELECT \"INBOX\"\r\n");

    CurlSend(curl, "A002 IDLE\r\n");

    std::string idleresp = "";
    while(timeout != 0){
        std::cout<<"Timeout: "<<timeout<<std::endl;
        if (0 == CurlWait(curl, true, timeout)){
            break; // Timeout
        }
        CurlRecv(curl, idleresp);
        std::vector<std::string> idlecmd;
        CMailClient::GetLines(idleresp, idlecmd);
        for (std::vector<std::string>::const_iterator it = idlecmd.begin();
                it != idlecmd.end();
                ++it){
            if (verboseFlag){
                std::cout<<"S: "<< *it << std::endl;
            }
            const std::string line = *it;
            if (line.find("EXISTS") != std::string::npos){
                unsigned int msgcount = 0;
                sscanf(line.c_str(), "* %u EXISTS", &msgcount);
                //std::cout<<"MsgCount: " << msgcount << std::endl;
                if (m_cb){
                    BMEventMsgCount bmeMsgCount;
                    bmeMsgCount.h.magic = BMMAGIC;
                    bmeMsgCount.h.bmef = bmefMsgCount;
                    bmeMsgCount.msgcount = msgcount;
                    m_cb((BMEventHead *)&bmeMsgCount, m_cbp);
                }
            }
        }
    }

    /**
     * Stop Idle Mode
     */
    CurlSend(curl, "DONE\r\n");
    return bmOk;
}

int CMailClient::GetEmailAddrList(const std::string & addr, std::vector<std::string> & vecAddrs)
{
    /**
     * Format of address decipted in RFC821
     * <p1@domain1.com>,<p2@domain2.com>
     */
    char * buf = strdup(addr.c_str());
    const char * delims = "< \r\n\t,>";
    char * tok = strtok(buf, delims);
    while(tok != NULL){
        vecAddrs.push_back(tok);
        tok = strtok(NULL, delims);
    };

    free(buf);
    return 0;
}

int CMailClient::GetLines(std::string & str, std::vector<std::string> & lines)
{
    if (str.empty()) return 0;

    const char endc = str.at(str.length() - 1);
    
    char * buf = strdup(str.c_str());
    const char * delims = "\r\n";
    char * tok = strtok(buf, delims);
    while(tok != NULL){
        lines.push_back(tok);
        tok = strtok(NULL, delims);
    };
    free(buf);
    
    // Get the tail back;
    if (endc != '\r' && endc != '\n' && !lines.empty()){
        str = lines.at(lines.size() - 1);
        lines.pop_back();
        return 0;
    }
    
    str = "";
    return 0;
}

size_t CMailClient::OnTxfer(void *ptr, size_t size, size_t nmemb, void *sstrm)
{
    struct TxCallback_t * ctx = (struct TxCallback_t *)sstrm;

    if (ctx->offset >= ctx->length) {
        return 0;
    }

    size_t xfer_bytes = (size*nmemb < ctx->length-ctx->offset)
                      ? size*nmemb
                      : ctx->length - ctx->offset;

    memcpy(ptr, (unsigned char *)ctx->src + ctx->offset, xfer_bytes);

    ctx->offset += xfer_bytes;

    return (xfer_bytes); // 0 for EOF
}

size_t CMailClient::OnRxfer(void *contents, size_t size, size_t nmemb, void * userp)
{
    size_t realsize = size * nmemb;
    struct RxCallback_t *ctx = (struct RxCallback_t *)userp;

    ctx->memory = (char *)realloc(ctx->memory, ctx->size + realsize + 1);
    if(ctx->memory == NULL) {
        return 0;
    }

    memcpy(&(ctx->memory[ctx->size]), contents, realsize);
    ctx->size += realsize;
    ctx->memory[ctx->size] = 0;

    return realsize;
}
