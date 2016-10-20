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

#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <string>

#include <bitmailcore/bitmail.h>

/**
 * identifier of bra
 * hex present of sha256 hash of bra
 */
typedef std::string BraId;

/**
 * Bra type
 */
enum Brat{
	brat_undef   = 0,
	brat_bitmail = 1,
	brat_image   = 2,
	brat_sound   = 3,
	brat_video   = 4,
	brat_live    = 5,
};

enum BradStatus{
	BradStatusOk = 0,
	BradStatusStartup = 1,
	BradStatusShutdown = 2,
};

enum BradError{
	BradErrorOk = 0,
	BradErrorBind = 1,
	BradErrorListen = 2,
	BradErrorSocket = 3,
};

#define BRAMAGIC    ("Brad")
#define BRAMAGICLEN (4)

/**
 ***************************************************************
 * Bra Daemon class
 ***************************************************************
 */
// callback when inbound connection comming
typedef int (* InboundConnectionCB)(int sockfd, void * userp);

/**
 * Bra Daemon class
 */
class Brad{
public:
	explicit Brad(unsigned short port, InboundConnectionCB cb, void * userp);
	~Brad();
public:
	bool Startup();
	int  WaitForConnections(unsigned int timeoutMs);
	unsigned short GetPort() const;
	bool Shutdown();
private:
	unsigned short port_;
	int servfd_;
	InboundConnectionCB m_cb;
	void * m_userp;
};

/**
 ***************************************************************
 * Bra Connection class
 ***************************************************************
 */
class Brac{
public:
	/**
	 * create a outbound connection to Brad
	 */
	explicit Brac(const std::string & url, unsigned int timeout, BMEventCB cb, void * cbp);
	/**
	 * create a inbound connection accepted by Brad
	 */
	explicit Brac(int sockfd, BMEventCB cb, void * cbp);
	~Brac();
public:
	bool IsValidSocket() const;
	bool IsKeepAlive(unsigned int keepalive) const;
	int  sockfd() const;
	int  IsSendable() const;
	void Close();
	bool Send(const std::string & smime, RTxProgressCB cb, void * userp);
	bool Recv(RTxProgressCB cb, void * userp);
	void email(const std::string & nm);
	std::string email(void) const;
private:
	bool MakeNonBlocking();
private:
	void * curl_;
	int sockfd_;
	bool inbound_;
	unsigned int lastalive_;
	std::string rxbuf_;
	std::string email_;
	BMEventCB m_cb;
	void *    m_cbp;
};

