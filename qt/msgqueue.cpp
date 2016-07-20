#include "msgqueue.h"
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

BMMessage::BMMessage()
{

}

BMMessage::~BMMessage()
{

}

bool BMMessage::Load(const QString & qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toLatin1());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("fTx")
            && jsonMsg.contains("msgType")
            && jsonMsg.contains("from")
            && jsonMsg.contains("group")
            && jsonMsg.contains("recip")
            && jsonMsg.contains("content")
            && jsonMsg.contains("certId")
            && jsonMsg.contains("cert")){

    }else{
        return false;
    }
    _fTx = jsonMsg["fTx"].toBool();
    _MsgType = (MsgType)jsonMsg["msgType"].toInt();
    _From = jsonMsg["from"].toString();
    _Recip = jsonMsg["recip"].toString().split(";");
    _GroupName = jsonMsg["group"].toString();
    _Content = jsonMsg["content"].toString();
    _CertId = jsonMsg["certId"].toString();
    _Cert = jsonMsg["cert"].toString();
    return true;
}

QString BMMessage::Serialize()
{
    QJsonObject jsonMsg;
    jsonMsg["fTx"] = _fTx;
    jsonMsg["msgType"] = (int)_MsgType;
    jsonMsg["from"] = _From;
    jsonMsg["group"] = _GroupName;
    jsonMsg["recip"] = _Recip.join(";");
    jsonMsg["content"] = _Content;
    jsonMsg["certId"] = _CertId;
    jsonMsg["cert"] = _Cert;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
}

bool BMMessage::isTx() const
{
    return _fTx;
}

bool BMMessage::isRx() const
{
    return !_fTx;
}

enum MsgType BMMessage::msgType() const
{
    return _MsgType;
}

bool BMMessage::isPeerMsg() const
{
    return _MsgType == mt_peer;
}

bool BMMessage::isGroupMsg()const
{
    return _MsgType == mt_group;
}

bool BMMessage::isSubscribeMsg() const
{
    return _MsgType == mt_subscribe;
}

QString BMMessage::from() const
{
    return _From;
}

QString BMMessage::groupName() const
{
    return _GroupName;
}

QStringList BMMessage::recip() const
{
    return _Recip;
}

QString BMMessage::content() const
{
    return _Content;
}

QString BMMessage::certid() const
{
    return _CertId;
}

QString BMMessage::cert() const
{
    return _Cert;
}

// Setters
void BMMessage::rtx(bool fTx)
{
    _fTx = fTx;
}

void BMMessage::msgType(MsgType mt)
{
    _MsgType = mt;
}

void BMMessage::from(const QString & f)
{
    _From = f;
}

void BMMessage::groupName(const QString & g)
{
    _GroupName = g;
}

void BMMessage::recip(const QStringList & r)
{
    _Recip = r;
}

void BMMessage::content(const QString & c)
{
    _Content = c;
}

void BMMessage::certid(const QString & cid)
{
    _CertId = cid;
}

void BMMessage::cert(const QString & c)
{
    _Cert = c;
}

