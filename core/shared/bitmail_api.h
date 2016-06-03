#ifndef BMAPI_H
#define BMAPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (* MsgCB)(const char * from, const char * msg, const char * cert, void * userp);

typedef int (* PollCB)(unsigned int mcnt, void * userp);

void * BM_Init();

int BM_CreateProfile(void * ctx, const char * email, const char * nick, const char * passphrase);

int BM_LoadProfile(void * ctx, const char * keypem, const char * certpem, const char * passphrase);

int BM_SaveProfile(void * ctx, const char * passphrase, char ** keypem, char ** certpem);

int BM_Free(char * str);

int BM_AddBuddy(void * ctx, const char * cert);

int BM_RemoveBuddy(void * ctx, const char * email);

int BM_NetConfig(void * ctx, const char * txurl, const char * rxurl, const char * u, const char * p, MsgCB onmsg, void * onmsgp, PollCB onpoll, void * userp);

int BM_SendMessage(void * ctx, const char * email, const char * msg);

int BM_GroupMessage(void * ctx, int bcnt, const char * emails [], const char * msg);

int BM_CheckInbox(void * ctx);

int BM_StartIdle(void * ctx, unsigned int timeout);

void BM_Close(void * ctx);


#ifdef __cplusplus
};
#endif

#endif // BMAPI_H
