#include "txthread.h"
#include <QDebug>
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

void TxThread::onSendMessage(const QStringList &to, const QString &msg)
{
    BitMailMessage bmMsg("", to, msg, "");
    if (!m_txq.writable(25/*milliseconds*/)){

    }else{
        m_txq.push(bmMsg);
    }
}

void TxThread::run()
{    
    while(!m_fStopFlag){
        if (m_txq.readable(6*1000)){
            BitMailMessage msg = m_txq.pop();
            QStringList qslTo = msg.to();
            std::vector<std::string> vecTo;
            for (QStringList::iterator it = qslTo.begin(); it != qslTo.end(); it++){
                vecTo.push_back(it->toStdString());
            }
            QString qsMsg = msg.msg();
            if (m_bitmail){
                m_bitmail->SendMsg(vecTo
                                   , qsMsg.toStdString()
                                   , TxProgressHandler
                                   , this);
            }

        }
    }

    emit done();
}

void TxThread::stop()
{
    m_fStopFlag = true;
}

void TxThread::NotifyTxProgress(const QString &info)
{
    emit txProgress(info);
}

int TxProgressHandler(RTxState st, const char * info, void * userp)
{
    (void)st;
    TxThread * self = (TxThread *)userp;
    if (self == NULL){
        return bmInvalidParam;
    }
    self->NotifyTxProgress(QString::fromLatin1(info));
    return bmOk;
}
