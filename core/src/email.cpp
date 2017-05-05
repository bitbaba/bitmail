# include <bitmailcore/bitmail_internal.h>
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
    const void  * src   ;
    size_t        length;
    size_t        offset;
    CMailClient * self  ;
    RTxProgressCB cb    ;
    void *        userp ;
};

struct RxCallback_t {
  char        * memory;
  size_t        size  ;
  size_t        length;
  CMailClient * self  ;
  RTxProgressCB cb    ;
  void *        userp ;
};

#define DefaultProxy ("soks5://127.0.0.1:1080/")

CMailClient::CMailClient(ILock * lock, BMEventCB cb, void * cbp)
    : m_txurl(""), m_rxurl(""), m_login(""), m_password(""), m_proxy("")
    , m_tx(NULL), m_rx(NULL)
    , m_cb(cb), m_cbp(cbp)
    , m_lock(lock)
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
}

bool CMailClient::config(const std::string & txUrl
                        , const std::string & rxUrl
                        , const std::string & login
                        , const std::string & password
                        , const std::string & proxy)
{
    ScopedLock scope(m_lock);

    m_txurl = txUrl;
    if (!m_txurl.empty() && '/' != m_txurl.at(m_txurl.length() - 1)){
        m_txurl += "/";
    }

    m_rxurl = rxUrl;
    if (!m_rxurl.empty() && '/' != m_rxurl.at(m_rxurl.length() - 1)){
        m_rxurl += "/";
    }

    m_login= login;
    m_password= password;

    m_proxy = proxy;

    return true;
}

std::string CMailClient::txUrl() const
{
    ScopedLock scope(m_lock);

    return m_txurl;
}

std::string CMailClient::rxUrl() const
{
    ScopedLock scope(m_lock);

    return m_rxurl;
}

std::string CMailClient::login() const
{
    ScopedLock scope(m_lock);

    return m_login;
}

std::string CMailClient::password() const
{
    ScopedLock scope(m_lock);

    return m_password;
}

std::string CMailClient::proxy() const
{
    ScopedLock scope(m_lock);

    return m_proxy;
}

bool CMailClient::Tx( const std::string & from
                        , const std::vector<std::string> & to
                        , const std::string & encSignedMail
                        , RTxProgressCB cb, void * userp)
{
    // Spam: http://challenge.aol.com/en/US/spam.html

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Sending";
        cb(Tx_start, txinfo.str().c_str(), userp);
    }

    if (!m_tx){
        m_tx = curl_easy_init();
    }

    CURL * curl = (CURL * ) m_tx;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct TxCallback_t txcb;

    std::stringstream sstrmMail;

    /*
     * <From> Field
     */
    sstrmMail << "From: <"<< from <<">"
              << "\r\n";

    /*
     * <To> Field
     */
    sstrmMail     << "To:        \r\n";
    for (std::vector<std::string>::const_iterator it = to.begin(); ;){
        sstrmMail << "           <" << *it << ">";
        ++it;
        if (it == to.end()){
            break;
        }
        sstrmMail << ",\r\n" ;
    }
    sstrmMail     << "\r\n";

    /**
     * <Subject> field
     */
    sstrmMail << "Subject: BitMail"
              << "\r\n";

    /**
     *<MimeBody>
     */
    sstrmMail << encSignedMail;

    std::string strMailBody = sstrmMail.str();
    txcb.src = (void*)strMailBody.data();
    txcb.length = strMailBody.length();
    txcb.offset = 0;
    txcb.self = this;
    txcb.cb = cb;
    txcb.userp = userp;

    /* Set url, username and password */
    do {
        ScopedLock scope(m_lock);
        curl_easy_setopt(curl, CURLOPT_URL, m_txurl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_login.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
    }while(0);

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
        txinfo<< "Uploading Message";
        cb(Tx_upload, txinfo.str().c_str(), userp);
    }
    res = curl_easy_perform(curl);

    curl_slist_free_all(recipients);

    // AOL spam will block to send message, without any curl error,
    // do anti-bot here: http://challenge.aol.com/spam.html
    if(res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        if (cb ){
            std::stringstream txinfo;
            txinfo<< "Fail";
            cb(Tx_error, txinfo.str().c_str(), userp);
        }
        // do-reset
        curl_easy_reset(curl);

        return false;
    }

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Send Ok";
        cb(Tx_done, txinfo.str().c_str(), userp);
    }

    curl_easy_reset(curl);
    return true;
}

