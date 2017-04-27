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
    bmefMsgUndef = 0,
    bmefMessage  = 1,
};

enum BMEventSource{
    bmesUndef = 0,
    bmesEmail = 1,
};

struct BMEventHead{
    unsigned int magic;
    BMEventFlag  bmef;
};

struct BMEventMessage{
    BMEventHead    h;
    std::string    msg;
};

typedef
bool (* BMEventCB)(BMEventHead * h, void * userp);

/**
 ******************************************************************************
 * End of Internal definitions
 ******************************************************************************
 */
