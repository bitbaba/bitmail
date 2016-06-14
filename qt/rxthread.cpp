#include "rxthread.h"
#include <QSemaphore>
#include <bitmailcore/bitmail.h>
#include <QDebug>

static
int MessageEventHandler(const char * from, const char * msg, const char * certid, const char * cert, void * p);

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
        m_bitmail->CheckInbox();
    }

    m_bitmail->Expunge();

    emit done();

    qDebug() << "Rx Thread quit";
}

void RxThread::stop()
{
    m_fStopFlag = true;
}

void RxThread::NotifyNewMessage(const QString &from, const QString &msg, const QString & certid, const QString &cert)
{
    emit gotMessage(from, msg, certid, cert);
    return ;
}

void RxThread::onInboxPollEvent()
{
    m_inboxPoll.release();
}

int MessageEventHandler(const char * from, const char * msg, const char * certid, const char * cert, void * p)
{
    (void)from;    (void)msg;   (void)certid;  (void)cert;    (void)p;
    RxThread * self = (RxThread *)p;
    (void) self;

    QString qsFrom = QString::fromStdString(from);
    QString qsMsg  = QByteArray::fromBase64(QByteArray(msg));
    QString qsCertID = QString::fromStdString(certid);
    QString qsCert = QByteArray::fromBase64(QByteArray(cert));

    self->NotifyNewMessage(qsFrom, qsMsg, qsCertID, qsCert);
    return 0;
}


