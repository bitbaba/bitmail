#ifndef TXTHREAD_H
#define TXTHREAD_H

#include <QThread>
#include <QObject>
#include "msgqueue.h"

class BitMail;

class TxThread : public QThread
{
    Q_OBJECT
public:
    TxThread(BitMail * bm);
    ~TxThread();
public:
    void run();
    void stop();
    void NotifyTxProgress(const QString & info);

signals:

    void done();

    void txProgress(const QString & info);

public slots:
    void onSendMessage(MsgType mt
                       , const QString & from
                       , const QString & group
                       , const QStringList & recip
                       , const QString & content);

private:
    BitMail * m_bitmail;
    MsgQueue<BMMessage> m_txq;
    bool m_fStopFlag;
};

#endif // TXTHREAD_H
