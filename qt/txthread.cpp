#include "txthread.h"
#include <QDebug>
#include <bitmailcore/bitmail.h>


TxThread::TxThread(BitMail * bm, unsigned int qsize)
    : m_bitmail(bm)
    , m_txq(qsize)
{

}

TxThread::~TxThread()
{

}

void TxThread::onSendMessage(const QString &to, const QString &msg)
{
    (void)to; (void)msg;
    BitMailMessage bmMsg("", to, msg);
    if (!m_txq.writable(1000)){
        qDebug() << "miss";
    }else{
        m_txq.push(bmMsg);
    }
}

void TxThread::onGroupMessage(const QStringList &to, const QString &msg)
{
    (void)to; (void)msg;
}

void TxThread::run()
{
    while(true){
        if (m_txq.readable(-1)){
            BitMailMessage msg = m_txq.pop();
            (void)msg;
            QString qsFrom = msg.from();
            (void)qsFrom;
            QString qsTo = msg.to();
            QString qsMsg = msg.msg();
            if (m_bitmail){
                m_bitmail->SendMsg(qsTo.toStdString(), qsMsg.toStdString());
            }
        }
    }
}
