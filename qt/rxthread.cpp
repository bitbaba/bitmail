#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>

static
int MessageEventHandler(const char * from, const char * msg, const char * cert, void * p);

static
int PollEventHandler(unsigned int count, void * p);

RxThread::RxThread(BitMail * bm, unsigned int qsize)
    : m_bitmail(bm)
    , m_checkInterval(5*1000)
    , m_rxq(qsize)
    , m_inboxPoll(0)
{

}

RxThread::~RxThread()
{

}

void RxThread::run()
{
    m_bitmail->OnPollEvent(PollEventHandler, this);

    m_bitmail->OnMessageEvent(MessageEventHandler, this);

    while(true){
        m_inboxPoll.acquire(m_checkInterval);
        m_bitmail->CheckInbox();
    }
}

void RxThread::NotifyInboxPollEvent(int count)
{
    (void)count;
    m_inboxPoll.release();
}

int MessageEventHandler(const char * from, const char * msg, const char * cert, void * p)
{
    (void)from;    (void)msg;    (void)cert;    (void)p;
    qDebug() << QString::fromStdString(from);
    return 0;
}

int PollEventHandler(unsigned int count, void * p)
{
    (void)count;    (void)p;
    RxThread * self = (RxThread *)p;
    self->NotifyInboxPollEvent(count);
    return 0;
}
