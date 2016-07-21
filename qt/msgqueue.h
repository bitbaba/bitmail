#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <QObject>
#include <QMutex>
#include <QSemaphore>
#include <QQueue>
#include <QStringList>


class RTXMessage{
private:
    bool         _fTx;                /*true=tx, false=rx*/
    QString      _From;               /*email*/
    QStringList  _Recip;              /*recip list of email*/
    QString      _CertId;             /*identifier of singer's certificate*/
    QString      _Cert;               /*signer's certificate*/
    QString      _Content;            /*real message content*/

public:
    RTXMessage();
    ~RTXMessage();
    bool Load(const QString & jsonMsg);
    QString Serialize() const;
public:
    // Getters
    bool isTx() const;
    bool isRx() const;
    QString from() const;

    QStringList recip() const;
    QString content() const;
    QString certid() const;
    QString cert() const;
    // Setters
    void rtx(bool fTx);
    void from(const QString & s);
    void recip(const QStringList & r);
    void content(const QString & c);
    void certid(const QString & cid);
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
    QString Serialize();
public:
    enum MsgType msgType() const;
    bool isPeerMsg() const;
    bool isGroupMsg()const;
    bool isSubMsg() const;
    void msgType(MsgType mt);
    QString content() const;
    void content(const QString & ctnt);
private:
    MsgType _MsgType;
    QString _Content;
};

class PeerMessage{
public:
    PeerMessage();
    ~PeerMessage();
public:
    bool Load(const QString & qsJsonMsg);
    QString Serialize()const;
public:
    QString content() const;
    void content(const QString & ctnt);
private:
    QString _Content;
};

class GroupMessage{
public:
    GroupMessage();
    ~GroupMessage();
public:
    bool Load(const QString & qsJsonMsg);
    QString Serialize()const;
public:
    QString groupId() const;
    void groupId(const QString & gid);
    QString groupName()const;
    void groupName(const QString & gname);
    QString content() const;
    void content(const QString & ctnt);
private:
    QString _GroupId;
    QString _GroupName;
    QString _Content;
};

class SubMessage{
public:
    SubMessage();
    ~SubMessage();
public:
    bool Load(const QString & qsJsonMsg);
    QString Serialize()const;
public:
    QString refer()const;
    void refer(const QString & ref);
    QString content() const;
    void content(const QString & ctnt);
private:
    QString _Refer;
    QString _Content;
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
