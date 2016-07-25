#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#define CURL_STATICLIB
#include <curl/curl.h>

#include <bitmailcore/x509cert.h>
#include <bitmailcore/bitmail.h>

#include <json/json.h>

#if defined(__GNUC__)
    #include <unistd.h>
#endif

#if defined(WIN32)
#define SLEEP(x) ::Sleep((x) * 1000)
#else
#define SLEEP(x) sleep((x))
#endif

enum MsgType{
    mt_undef = 0,
    mt_peer,
    mt_group,
    mt_subscribe,
};


class BMMessage{
public:
    BMMessage();
    ~BMMessage();
public:
    bool Load(const std::string &jsonMsg);
    std::string Serialize();
public:
    enum MsgType msgType() const;
    bool isPeerMsg() const;
    bool isGroupMsg()const;
    bool isSubMsg() const;
    void msgType(MsgType mt);
    std::string content() const;
    void content(const std::string & ctnt);
private:
    MsgType _MsgType;
    std::string _Content;
};

class PeerMessage{
public:
    PeerMessage();
    ~PeerMessage();
public:
    bool Load(const std::string & sJsonMsg);
    std::string Serialize()const;
public:
    std::string content() const;
    void content(const std::string & ctnt);
private:
    std::string _Content;
};

using namespace std;

// for simple daemon,
// save passphrase, profile, cert etc, in head file "setting.h"
// like:
// const char * passphrase = ".......";
// const char * profile    = "-----BEGIN CERT....";
// const char * privkey    = "-----BEGIN PRIV....";
//
#include "setting.h"

static
int onNewMessage(const char * from
		, const char * recip
		, const char * msg
		, const char * certid
		, const char * cert
		, void * p);

static
int RxProgressHandler(RTxState state, const char *info, void *userptr);

int main(int argc, char **argv)
{
   BitMail mail;
   if (bmOk != mail.LoadProfile(passphrase, key, profile)){
	   perror("Failed to load profile\n");
	   return -1;
   }

   mail.OnMessageEvent(onNewMessage, &mail);

   if (bmOk != mail.InitNetwork(txurl, txlogin, mail.Decrypt(txpassword)
		   , rxurl, rxlogin, mail.Decrypt(rxpassword))){
	   perror("Failed to setup network\n");
	   return -2;
   }

   while(true){
	   printf("Waiting for new message\n");
	   mail.CheckInbox(NULL, &mail);
	   SLEEP(5);
   }

   exit(0);
}

static
int writer(char *data, size_t size, size_t nmemb,
                  std::string *writerData)
{
  if (writerData == NULL)
    return 0;

  writerData->append(data, size*nmemb);

  return size * nmemb;
}


std::string HandleSearch(const std::string & url, const std::string & param)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
      /* First set the URL that is about to receive our POST. This URL can
         just as well be a https:// URL if that is what should receive the
         data. */
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

      /* we use a self-signed test server, skip verification during debugging */
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

      /* Now specify the POST data */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param.c_str());

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK){
        fprintf(stderr
                , "curl_easy_perform() failed: %s\n"
                , curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return response;
}

int onNewMessage(const char * from
		, const char * recip
		, const char * msg
		, const char * certid
		, const char * cert
		, void * p)
{
	printf("From: %s\n", from);
	printf("Recip: %s\n", recip);
	printf("Msg: %s\n", msg);
	printf("certid: %s\n", certid);
	printf("cert: %s\n", cert);

	/**
	 * Op1: trust peer
	 * Orgnization bitmail account always `trust' peers
	 */
	BitMail * self = (BitMail *)p;
	self->AddFriend(from, cert);

	BMMessage bmMsg;
	bmMsg.Load(msg);
	if (bmMsg.msgType() != mt_peer){
		return bmInvalidParam;
	}
	PeerMessage peerMsg;
	peerMsg.Load(bmMsg.content());

	printf("PeerMsg::_Content: %s\n", peerMsg.content().c_str());

	const std::string query = peerMsg.content();
	char * tmp = curl_escape(query.c_str(), query.length());
	std::string urlEncodedQuery = tmp;
	curl_free(tmp);
	std::string url = "http://127.0.0.1:10086/search";

	Json::Value jsonParam;
	if (false){
		jsonParam["query"] = urlEncodedQuery;
	}else{
		url += "?";
		url += urlEncodedQuery;
	}
	std::string resp = HandleSearch(url, jsonParam.toStyledString());
	std::cout<<"Response: " << resp.substr(0, 32) << "..." << std::endl;

	std::vector<std::string> vecRecip;
	vecRecip.push_back(from);
	peerMsg.content(resp);
	bmMsg.content(peerMsg.Serialize());
	/**
	 * Op2: send response
	 * And here use the `trusted' cert to encrypt.
	 *
	 */
	self->SendMsg(vecRecip, bmMsg.Serialize(), RxProgressHandler, self);
	return bmOk;

	/**
	 * Note: Op1 and Op2 should be processed as a ATOMIC task.
	 * case: Alice@somenet.net send a email, with a smtp header: `From: Bob@othernet.net'
	 * in most cases, SMTP server will not allow Alice to do like above.
	 */
}


static
int RxProgressHandler(RTxState state, const char *info, void *userptr)
{
	printf("RTxState: %d\n", state);
	printf("info: %s\n", info);
	BitMail * self = (BitMail *)userptr;
	(void)userptr;
    return bmOk;
}


BMMessage::BMMessage()
{

}
BMMessage::~BMMessage()
{

}

bool BMMessage::Load(const std::string & sjsonmsg)
{
    Json::Reader reader;
    Json::Value jsonMsg;
    reader.parse(sjsonmsg, jsonMsg);
    _MsgType = (MsgType)jsonMsg["msgType"].asInt();
    _Content = jsonMsg["content"].asString();
    return true;
}

std::string BMMessage::Serialize()
{
	Json::Value jsonMsg;
    jsonMsg["msgType"] = (int)_MsgType;
    jsonMsg["content"] = _Content;
    return jsonMsg.toStyledString();
}

enum MsgType BMMessage::msgType() const
{
    return _MsgType;
}
bool BMMessage::isPeerMsg() const
{
    return _MsgType == mt_peer;
}
bool BMMessage::isGroupMsg()const
{
    return _MsgType == mt_group;
}
bool BMMessage::isSubMsg() const
{
    return _MsgType == mt_subscribe;
}
void BMMessage::msgType(MsgType mt)
{
    _MsgType = mt;
}

std::string BMMessage::content() const
{
    return _Content;
}
void BMMessage::content(const std::string & ctnt)
{
    _Content = ctnt;
}


PeerMessage::PeerMessage()
{

}

PeerMessage::~PeerMessage()
{

}

bool PeerMessage::Load(const std::string & sjsonmsg)
{
	Json::Reader reader;
	Json::Value jsonMsg;
	reader.parse(sjsonmsg, jsonMsg);
    _Content = jsonMsg["content"].asString();
    return true;
}

std::string PeerMessage::Serialize()const
{
	Json::Value jsonMsg;
	jsonMsg["content"] = _Content;
    return jsonMsg.toStyledString();
}

std::string PeerMessage::content() const
{
    return _Content;
}
void PeerMessage::content(const std::string & ctnt)
{
    _Content = ctnt;
}
