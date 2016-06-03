#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>

static
int MessageEventHandler(const char * from, const char * msg, const char * cert, void * p);

RxThread::RxThread(BitMail * bm)
    : m_bitmail(bm)
    , m_checkInterval(6*1000)
    , m_rxq(1000)
    , m_inboxPoll(0)
{

}

RxThread::~RxThread()
{

}

void RxThread::run()
{
    m_bitmail->OnMessageEvent(MessageEventHandler, this);

    while(true){
        //TODO: exit loop elegant

        // signal or timeout
        m_inboxPoll.tryAcquire(1, m_checkInterval);

        // check inbox
        m_bitmail->CheckInbox();
    }
}

void RxThread::onInboxPollEvent()
{
    m_inboxPoll.release();
}

int MessageEventHandler(const char * from, const char * msg, const char * cert, void * p)
{
    (void)from;    (void)msg;    (void)cert;    (void)p;
    RxThread * self = (RxThread *)p;
    qDebug() << QString::fromStdString(from);
    qDebug() << QByteArray::fromBase64(QByteArray(msg));
    qDebug() << QByteArray::fromBase64(QByteArray(cert));


    return 0;
}


