#ifndef RXTHREAD_H
#define RXTHREAD_H

#include <QThread>
#include <QObject>
#include "msgqueue.h"

class BitMail;

class RxThread : public QThread
{
    Q_OBJECT
public:
    RxThread(BitMail * bm, unsigned int qsize);
    ~RxThread();
public:
    void run();
    void NotifyInboxPollEvent(int count);
signals:
    void gotMessage(const QString & from
                        , const QString & msg
                        , const QString & cert);
private:
    BitMail * m_bitmail;
    unsigned int m_checkInterval;
    MsgQueue m_rxq;
    QSemaphore m_inboxPoll;
};

#endif // RXTHREAD_H
