#ifndef THREADGETMESSAGE_H
#define THREADGETMESSAGE_H

#include <QThread>
#include <QObject>

#include <bitmailcore/bitmail.h>

class ThreadGetMessage : public QThread
{
    Q_OBJECT;
public:
    ThreadGetMessage(BitMail * instPtr, unsigned int checkInterval);
    ~ThreadGetMessage();
    void run();
    void SetInterval(unsigned int interval);
    static int GetMessage_Callback(const std::string & jsonParam, void * userdata);
signals:
    void MessageGot(QString );
    void StrangerRequest(QString );
private:
    BitMail * m_refInst;
    unsigned int m_checkInterval;
};

#endif // THREADGETMESSAGE_H
