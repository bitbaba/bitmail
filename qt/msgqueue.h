#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QQueue>

class BitMailMessage{
private:
    QString m_from;
    QString m_to;
    QString m_msg;
    QString m_cert;
public:
    BitMailMessage();
    BitMailMessage(const QString & f, const QString & t, const QString & m, const QString & c);
    ~BitMailMessage();
public:
    QString from() const;
    QString to() const;
    QString msg() const;
    QString cert() const;
    void setFrom(const QString & f);
    void setTo(const QString & t);
    void setMsg(const QString & m);
    void setCert(const QString & c);
};

class MsgQueue
{
public:
    MsgQueue(unsigned int capcity);

    ~MsgQueue();

    bool readable(int timeout);

    bool writable(int timeout);

    void push(const BitMailMessage & msg);

    BitMailMessage pop();

private:
    QMutex m_mutex;
    QSemaphore m_readable;
    QSemaphore m_writable;
    QQueue<BitMailMessage> m_q;
};

#endif // MSGQUEUE_H
