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

#if defined(WIN32)
#   define CLOSE(fd) closesocket(fd)
#else
#   define CLOSE(fd) do {					\
		while (close((fd)) == -1 && errno == EINTR)	\
			;					\
	} while (0)
#endif

Brac::Brac(int sockfd)
{

}
Brac::~Brac()
{

}

Brad::Brad(OnBradStatus cbStatus, OnBradClientConnect cbClient, void * userptr)
: m_statusHandler(cbStatus)
, m_clientHandler(cbClient)
, m_userptr(userptr)
{

}

int Brad::Startup(unsigned short port)
{
	struct sockaddr_in sin;

	m_serv = socket(AF_INET, SOCK_STREAM, 0);
	if (m_serv == -1){
		printf("socket error");
		return BradErrorSocket;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_serv, (const struct sockaddr*)&sin, sizeof(sin)) == -1){
		printf("bind error");
		return BradErrorBind;
	}

	if (listen(m_serv, SOMAXCONN) == -1){
		printf("listen error");
		return BradErrorListen;
	}

	if (m_statusHandler){
		m_statusHandler(BradStatusStartup, m_userptr);
	}

	while (true) {
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t len = sizeof(addr);
		int client = accept(m_serv, (struct sockaddr*)&addr, &len);
		if (client == -1) {
			break;
		}
		if (m_clientHandler){
			Brac * c = new Brac(client);
			m_clientHandler(c, m_userptr);
		}
	}

	if (m_statusHandler){
		m_statusHandler(BradStatusShutdown, m_userptr);
	}

	return BradErrorOk;
}

int Brad::CloseClient(Brac * c)
{
	if (c){
		delete c;
		c = NULL;
	}
	return 0;
}

int Brad::Shutdown()
{
	CLOSE(m_serv);
	m_serv = InvalidSocket;
	return 0;
}
