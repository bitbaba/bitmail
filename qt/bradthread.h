#ifndef BRADTHREAD_H
#define BRADTHREAD_H

#include <QObject>
#include <QThread>

class BitMail;

class BradThread : public QThread
{
    Q_OBJECT
public:
    BradThread(BitMail * bitmail);
public:
    void run();
    void stop();
signals:
    void signalBradThDone();
private:
    BitMail * m_bitmail;
    bool m_fStopFlag;
};

#endif // BRADTHREAD_H
