#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>

static
int MessageEventHandler(const char * from, const char * recip, const char * msg, const char * certid, const char * cert, void * p);

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

void RxThread::NotifyNewMessage(enum MsgType mt
                                , const QString &from
                                , const QString & group
                                , const QStringList & recip
                                , const QString & content
                                , const QString & certid
                                , const QString &cert)
{
    emit gotMessage(mt, from, group, recip, content, certid, cert);
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

int MessageEventHandler(const char * from, const char * recip, const char * msg, const char * certid, const char * cert, void * p)
{
    BMMessage rxMsg;
    if (!rxMsg.Load(QString::fromStdString((msg)))){
        return bmInvalidParam;
    }
    QString qsGroup = rxMsg.groupName();
    MsgType mt = rxMsg.msgType();
    QString qsContent = rxMsg.content();

    QString qsFrom = QString::fromStdString(from);
    QStringList qslRecip = QString::fromStdString(recip).split(RECIP_SEPARATOR, QString::SkipEmptyParts);
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QString::fromStdString((cert));

    RxThread * self = (RxThread *)p;
    self->NotifyNewMessage(mt, qsFrom, qsGroup, qslRecip, qsContent, qsCertID, qsCert);
    return bmOk;
}

int RxProgressHandler(RTxState state, const char *info, void *userptr)
{
    (void)state;
    RxThread * self = (RxThread *)userptr;
    self->NotifyProgress(QString::fromLatin1(info));
    return bmOk;
}