bool CMailClient::Rx(RTxProgressCB cb, void * userp)
{
    if (cb ){
        cb(Rx_start, "", userp);
    }

    std::vector<MessageNo> msgnolist;
    this->GetUnseen(msgnolist);

    if (cb ){
        std::string info = "list: ";
        for (std::vector<MessageNo>::const_iterator it = msgnolist.begin();it != msgnolist.end();++it){
            char buf[100] = ""; sprintf(buf, "%d", *it); info += buf; info += ",";
        }
        cb(Rx_msglist, info.c_str(), userp);
    }

    for (std::vector<MessageNo>::iterator it = msgnolist.begin();it != msgnolist.end();it ++){
        MessageNo msgno = *it;

        if (cb ){
            std::stringstream rxinfo;
            rxinfo<< "Recv:[" << *it << "]";
            cb(Rx_msg, rxinfo.str().c_str(), userp);
        }

        std::string smime;
        this->GetMsg(msgno, smime, cb, userp);

        if (cb ){
            std::stringstream rxinfo;
            rxinfo<< "Size:[" <<  smime.length() << "]";
            cb(Rx_doneload, rxinfo.str().c_str(), userp);
        }

        if (m_cb){
            BMEventMessage bmeMsg;
            bmeMsg.h.magic = BMMAGIC;
            bmeMsg.h.bmef  = bmefMessage;
            bmeMsg.msg     = smime;
            m_cb((BMEventHead *)&bmeMsg, m_cbp);
        }
        
        this->StoreFlag(msgno, "\\Seen");
        if (cb ){
            std::stringstream rxinfo;
            rxinfo<<"Seen : [" <<  msgno << "]";
            cb(Rx_delete, rxinfo.str().c_str(), userp);
        }

        this->StoreFlag(msgno, "\\Deleted");
        if (cb ){
            std::stringstream rxinfo;
            rxinfo<<"Delete : [" <<  msgno << "]";
            cb(Rx_delete, rxinfo.str().c_str(), userp);
        }
    }
    
    if (cb ){
        std::stringstream rxinfo;
        rxinfo<<"Fetch Ok";
        cb(Rx_done, rxinfo.str().c_str(), userp);
    }

    return true;
}

bool CMailClient::GetUnseen(std::vector<MessageNo> & msgnolist)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 0; /* test with 0 byte total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    do {
        ScopedLock scope(m_lock);
        std::string inboxurl = m_rxurl; inboxurl += "INBOX";
        curl_easy_setopt(curl, CURLOPT_URL, inboxurl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_login.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
    }while(0);

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
        return false;
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
        return false;
    }
    
    std::vector<std::string> lines = CMailClient::toLines(rawResult);
    if (lines.empty()){
        return false;
    }
    
    rawResult.clear();
    for (std::vector<std::string>::const_iterator it = lines.begin();it!=lines.end();++it){
        if (it->find("* SEARCH") != std::string::npos){
            rawResult = *it;
            break;
        }
    }
    
    if (rawResult.empty()){
        return false;
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

    return true;
}

bool CMailClient::GetMsg(MessageNo msgno, std::string & smime, RTxProgressCB cb, void * userp)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 1; /* test with 1 Byte total length*/
    chunk.cb   = cb;
    chunk.userp= userp;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    do {
        ScopedLock scope(m_lock);
        std::stringstream strmmsgurl; strmmsgurl << m_rxurl << "INBOX" << "/;UID=" << msgno;
        curl_easy_setopt(curl, CURLOPT_URL, strmmsgurl.str().c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_login.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
    }while(0);

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
        return false;
    }

    smime.append(chunk.memory, chunk.size);

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return true;
}

