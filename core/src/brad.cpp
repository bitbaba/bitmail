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
# include <bitmailcore/brad.h>

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

Brad::Brad(unsigned short port, InboundConnectionCB cb, void * userp)
	: port_(port)
	, m_cb(cb)
	  , m_userp(userp)
{

}

Brad::~Brad()
{

}

bool Brad::Startup()
{
	servfd_ = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = 0;
	my_addr.sin_port = htons(port_);

	if (bind(servfd_, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1)
	{
		closesocket(servfd_);
		servfd_ = CURL_SOCKET_BAD;
		return false;
	}

	if (listen(servfd_, 5) == -1){
		closesocket(servfd_);
		servfd_ = CURL_SOCKET_BAD;
		return false;
	}

	return true;
}

int Brad::WaitForConnections(unsigned int timeoutMs)
{
	fd_set rfds, wfds, efds;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(servfd_, &rfds);

	FD_ZERO(&wfds);
	FD_SET(servfd_, &wfds);

	FD_ZERO(&efds);
	FD_SET(servfd_, &efds);

	/* Wait up to five seconds. */
	tv.tv_sec = timeoutMs/1000;
	tv.tv_usec = timeoutMs%1000 * 1000;

	retval = select(servfd_ + 1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval == -1){
		return bmWaitFail;
	}
	if (retval == 0){
		return bmWaitTimeout;
	}

	if (!FD_ISSET(servfd_, &rfds)){
		return bmOk;
	}

	struct sockaddr_in peeraddr;
	socklen_t peeraddrlen = sizeof(peeraddr);
	int sockfd = accept(servfd_, (sockaddr*)&peeraddr, &peeraddrlen);
	if (sockfd == CURL_SOCKET_BAD){
		return bmOk;
	}

	if (!m_cb || bmOk != m_cb(sockfd, m_userp)){
		closesocket(sockfd);
	}

	return bmOk;
}

unsigned short Brad::GetPort() const
{
	return port_;
}

bool Brad::Shutdown()
{
	if (servfd_ != CURL_SOCKET_BAD){
		closesocket(servfd_);
		servfd_ = CURL_SOCKET_BAD;
	}
	return true;
}

Brac::Brac(const std::string & url, unsigned int timeout)
	: sockfd_(CURL_SOCKET_BAD)
	, curl_(NULL)
	, inbound_(false)
	, rxbuf_("")
	, email_("")
{
	CURLcode res;
	curl_ = curl_easy_init();
	if (curl_){
		curl_easy_setopt((CURL*)curl_, CURLOPT_URL, url.c_str());
		curl_easy_setopt((CURL*)curl_, CURLOPT_CONNECT_ONLY, 1l);
		if (timeout){
			curl_easy_setopt((CURL*)curl_, CURLOPT_CONNECTTIMEOUT, timeout);
		}
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
	, rxbuf_("")
	, email_("")
{
	if (sockfd_ != CURL_SOCKET_BAD){
		MakeNonBlocking();
	}
}

Brac::~Brac()
{
	Close();
}

bool Brac::IsValidSocket() const
{
	return (sockfd_ != CURL_SOCKET_BAD);
}

int Brac::sockfd() const
{
	return sockfd_;
}

int Brac::IsSendable() const
{
	fd_set rfds, wfds, efds;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(sockfd_, &rfds);

	FD_ZERO(&wfds);
	FD_SET(sockfd_, &wfds);

	FD_ZERO(&efds);
	FD_SET(sockfd_, &efds);

	/* Wait up to 0 second. */
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	retval = select(sockfd_ + 1, NULL, &wfds, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval == -1){
		return bmWaitFail;
	}
	if (retval == 0){
		return bmWaitTimeout;
	}

	return bmOk;
}

void Brac::Close()
{
	if (curl_){
		curl_easy_cleanup((CURL*)curl_);
		curl_ = NULL;
	}
	if (inbound_ && sockfd_ != CURL_SOCKET_BAD){
		closesocket(sockfd_);
	}
	sockfd_ = CURL_SOCKET_BAD;
}

bool Brac::Send(const std::string & smime, RTxProgressCB cb, void * userp)
{
	//TODO: encrypting message by BitMail::EncMsg(...);
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
	email_ = email;
}

std::string Brac::email(void) const
{
	return email_;
}




