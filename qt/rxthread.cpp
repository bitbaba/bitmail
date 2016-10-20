#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>

static
int MessageEventHandler(const char * from, const char * msg, unsigned int msglen, const char * certid, const char * cert, void * p);

static
int RxProgressHandler(RTxState state, const char * info, void * userptr);

RxThread::RxThread(BitMail * bm)
    : m_bitmail(bm)
    , m_checkInterval(6*1000)
    , m_inboxPoll(0)
    , m_fStopFlag(false)
{

}

RxThread::~RxThread()
{

}

void RxThread::run()
{
    m_bitmail->OnMessageEvent(MessageEventHandler, this);

    while(!m_fStopFlag){
        // signal or timeout
        m_inboxPoll.tryAcquire(1, m_checkInterval);

        // check brac events
        m_bitmail->PollBracs(1000);

        // check inbox
        m_bitmail->CheckInbox(RxProgressHandler, this);
    }

    m_bitmail->Expunge();

    emit done();
}

void RxThread::stop()
{
    m_fStopFlag = true;
}

void RxThread::NotifyNewMessage(const QString &from
                                , const QString & content
                                , const QString & certid
                                , const QString &cert)
{
    emit gotMessage(from, content, certid, cert);
    return ;
}

void RxThread::NotifyProgress(const QString & info)
{
    emit rxProgress(info);
    return ;
}

void RxThread::onInboxPollEvent()
{
    m_inboxPoll.release();
}

int MessageEventHandler(const char * from, const char * msg, unsigned int msglen, const char * certid, const char * cert, void * p)
{
    QString qsFrom = QString::fromStdString(from);
    QString qsContent = QString::fromUtf8(msg, msglen);
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QString::fromStdString((cert));

    RxThread * self = (RxThread *)p;
    self->NotifyNewMessage(qsFrom, qsContent, qsCertID, qsCert);
    return bmOk;
}

int RxProgressHandler(RTxState state, const char *info, void *userptr)
{
    (void)state;
    RxThread * self = (RxThread *)userptr;
    self->NotifyProgress(QString::fromLatin1(info));
    return bmOk;
}


