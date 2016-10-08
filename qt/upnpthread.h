#ifndef UPNPTHREAD_H
#define UPNPTHREAD_H

#include <QObject>
#include <QThread>

class BitMail;

class UpnpThread : public QThread
{
    Q_OBJECT
public:
    UpnpThread(BitMail * bitmail);
    ~UpnpThread();
public:
    void run();

signals:
    void done(bool fOk, const QString & extUrl);

private:
    BitMail * m_bitmail;
};

#endif // UPNPTHREAD_H
