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
    void NotifyTxProgress(int st, const QString & info);

signals:

    void done();

    void txProgress(int st, const QString & info);

public slots:
    void onSendMessage(const QString & from
                       , const QStringList & to
                       , const QString & content
                       , bool signOnly);

private:
    BitMail * m_bitmail;
    MsgQueue<QString> m_txq;
    bool m_fStopFlag;
};

#endif // TXTHREAD_H
