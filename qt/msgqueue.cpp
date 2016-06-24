#include "msgqueue.h"
#include <QMutexLocker>

BitMailMessage::BitMailMessage()
{

}

BitMailMessage::BitMailMessage(const QString &from, const QStringList & vecTo, const QString &m, const QString & c)
    : m_from(from)
    , m_to(vecTo)
    , m_msg(m)
    , m_cert(c)
{

}

BitMailMessage::~BitMailMessage()
{

}

QString BitMailMessage::from() const
{
    return m_from;
}

QStringList BitMailMessage::to() const
{
    return m_to;
}

QString BitMailMessage::msg() const
{
    return m_msg;
}

QString BitMailMessage::cert() const
{
    return m_cert;
}

void BitMailMessage::setFrom(const QString &f)
{
    m_from = f;
}

void BitMailMessage::setTo(const QStringList &t)
{
    m_to = t;
}

void BitMailMessage::setMsg(const QString &m)
{
    m_msg = m;
}

void BitMailMessage::setCert(const QString &c)
{
    m_cert = c;
}

MsgQueue::MsgQueue(unsigned int capcity)
    : m_readable(0)
    , m_writable(capcity)
{

}

MsgQueue::~MsgQueue()
{

}

bool MsgQueue::readable(int timeout)
{
    return m_readable.tryAcquire(1, timeout);
}

bool MsgQueue::writable(int timeout)
{
    return m_writable.tryAcquire(1, timeout);
}

void MsgQueue::push(const BitMailMessage &msg)
{
    QMutexLocker lock(&m_mutex);
    m_q.enqueue(msg);
    lock.unlock();
    m_readable.release();
    return ;
}

BitMailMessage MsgQueue::pop()
{
    QMutexLocker lock(&m_mutex);
    BitMailMessage msg = m_q.dequeue();
    lock.unlock();
    m_writable.release();
    return msg;
}
