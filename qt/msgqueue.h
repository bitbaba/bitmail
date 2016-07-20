#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QQueue>
#include <QStringList>

enum MsgType{
    mt_undef = 0,
    mt_peer,
    mt_group,
    mt_subscribe,
};

Q_DECLARE_METATYPE(MsgType)

class BMMessage{
private:
    bool         _fTx;                /*true=tx, false=rx*/
    enum MsgType _MsgType;            /*{peer, group, subscribe}*/
    QString      _From;               /*email*/
    QString      _GroupName;          /*additional attribute for <group> type*/
    QStringList  _Recip;              /*recip list of email*/
    QString      _Content;            /*real message content*/
    QString      _CertId;             /*identifier of singer's certificate*/
    QString      _Cert;               /*signer's certificate*/
public:
    BMMessage();
    ~BMMessage();
    bool Load(const QString & jsonMsg);
    QString Serialize();
public:
    // Getters
    bool isTx() const;
    bool isRx() const;
    enum MsgType msgType() const;
    bool isPeerMsg() const;
    bool isGroupMsg()const;
    bool isSubscribeMsg() const;
    QString from() const;
    QString groupName() const;
    QStringList recip() const;
    QString content() const;
    QString certid() const;
    QString cert() const;
    // Setters
    void rtx(bool fTx);
    void msgType(MsgType mt);
    void from(const QString & s);
    void groupName(const QString & g);
    void recip(const QStringList & r);
    void content(const QString & c);
    void certid(const QString & cid);
    void cert(const QString & c);
};

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
