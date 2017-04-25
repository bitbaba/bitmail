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


CMailClient::CMailClient(BMEventCB cb, void * cbp)
            : m_cb(cb)
            , m_cbp(cbp)
            , m_tx(NULL)
            , m_rx(NULL)
            , m_proxy("soks5://127.0.0.1:1080/")
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

int CMailClient::Init(const std::string & txUrl
                        , const std::string & txUser
                        , const std::string & txPass
                        , const std::string & rxUrl
                        , const std::string & rxUser
                        , const std::string & rxPass
                        , const std::string & proxy)
{
    m_txurl = txUrl;
    if (!m_txurl.empty() && '/' != m_txurl.at(m_txurl.length() - 1)){
        m_txurl += "/";
    }
    m_txuser= txUser;
    m_txpass= txPass;

    m_rxurl = rxUrl;
    if (!m_rxurl.empty() && '/' != m_rxurl.at(m_rxurl.length() - 1)){
        m_rxurl += "/";
    }
    m_rxuser= rxUser;
    m_rxpass= rxPass;

    m_proxy = proxy;

    return bmOk;
}

std::string CMailClient::txUrl() const
{
    return m_txurl;
}

std::string CMailClient::txLogin() const
{
    return m_txuser;
}

std::string CMailClient::txPassword() const
{
    return m_txpass;
}

std::string CMailClient::rxUrl() const
{
    return m_rxurl;
}

std::string CMailClient::rxLogin() const
{
    return m_rxuser;
}

std::string CMailClient::rxPassword() const
{
    return m_rxpass;
}

std::string CMailClient::proxy() const
{
    return m_proxy;
}

int CMailClient::SendMsg( const std::string & from
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
    curl_easy_setopt(curl, CURLOPT_URL, m_txurl.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_txuser.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_txpass.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
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

        return bmTxFail;
    }

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Send Ok";
        cb(Tx_done, txinfo.str().c_str(), userp);
    }

    curl_easy_reset(curl);
    return bmOk;
}

int CMailClient::CheckInbox(RTxProgressCB cb, void * userp)
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

    return msgnolist.size();
}

int CMailClient::GetUnseen(std::vector<MessageNo> & msgnolist)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 0; /* test with 100k total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    std::string inboxurl = m_rxurl;
    inboxurl += "INBOX";
    curl_easy_setopt(curl, CURLOPT_URL, inboxurl.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
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

int CMailClient::GetMsg(MessageNo msgno, std::string & smime, RTxProgressCB cb, void * userp)
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 100000; /* test with 100k total length*/
    chunk.cb   = cb;
    chunk.userp= userp;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    std::stringstream strmmsgurl;
    strmmsgurl << m_rxurl << "INBOX" << "/;UID=" << msgno;
    std::string msgurl = strmmsgurl.str();
    std::cout<<msgurl<<std::endl;
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
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
    chunk.length = 0; /* test with 100k total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    std::string msgurl = m_rxurl;
    msgurl += "INBOX";
    std::cout<<msgurl<<std::endl;
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);    
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
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

int CMailClient::Expunge()
{
    struct RxCallback_t chunk;
    chunk.memory = (char *)::malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.self = this;
    chunk.length = 0; /* test with 100k total length*/
    chunk.cb   = NULL;
    chunk.userp= NULL;

    if (!m_rx){
        m_rx = curl_easy_init();
    }

    CURL *curl = (CURL * )m_rx;
    CURLcode res = CURLE_OK;

    std::string msgurl = m_rxurl;
    msgurl += "INBOX";
    std::cout<<msgurl<<std::endl;
    curl_easy_setopt(curl, CURLOPT_URL, msgurl.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME, m_rxuser.c_str()/*pop3 login, maybe diff from smtp*/);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_rxpass.c_str()/* pop3 passphrase*/);
    curl_easy_setopt(curl, CURLOPT_PROXY, m_proxy.c_str());
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
