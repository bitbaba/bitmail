#include <bitmailcore/bitmail_api.h>

#include <bitmailcore/bitmail.h>

#include <string.h>
#include <string>
#include <iostream>
#include <stdlib.h>

void * BM_Init()
{
	return new BitMail();
}

int BM_CreateProfile(void * ctx, const char * email, const char * nick, const char * passphrase)
{
	if (!ctx) {
		return 1;
	}
	if (!email || !*email || strchr(email, '@') == NULL){
		return 2;
	}
	if (!passphrase || !*passphrase){
		return 3;
	}
	if (!nick || !*nick){
		return 4;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->CreateProfile(nick, email, passphrase, 1024);
	return 0;
}

int BM_LoadProfile(void * ctx, const char * keypem, const char * certpem, const char * passphrase)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->LoadProfile(passphrase, keypem, certpem);
	return 0;
}

int BM_SaveProfile(void * ctx, const char * passphrase, char ** keypem, char ** certpem)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	std::string sKeyPem, sCertPem;
	bm->SaveProfile(passphrase, sKeyPem, sCertPem);
	*keypem = strdup(sKeyPem.c_str());
	*certpem = strdup(sCertPem.c_str());
	return 0;
}

int BM_Free(char * str){
	if (!str){
		return 1;
	}
	free(str);
	str = NULL;
	return 0;
}

int BM_AddBuddy(void * ctx, const char * cert)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->AddBuddy(cert);
	return 0;
}

int BM_RemoveBuddy(void * ctx, const char * email)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->RemoveBuddy(email);
	return 0;
}

int BM_NetConfig(void * ctx, const char * txurl, const char * rxurl, const char * u, const char * p, MsgCB onmsg, void * onmsgp, PollCB onpoll, void * onpollp)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->InitNetwork(txurl, u, p, rxurl, u, p);
	bm->OnPollEvent(onpoll, onmsgp);
	bm->OnMessageEvent(onmsg, onpollp);
	return 0;
}

int BM_SendMessage(void * ctx, const char * email, const char * msg)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->SendMsg(email, msg);
	return 0;
}

int BM_GroupMessage(void * ctx, int bcnt, const char * emails [], const char * msg)
{
	if (!ctx){
		return 1;
	}
	std::vector<std::string> vecTos;
	for (int i = 0; i < bcnt; i++){
		vecTos.push_back(emails[i]);
	}
	if (vecTos.size() == 0){
		return 2;
	}
	BitMail * bm = (BitMail*)ctx;
	bm->GroupMsg(vecTos, msg);
	return 0;
}

int BM_CheckInbox(void * ctx)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->CheckInbox();
	return 0;
}

int BM_StartIdle(void * ctx, unsigned int timeout)
{
	if (!ctx){
		return 1;
	}
	BitMail * bm = (BitMail *)ctx;
	bm->StartIdle(timeout);
	return 0;
}

void BM_Close(void * ctx)
{
	if (ctx != NULL){
		delete (BitMail*)ctx;
		ctx = NULL;
	}
}

