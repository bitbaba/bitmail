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
    void NotifyProgress(const QString & info);
    void stop();
signals:
    void inboxPollEvent();
    void done();
    void inboxPollProgress(const QString & info);
private:
    BitMail * m_bitmail;
    unsigned int m_lastCount;
    unsigned int m_reIdleInterval;
    bool m_fStopFlag;
};

#endif // POLLTHREAD_H
