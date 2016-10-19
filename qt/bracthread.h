#ifndef BRACTHREAD_H
#define BRACTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "msgqueue.h"

class Brac;
class BitMail;

class BracThread : public QThread
{
    Q_OBJECT
public:
    BracThread(BitMail * bitmail);
public:
    void run();
    void stop();
    void Add(Brac * brac);
    void ByMx(const RTXMessage & rtxMsg);
    Brac * GetBrac(const std::vector<Brac *> & bracs, const QString & to);
signals:
    void signalBracThDone();

    void signalByMx(  const QString & from
                     , const QStringList & recip
                     , const QString & msg);
private:
    bool m_fStop;
    BitMail * m_bitmail;
    QMutex m_mutex;
    std::vector<Brac *> m_bracs;
    MsgQueue<RTXMessage> m_txq;
};

#endif // BRACTHREAD_H
