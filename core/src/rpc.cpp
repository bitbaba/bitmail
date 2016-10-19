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
# include <bitmailcore/bitmail_internal.h>
# include <bitmailcore/rpc.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <curl/curl.h>
#include <microhttpd.h>
#include <fcntl.h>

#if !defined(WIN32)
#define closesocket(fd) close(fd)
#include <arpa/inet.h>
#endif
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

/**
 * RPC Server implementation
 */

RPCServer::RPCServer(unsigned short port, BMEventCB cb, void * userp)
{
	m_port = port;
	m_cb = cb;
	m_userp = userp;
}

RPCServer::~RPCServer()
{

}

bool RPCServer::Startup() {
	// Tutorial: https://www.gnu.org/software/libmicrohttpd/tutorial.html
	// Manual: https://www.gnu.org/software/libmicrohttpd/manual/libmicrohttpd.html
	d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG , m_port
			, NULL, NULL
			, &answer_to_connection, this
			, MHD_OPTION_NOTIFY_COMPLETED, &request_completed
			, NULL, MHD_OPTION_END);
	return (d != NULL);
}

unsigned short RPCServer::GetPort() const
{
	return m_port;
}

bool RPCServer::Shutdown()
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
	RPCServer * self = (RPCServer *)cls;

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




