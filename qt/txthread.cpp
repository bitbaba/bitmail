#include "txthread.h"
#include "main.h"
#include <QDebug>
#include <QJsonDocument>
#include <bitmailcore/bitmail.h>

static
int TxProgressHandler(RTxState st, const char * info, void * userp);

TxThread::TxThread(BitMail * bm)
    : m_bitmail(bm)
    , m_txq(1000)
    , m_fStopFlag(false)
{

}

TxThread::~TxThread()
{

}

void TxThread::onSendMessage(const QString & from
                             , const QStringList & to
                             , const QString & content)
{
    (void)from;
    QJsonDocument doc;
    QJsonObject obj;
    obj["to"] = to.join("|");
    obj["msg"] = content;
    doc.setObject(obj);
    if (m_txq.writable(25/*milliseconds*/)){
        m_txq.push(doc.toJson());
    }else{
        qDebug() << "Tx Queue Overflow, missing message.";
    }
}

void TxThread::run()
{    
    while(!m_fStopFlag){

        if (!m_txq.readable(6*1000)) continue ;

        QJsonObject obj = QJsonDocument::fromJson(m_txq.pop().toUtf8()).object();
        QStringList to = obj["to"].toString().split("|");
        QString msg = obj["msg"].toString();

        std::vector<std::string> vecTo = BMQTApplication::toStdStringList(to);

        if (m_bitmail){
            m_bitmail->SendMsg(vecTo, msg.toStdString(), TxProgressHandler, this);
        }

    }

    emit done();
}

void TxThread::stop()
{
    m_fStopFlag = true;
}

void TxThread::NotifyTxProgress(int st, const QString &info)
{
    emit txProgress(st, info);
}

int TxProgressHandler(RTxState st, const char * info, void * userp)
{
    TxThread * self = (TxThread *)userp;
    if (self == NULL){
        return bmInvalidParam;
    }
    self->NotifyTxProgress((int)st, QString::fromLatin1(info));
    return bmOk;
}
