#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QQueue>
#include <QStringList>
#include <QJsonObject>

class RTXMessage {
private:
    QJsonObject  _Root;

public:
    RTXMessage();
    ~RTXMessage();
    bool Load(const QString & jsonMsg);
    QString Serialize() const;
    operator QJsonObject() const;
public:
    bool isTx() const;
    bool isRx() const;
    void rtx(bool fTx);

    QString from() const;    
    void from(const QString & s);

    QStringList recip() const;
    void recip(const QStringList & r);

    QJsonObject content() const;
    void content(const QJsonObject & c);

    QString certid() const;
    void certid(const QString & cid);

    QString cert() const;
    void cert(const QString & c);
};

enum MsgType{
    mt_undef = 0,
    mt_peer,
    mt_group,
    mt_subscribe,
};

Q_DECLARE_METATYPE(MsgType)

class BMMessage{
public:
    BMMessage();
    ~BMMessage();
public:
    bool Load(const QString &jsonMsg);
    bool Load(const QJsonObject & root);
    QString Serialize();
    operator QJsonObject() const;
public:
    enum MsgType msgType() const;
    bool isPeerMsg() const;
    bool isGroupMsg()const;
    bool isSubMsg() const;
    void msgType(MsgType mt);

    QJsonObject content() const;
    void content(const QJsonObject & ctnt);

    QString bradExtUrl() const;
    void bradExtUrl(const QString & extUrl);
private:
    QJsonObject _Root;
};

class PeerMessage{
public:
    PeerMessage();
    ~PeerMessage();
public:
    bool Load(const QString & qsJsonMsg);
    bool Load(const QJsonObject & root);
    QString Serialize()const;
    operator QJsonObject() const;
public:
    QString content() const;
    void content(const QString & ctnt);
private:
    QJsonObject _Root;
};

class GroupMessage{
public:
    GroupMessage();
    ~GroupMessage();

public:
    bool Load(const QString & qsJsonMsg);
    bool Load(const QJsonObject & root);
    QString Serialize()const;
    operator QJsonObject() const;
public:
    QString groupId() const;
    void groupId(const QString & gid);
    QString groupName()const;
    void groupName(const QString & gname);
    QString content() const;
    void content(const QString & ctnt);
private:
    QJsonObject _Root;
};

class SubMessage{
public:
    SubMessage();
    ~SubMessage();

public:
    bool Load(const QString & qsJsonMsg);
    bool Load(const QJsonObject & root);
    QString Serialize()const;
    operator QJsonObject() const;
public:
    QString refer()const;
    void refer(const QString & ref);
    QString content() const;
    void content(const QString & ctnt);
private:
    QJsonObject _Root;
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
