/**
 ******************************************************************************
 * Following definitions should be in a internal Header for shared reference
 ******************************************************************************
 */
#include <string>
#include <vector>
#include <set>
#include <map>

typedef unsigned int MessageNo;

#define BMMAGIC (0xbeefbeef)

enum BMEventFlag{
    bmefSystem       =     0,
    bmefMsgCount     =     1,
    bmefMessage      =     2,
};

struct BMEventHead{
    unsigned int magic;
    BMEventFlag bmef;
};

struct BMEventSystem{
    BMEventHead h;
    unsigned int reserved;
};

struct BMEventMsgCount{
    BMEventHead h;
    unsigned int msgcount;
};

struct BMEventMessage{
    BMEventHead h;
    std::string msg;
};

typedef int (* BMEventCB)(BMEventHead * h, void * userp);

/**
 ******************************************************************************
 * End of Internal definitions
 ******************************************************************************
 */
