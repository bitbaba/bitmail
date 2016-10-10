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
signals:
    void signalBracThDone();
private:
    bool m_fStop;
    BitMail * m_bitmail;
    QMutex m_mutex;
    std::vector<Brac *> m_bracs;
    MsgQueue<RTXMessage> m_txq;
};

#endif // BRACTHREAD_H
