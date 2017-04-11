#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>
#include <QDateTime>

static
int MessageEventHandler(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, void * p);

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
    qint64 lastCheck = QDateTime::currentMSecsSinceEpoch();

    m_bitmail->OnMessageEvent(MessageEventHandler, this);

    while(!m_fStopFlag){
        qDebug() << "RxThread: Waiting for inbox poll event";
        m_inboxPoll.tryAcquire(1, m_checkInterval);
        qDebug() << "RxThread: Timeout of inbox poll event";

        qint64 now = QDateTime::currentMSecsSinceEpoch();

        if (lastCheck + m_checkInterval < now){
            qDebug() << "RxThread: check inbox directly!";
            m_bitmail->CheckInbox(RxProgressHandler, this);
            lastCheck = now;
        }        
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
                                , const QString &cert)
{
    emit gotMessage(from, receips, content, certid, cert);
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

int MessageEventHandler(const char * from, const char * receips, const char * msg, unsigned int msglen, const char * certid, const char * cert, void * p)
{
    QString qsFrom = QString::fromStdString(from);
    QString qsReceips = QString::fromStdString(receips);
    QString qsContent = QString::fromUtf8(msg, msglen);
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QString::fromStdString((cert));

    RxThread * self = (RxThread *)p;
    self->NotifyNewMessage(qsFrom, qsReceips, qsContent, qsCertID, qsCert);
    return bmOk;
}

int RxProgressHandler(RTxState state, const char *info, void *userptr)
{
    (void)state;
    RxThread * self = (RxThread *)userptr;
    self->NotifyProgress(QString::fromLatin1(info));
    return bmOk;
}


