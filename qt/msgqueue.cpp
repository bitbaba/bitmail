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
    _Root = jdoc.object();
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("fTx")
            && _Root.contains("from")
            && _Root.contains("recip")
            && _Root.contains("content")
            && _Root.contains("certId")
            && _Root.contains("cert")){
        return true;
    }else{
        return false;
    }
}

QString RTXMessage::Serialize() const
{
    QJsonDocument jdoc;
    jdoc.setObject(_Root);
    return jdoc.toJson();
}

RTXMessage::operator QJsonObject() const
{
    return _Root;
}

bool RTXMessage::isTx() const
{
    return _Root["fTx"].toBool();
}

bool RTXMessage::isRx() const
{
    return ! _Root["fTx"].toBool();
}

QString RTXMessage::from() const
{
    return _Root["from"].toString();
}

QStringList RTXMessage::recip() const
{
    return _Root["recip"].toString().split(";");
}

QJsonObject RTXMessage::content() const
{
    return _Root["content"].toObject();
}

QString RTXMessage::certid() const
{
    return _Root["certId"].toString();
}

QString RTXMessage::cert() const
{
    return _Root["cert"].toString();
}

// Setters
void RTXMessage::rtx(bool fTx)
{
    _Root["fTx"] = fTx;
}

void RTXMessage::from(const QString & f)
{
    _Root["from"] = f;
}

void RTXMessage::recip(const QStringList & r)
{
    _Root["recip"] = r.join(";");
}

void RTXMessage::content(const QJsonObject & c)
{
    _Root["content"] = c;
}

void RTXMessage::certid(const QString & cid)
{
    _Root["certId"] = cid;
}

void RTXMessage::cert(const QString & c)
{
    _Root["cert"] = c;
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
    _Root = jdoc.object();
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("msgType")
            && _Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

bool BMMessage::Load(const QJsonObject & root)
{
    _Root = root;
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("msgType")
            && _Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

QString BMMessage::Serialize()
{
    QJsonDocument jdoc;
    jdoc.setObject(_Root);
    return jdoc.toJson();
}

BMMessage::operator QJsonObject() const
{
    return _Root;
}

enum MsgType BMMessage::msgType() const
{
    return (MsgType) _Root["msgType"].toInt();
}
bool BMMessage::isPeerMsg() const
{
    return ((MsgType) _Root["msgType"].toInt()) == mt_peer;
}
bool BMMessage::isGroupMsg()const
{
    return ((MsgType) _Root["msgType"].toInt()) == mt_group;
}
bool BMMessage::isSubMsg() const
{
    return ((MsgType) _Root["msgType"].toInt()) == mt_subscribe;
}
void BMMessage::msgType(MsgType mt)
{
    _Root["msgType"] = (int) mt;
}

QString BMMessage::bradExtUrl() const
{
    // Not required
    if (_Root.contains("bradExtUrl")){
        return _Root["bradExtUrl"].toString();
    }
    return "";
}

void BMMessage::bradExtUrl(const QString & extUrl)
{
    _Root["bradExtUrl"] = extUrl;
}

QJsonObject BMMessage::content() const
{
    return _Root["content"].toObject();
}
void BMMessage::content(const QJsonObject & ctnt)
{
    _Root["content"] = ctnt;
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
    _Root = jdoc.object();
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

bool PeerMessage::Load(const QJsonObject & root)
{
    _Root = root;
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

QString PeerMessage::Serialize()const
{
    QJsonDocument jdoc;
    jdoc.setObject(_Root);
    return jdoc.toJson();
}

PeerMessage::operator QJsonObject() const
{
    return _Root;
}

QString PeerMessage::content() const
{
    return _Root["content"].toString();
}
void PeerMessage::content(const QString & ctnt)
{
    _Root["content"] = ctnt;
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
    _Root = jdoc.object();
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("groupId")
            && _Root.contains("groupName")
            && _Root.contains("content")){
        return true;
    }else{
        return false;
    }
}


bool GroupMessage::Load(const QJsonObject & root)
{
    _Root = root;
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("groupId")
            && _Root.contains("groupName")
            && _Root.contains("content")){
        return true;
    }else{
        return false;
    }
}



QString GroupMessage::Serialize()const
{
    QJsonDocument jdoc;
    jdoc.setObject(_Root);
    return jdoc.toJson();
}

GroupMessage::operator QJsonObject() const
{
    return _Root;
}

QString GroupMessage::groupId() const
{
    return _Root["groupId"].toString();
}
void GroupMessage::groupId(const QString & gid)
{
    _Root["groupId"] = gid;
}

QString GroupMessage::groupName()const
{
    return _Root["groupName"].toString();
}
void GroupMessage::groupName(const QString & gname)
{
    _Root["groupName"] = gname;
}

QString GroupMessage::content() const
{
    return _Root["content"].toString();
}
void GroupMessage::content(const QString & ctnt)
{
    _Root["content"] = ctnt;
}

QStringList GroupMessage::members() const
{
    if (_Root.contains("members")){
        return _Root["members"].toString().split(";");
    }
    return QStringList();
}
void GroupMessage::members(const QStringList & m)
{
    _Root["members"] = m.join(";");
}

QString GroupMessage::creator() const
{
    if (_Root.contains("creator")){
        return _Root["creator"].toString();
    }
    return "";
}
void GroupMessage::creator(const QString & c)
{
    _Root["creator"] = c;
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
    _Root = jdoc.object();
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("refer")
            && _Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

bool SubMessage::Load(const QJsonObject & root)
{
    _Root = root;
    if (_Root.isEmpty()){
        return false;
    }
    if (_Root.contains("content")){
        return true;
    }else{
        return false;
    }
}

QString SubMessage::Serialize()const
{
    QJsonDocument jdoc;
    jdoc.setObject(_Root);
    return jdoc.toJson();
}

SubMessage::operator QJsonObject() const
{
    return _Root;
}

QString SubMessage::refer()const
{
    if (_Root.contains("refer")){
        return _Root["refer"].toString();
    }
    return "";
}
void SubMessage::refer(const QString & ref)
{
    _Root["refer"] = ref;
}

QString SubMessage::content() const
{
    return _Root["content"].toString();
}
void SubMessage::content(const QString & ctnt)
{
    _Root["content"] = ctnt;
}

