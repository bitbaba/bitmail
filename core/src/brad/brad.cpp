/**
*
* Forked from https://github.com/lesmon/thd
* following is original license
*
*
Copyright (c) 2015, 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "brad.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <curl/curl.h>
#include <microhttpd.h>
#include <fcntl.h>

#define POSTBUFFERSIZE  512
#define MAXNAMESIZE     20
#define MAXANSWERSIZE   512


#define ISPOST(m)       ((m[0]=='p'||m[0]=='P')&&(m[1]=='o'||m[1]=='O')&&(m[2]=='s'||m[2]=='S')&&(m[3]=='t'||m[3]=='T'))

static int
defaultResponse (struct MHD_Connection *connection, const std::string & resp_body);

static void
request_completed (void *cls
				   , struct MHD_Connection *connection
				   , void **con_cls
				   , enum MHD_RequestTerminationCode toe);

static int
answer_to_connection (void *cls
					  , struct MHD_Connection *connection
					  , const char *url
					  , const char *method
					  , const char *version
					  , const char *upload_data
					  , size_t *upload_data_size
					  , void **con_cls);


HttpBrad::HttpBrad(unsigned short port, BMEventCB cb, void * userp)
{
	m_port = port;
	m_cb = cb;
	m_userp = userp;
}

HttpBrad::~HttpBrad()
{

}

bool HttpBrad::Startup() {
	// Tutorial: https://www.gnu.org/software/libmicrohttpd/tutorial.html
	// Manual: https://www.gnu.org/software/libmicrohttpd/manual/libmicrohttpd.html
	d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG , m_port
						   , NULL, NULL
						   , &answer_to_connection, this
						   , MHD_OPTION_NOTIFY_COMPLETED, &request_completed
						   , NULL, MHD_OPTION_END);
	return (d != NULL);
}

unsigned short HttpBrad::GetPort() const
{
	return m_port;
}

bool HttpBrad::Shutdown()
{
	  MHD_stop_daemon(d);
	  d = NULL;
	  return 0;
}


int defaultResponse (struct MHD_Connection *connection, const std::string & resp_body)
{
	struct MHD_Response *response = MHD_create_response_from_buffer(resp_body.length()
																    , (void *) resp_body.data()
																    , MHD_RESPMEM_PERSISTENT);
	if (!response)
		return MHD_NO;

	int ret = MHD_queue_response (connection, MHD_HTTP_OK, response);

	MHD_destroy_response (response);

	return ret;
}

void request_completed (void *cls
					, struct MHD_Connection *connection
					, void **con_cls
					, enum MHD_RequestTerminationCode toe)
{
	std::string * req_body = ( std::string *) *con_cls;

	if (req_body != NULL){
		delete req_body;
		req_body = NULL;
	}
}


int answer_to_connection (void *cls
						, struct MHD_Connection *connection
						, const char *url
						, const char *method
						, const char *version
						, const char *upload_data
						, size_t *upload_data_size
						, void **con_cls)
{
	HttpBrad * self = (HttpBrad *)cls;

	if (!ISPOST(method))
	{
		return defaultResponse (connection, "error: not supported method");
	}

	if (NULL == *con_cls)
	{
		std::string * req_body = new std::string();
		*con_cls = (void *) req_body;
		return MHD_YES;
	}

	std::string * req_body = ( std::string *) *con_cls;

	if (*upload_data_size != 0){
		req_body->append(upload_data, *upload_data_size);
		*upload_data_size = 0;
		return MHD_YES;
	}
	else {
		//printf("buflen: %d\n", req_body->length());
		//printf("buffer: \n%s\n", req_body->c_str());
        if (self->m_cb){
            BMEventMessage bmeMsg;
            bmeMsg.h.magic = BMMAGIC;
            bmeMsg.h.bmef = bmefMessage;
            bmeMsg.msg = *req_body;
            self->m_cb((BMEventHead *)&bmeMsg, self->m_userp);
        }
        // Async process
		std::string resp_body = "Ok";
		return defaultResponse(connection, resp_body);
	}
}

struct PostCallback_t{
    const void  * src   ;
    size_t        length;
    size_t        offset;
};

size_t HttpBrac::OnTxfer(void *ptr, size_t size, size_t nmemb, void *sstrm)
{
    struct PostCallback_t * ctx = (struct PostCallback_t *)sstrm;

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

bool HttpBrac::SendMsg(const std::string & url, const std::string & request, RTxProgressCB cb, void * userp)
{
	bool verboseFlag = true;

	if (cb ){
        std::stringstream txinfo;
        txinfo<< "Try to Send Message";
        cb(RTS_Start, txinfo.str().c_str(), userp);
    }

    CURL * curl = curl_easy_init();
    CURLcode res = CURLE_OK;
    struct PostCallback_t txcb;

    txcb.src = request.data();
    txcb.length = request.length();
    txcb.offset = 0;

    curl_slist *slist = NULL;
    // Disable http1.1 "Expect: 100"-> "100-continue"
    slist = curl_slist_append(slist, "Expect:");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    // Feed Data in callback
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, HttpBrac::OnTxfer);
    curl_easy_setopt(curl, CURLOPT_READDATA, &txcb);
    // Post setting
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.length());

    if (verboseFlag){
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    if (cb ){
        std::stringstream txinfo;
        txinfo<< "Sending Message";
        cb(RTS_Work, txinfo.str().c_str(), userp);
    }
    res = curl_easy_perform(curl);

    //TODO: free slist

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

Brad::Brad(unsigned short port, InboundConnectionCB cb, void * userp)
{

}

Brad::~Brad()
{

}

bool Brad::Startup()
{
	return true;
}

unsigned short Brad::GetPort() const
{
	return port_;
}

bool Brad::Shutdown()
{
	return true;
}

Brac::Brac(const std::string & url)
	: sockfd_(CURL_SOCKET_BAD)
	, curl_(NULL)
	, inbound_(false)
	, txoffset_(0)
	, rxoffset_(0)
	, txbuf_("")
	, rxbuf_("")
	, m_email("")
{
	CURLcode res;
	curl_ = curl_easy_init();
	if (curl_){
		curl_easy_setopt((CURL*)curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt((CURL*)curl_, CURLOPT_CONNECT_ONLY, 1l);
		res = curl_easy_perform((CURL *)curl_);
		if (res == CURLE_OK){
			curl_socket_t sockfd = CURL_SOCKET_BAD;
			res = curl_easy_getinfo((CURL *)curl_
					, CURLINFO_ACTIVESOCKET
					, &sockfd);
			if (res == CURLE_OK){
				sockfd_ = (int)sockfd;
			}
		}
	}
	if (CURL_SOCKET_BAD != sockfd_){
		MakeNonBlocking();
	}
}

Brac::Brac(int sockfd)
	: sockfd_(sockfd)
	, curl_(NULL)
	, inbound_(true)
	, txoffset_(0)
	, rxoffset_(0)
	, txbuf_("")
	, rxbuf_("")
	, m_email("")
{
	if (sockfd_ != CURL_SOCKET_BAD){
		MakeNonBlocking();
	}
}

Brac::~Brac()
{
	if (curl_){
		curl_easy_cleanup((CURL*)curl_);
		curl_ = NULL;
	}
}

bool Brac::IsValidSocket() const
{
	return (sockfd_ != CURL_SOCKET_BAD);
}

int Brac::sockfd() const
{
	return sockfd_;
}

bool Brac::Send(RTxProgressCB cb, void * userp)
{
	return true;
}

bool Brac::Recv(RTxProgressCB cb, void * userp)
{
	return true;
}

bool Brac::MakeNonBlocking()
{
#ifdef WIN32
   unsigned long mode = 1;
   return (ioctlsocket(sockfd_, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(sockfd_, F_GETFL, 0);
   if (flags < 0) {
	   return false;
   }
   flags = (flags|O_NONBLOCK);
   return (fcntl(sockfd_, F_SETFL, flags) == 0) ? true : false;
#endif
}

void Brac::email(const std::string & email)
{
	m_email = email;
}

std::string Brac::email(void) const
{
	return m_email;
}




