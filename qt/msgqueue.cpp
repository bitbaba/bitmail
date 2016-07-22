#include "msgqueue.h"
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

RTXMessage::RTXMessage()
{

}

RTXMessage::~RTXMessage()
{

}

bool RTXMessage::Load(const QString & qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toUtf8());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("fTx")
            && jsonMsg.contains("from")
            && jsonMsg.contains("recip")
            && jsonMsg.contains("content")
            && jsonMsg.contains("certId")
            && jsonMsg.contains("cert")){

    }else{
        return false;
    }
    _fTx = jsonMsg["fTx"].toBool();
    _From = jsonMsg["from"].toString();
    _Recip = jsonMsg["recip"].toString().split(";");
    _CertId = jsonMsg["certId"].toString();
    _Cert = jsonMsg["cert"].toString();
    _Content = jsonMsg["content"].toString();
    return true;
}

QString RTXMessage::Serialize() const
{
    QJsonObject jsonMsg;
    jsonMsg["fTx"] = _fTx;
    jsonMsg["from"] = _From;
    jsonMsg["recip"] = _Recip.join(";");
    jsonMsg["content"] = _Content;
    jsonMsg["certId"] = _CertId;
    jsonMsg["cert"] = _Cert;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
}

bool RTXMessage::isTx() const
{
    return _fTx;
}

bool RTXMessage::isRx() const
{
    return !_fTx;
}

QString RTXMessage::from() const
{
    return _From;
}

QStringList RTXMessage::recip() const
{
    return _Recip;
}

QString RTXMessage::content() const
{
    return _Content;
}

QString RTXMessage::certid() const
{
    return _CertId;
}

QString RTXMessage::cert() const
{
    return _Cert;
}

// Setters
void RTXMessage::rtx(bool fTx)
{
    _fTx = fTx;
}

void RTXMessage::from(const QString & f)
{
    _From = f;
}

void RTXMessage::recip(const QStringList & r)
{
    _Recip = r;
}

void RTXMessage::content(const QString & c)
{
    _Content = c;
}

void RTXMessage::certid(const QString & cid)
{
    _CertId = cid;
}

void RTXMessage::cert(const QString & c)
{
    _Cert = c;
}

BMMessage::BMMessage()
{

}
BMMessage::~BMMessage()
{

}

bool BMMessage::Load(const QString &qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toUtf8());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("msgType")
            && jsonMsg.contains("content")){

    }else{
        return false;
    }
    _MsgType = (MsgType)jsonMsg["msgType"].toInt();
    _Content = jsonMsg["content"].toString();
    return true;
}

QString BMMessage::Serialize()
{
    QJsonObject jsonMsg;
    jsonMsg["msgType"] = (int)_MsgType;
    jsonMsg["content"] = _Content;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
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
bool BMMessage::isSubMsg() const
{
    return _MsgType == mt_subscribe;
}
void BMMessage::msgType(MsgType mt)
{
    _MsgType = mt;
}

QString BMMessage::content() const
{
    return _Content;
}
void BMMessage::content(const QString & ctnt)
{
    _Content = ctnt;
}


PeerMessage::PeerMessage()
{

}

PeerMessage::~PeerMessage()
{

}

bool PeerMessage::Load(const QString & qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toUtf8());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("content")){

    }else{
        return false;
    }
    _Content = jsonMsg["content"].toString();
    return true;
}

QString PeerMessage::Serialize()const
{
    QJsonObject jsonMsg;
    jsonMsg["content"] = _Content;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
}

QString PeerMessage::content() const
{
    return _Content;
}
void PeerMessage::content(const QString & ctnt)
{
    _Content = ctnt;
}

GroupMessage::GroupMessage()
{

}

GroupMessage::~GroupMessage()
{

}

bool GroupMessage::Load(const QString & qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toUtf8());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("groupId")
            && jsonMsg.contains("groupName")
            && jsonMsg.contains("content")){

    }else{
        return false;
    }
    _GroupId = jsonMsg["groupId"].toString();
    _GroupName = jsonMsg["groupName"].toString();
    _Content = jsonMsg["content"].toString();
    return true;
}

QString GroupMessage::Serialize()const
{
    QJsonObject jsonMsg;
    jsonMsg["groupId"] = _GroupId;
    jsonMsg["groupName"] = _GroupName;
    jsonMsg["content"] = _Content;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
}

QString GroupMessage::groupId() const
{
    return _GroupId;
}
void GroupMessage::groupId(const QString & gid)
{
    _GroupId = gid;
}

QString GroupMessage::groupName()const
{
    return _GroupName;
}
void GroupMessage::groupName(const QString & gname)
{
    _GroupName = gname;
}

QString GroupMessage::content() const
{
    return _Content;
}
void GroupMessage::content(const QString & ctnt)
{
    _Content = ctnt;
}

SubMessage::SubMessage()
{

}

SubMessage::~SubMessage()
{

}

bool SubMessage::Load(const QString & qsJsonMsg)
{
    QJsonDocument jdoc = QJsonDocument::fromJson(qsJsonMsg.toUtf8());
    if (!jdoc.isObject()){
        return false;
    }
    QJsonObject jsonMsg = jdoc.object();
    if (jsonMsg.isEmpty()){
        return false;
    }
    if (jsonMsg.contains("refer")
            && jsonMsg.contains("content")){

    }else{
        return false;
    }
    _Refer = jsonMsg["refer"].toString();
    _Content = jsonMsg["content"].toString();
    return true;
}

QString SubMessage::Serialize()const
{
    QJsonObject jsonMsg;
    jsonMsg["refer"] = _Refer;
    jsonMsg["content"] = _Content;
    QJsonDocument jdoc;
    jdoc.setObject(jsonMsg);
    return jdoc.toJson();
}

QString SubMessage::refer()const
{
    return _Refer;
}
void SubMessage::refer(const QString & ref)
{
    _Refer = ref;
}

QString SubMessage::content() const
{
    return _Content;
}
void SubMessage::content(const QString & ctnt)
{
    _Content = ctnt;
}

