#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>
#include <QDateTime>

static
int MessageEventHandler(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, const char * sigtime, void * p);

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
    while(!m_fStopFlag){
        qDebug() << "RxThread: Waiting for inbox poll event";
        m_inboxPoll.tryAcquire(1, m_checkInterval);

        qDebug() << "RxThread: checking inbox!";
        m_bitmail->CheckInbox(MessageEventHandler, this, RxProgressHandler, this);
    }

    m_bitmail->Expunge();

    emit done();
}

void RxThread::stop()
{
    m_fStopFlag = true;
}

void RxThread::NotifyNewMessage(const QString &from
                                , const QString & receips
                                , const QString & content
                                , const QString & certid
                                , const QString & cert
                                , const QString & sigtime)
{
    emit gotMessage(from, receips, content, certid, cert, sigtime);
    return ;
}

void RxThread::onInboxPollEvent()
{
    m_inboxPoll.release();
}

int MessageEventHandler(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, const char * sigtime, void * p)
{
    QString qsFrom = QString::fromStdString(from);
    QString qsReceips = QString::fromStdString(receips);
    QString qsContent = QString::fromUtf8(msg, msglen);
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QString::fromStdString((cert));
    QString qsSigTime = QString::fromStdString(sigtime);

    RxThread * self = (RxThread *)p;
    self->NotifyNewMessage(qsFrom, qsReceips, qsContent, qsCertID, qsCert, qsSigTime);
    return bmOk;
}

void RxThread::NotifyProgress(int st, const QString & info)
{
    emit rxProgress(st, info);
    return ;
}

int RxProgressHandler(RTxState st, const char *info, void *userptr)
{
    RxThread * self = (RxThread *)userptr;
    self->NotifyProgress((int)st, QString::fromLatin1(info));
    return bmOk;
}


