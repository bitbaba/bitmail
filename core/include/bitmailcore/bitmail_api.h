#ifndef BMAPI_H
#define BMAPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (* OnBmMessage)(void * ctx, const char * from, const char * msg, void * userp);

void * BM_Init();

int BM_CreateProfile(void * ctx, const char * email, const char * passphrase);

int BM_LoadProfile(void * ctx, const char * path, const char * passphrase);

int BM_SaveProfile(void * ctx, const char * path, const char * passphrase);

int BM_AddBuddy(void * ctx, const char * cert);

int BM_RemoveBuddy(void * ctx, const char * email);

int BM_TxConfig(void * ctx, const char * url, const char * u, const char * p);

int BM_SendMessage(void * ctx, const char * email, const char * msg);

int BM_GroupMessage(void * ctx, int bcnt, const char * emails [], const char * msg);

int BM_RxConfig(void * ctx, const char * url, const char * u, const char * p);

int BM_CheckInbox(void * ctx);

int BM_StartIdle(void * ctx);

void BM_Close(void * ctx);


#ifdef __cplusplus
};
#endif

#endif // BMAPI_H
