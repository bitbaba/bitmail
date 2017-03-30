#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QQueue>
#include <QStringList>
#include <QJsonObject>

template<class nodeType>
class MsgQueue
{
public:
    //declearations and implementations of template class must be in a signle file together.
    MsgQueue(unsigned int capcity)
        : m_readable(0)
        , m_writable(capcity)
    {

    }
    ~MsgQueue()
    {

    }

    bool readable(int timeout)
    {
        return m_readable.tryAcquire(1, timeout);
    }

    bool writable(int timeout)
    {
        return m_writable.tryAcquire(1, timeout);
    }

    void push(const nodeType &msg)
    {
        QMutexLocker lock(&m_mutex);
        m_q.enqueue(msg);
        lock.unlock();
        m_readable.release();
        return ;
    }

    nodeType pop()
    {
        QMutexLocker lock(&m_mutex);
        nodeType msg = m_q.dequeue();
        lock.unlock();
        m_writable.release();
        return msg;
    }

private:
    QMutex m_mutex;
    QSemaphore m_readable;
    QSemaphore m_writable;
    QQueue<nodeType> m_q;
};

#endif // MSGQUEUE_H