bool CMailClient::StoreFlag(MessageNo msgno, const std::string & flag)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 0; /* test with 0 byte total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    do {
        ScopedLock scope(m_lock);
        std::string msgurl = m_rxurl; msgurl += "INBOX";
        curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_login.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
    }while(0);

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
        return false;
    }

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return true;
}

bool CMailClient::Expunge()
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 0; /* test with 0 byte total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    do {
        ScopedLock scope(m_lock);
        std::string msgurl = m_rxurl; msgurl += "INBOX";
        curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, m_login.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
    }while(0);

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
        return false;
    }

    do {
        free(chunk.memory);
        chunk.memory = 0;
        chunk.size = 0;
        chunk.self = NULL;
    }while(0);

    return true;
}

std::vector<std::string> CMailClient::toAddrList(const std::string & addr)
{
    /**
     * Format of address decipted in RFC821
     * <p1@domain1.com>,<p2@domain2.com>
     */
    std::vector<std::string> vec_addrs;
    char * buf = strdup(addr.c_str());
    char * tok = strtok(buf, "< \r\n\t,>");
    do{
        if (tok != NULL) vec_addrs.push_back(tok);
    }while(tok = strtok(NULL, "< \r\n\t,>"));
    free(buf);
    return vec_addrs;
}

std::vector<std::string> CMailClient::toLines(const std::string & str)
{
    std::vector<std::string> lines;

    if (str.empty()) {
        return lines;
    }

    char * buf = strdup(str.c_str());
    char * tok = strtok(buf, "\r\n");
    do {
        if (tok != NULL) lines.push_back(tok);
    }while(tok = strtok(NULL, "\r\n"));

    free(buf);
    
    return lines;
}

size_t CMailClient::OnTxfer(void *ptr, size_t size, size_t nmemb, void *sstrm)
{
    struct TxCallback_t * ctx = (struct TxCallback_t *)sstrm;

    if (ctx->offset >= ctx->length) {
        if (ctx->cb){
            ctx->cb(Tx_updone, "100.00", ctx->userp);
        }
        return 0;
    }

    size_t xfer_bytes = (size*nmemb < ctx->length-ctx->offset)
                      ? size*nmemb
                      : ctx->length - ctx->offset;

    memcpy(ptr, (unsigned char *)ctx->src + ctx->offset, xfer_bytes);

    ctx->offset += xfer_bytes;

    if (ctx->cb){
        char percentage [100] = "";
        sprintf(percentage, "%.2f", ctx->offset * 100.0f / ctx->length);
        ctx->cb(Tx_upload, percentage, ctx->userp);
    }

    return (xfer_bytes); // 0 for EOF
}

size_t CMailClient::OnRxfer(void *contents, size_t size, size_t nmemb, void * userp)
{
    size_t realsize = size * nmemb;
    struct RxCallback_t *ctx = (struct RxCallback_t *)userp;

    ctx->memory = (char *)realloc(ctx->memory, ctx->size + realsize + 1);
    if(ctx->memory == NULL) {
        if (ctx->cb){
            ctx->cb(Rx_doneload, "100.00", ctx->userp);
        }
        return 0;
    }

    memcpy(&(ctx->memory[ctx->size]), contents, realsize);
    ctx->size += realsize;
    ctx->memory[ctx->size] = 0;

    if (ctx->cb){
        char percentage [100] = "";
        sprintf(percentage, "%.2f", ctx->size * 100.0f / 500000);
        ctx->cb(Rx_download, percentage, ctx->userp);
    }

    return realsize;
}
