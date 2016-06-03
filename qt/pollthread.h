#ifndef POLLTHREAD_H
#define POLLTHREAD_H

#include <QThread>

#include <QObject>

class BitMail;

class PollThread : public QThread
{
    Q_OBJECT
public:
    PollThread(BitMail * bm);
    ~PollThread();
    void run();
    void NotifyInboxPollEvent(unsigned int count);
signals:
    void inboxPollEvent();
private:
    BitMail * m_bitmail;
    unsigned int m_lastCount;
    unsigned int m_reIdleInterval;
};

#endif // POLLTHREAD_H
