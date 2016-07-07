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

void RxThread::NotifyNewMessage(const QString &from, const QStringList & recip, const QString &msg, const QString & certid, const QString &cert)
{
    emit gotMessage(from, recip, msg, certid, cert);
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
    RxThread * self = (RxThread *)p;
    QString qsFrom = QString::fromStdString(from);
    qDebug() << recip;
    QString qsRecip = QString::fromStdString(recip);
    QStringList qslRecip = qsRecip.split(RECIP_SEPARATOR);
    QString qsMsg  = QByteArray::fromBase64(QByteArray(msg));
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QByteArray::fromBase64(QByteArray(cert));

    self->NotifyNewMessage(qsFrom, qslRecip, qsMsg, qsCertID, qsCert);
    return bmOk;
}

int RxProgressHandler(RTxState state, const char *info, void *userptr)
{
    (void)state;
    RxThread * self = (RxThread *)userptr;
    self->NotifyProgress(QString::fromLatin1(info));
    return bmOk;
}


