#include "txthread.h"
#include <QDebug>
#include <bitmailcore/bitmail.h>


TxThread::TxThread(BitMail * bm)
    : m_bitmail(bm)
    , m_txq(1000)
    , m_fStopFlag(false)
{

}

TxThread::~TxThread()
{

}

void TxThread::onSendMessage(const QString &to, const QString &msg)
{
    (void)to; (void)msg;
    BitMailMessage bmMsg("", to, msg, "");
    if (!m_txq.writable(25/*milliseconds*/)){
        qDebug() << "tx miss";
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
    while(!m_fStopFlag){
        if (m_txq.readable(6*1000)){
            BitMailMessage msg = m_txq.pop();
            (void)msg;
            QString qsFrom = msg.from();
            (void)qsFrom;
            QString qsTo = msg.to();
            QString qsMsg = msg.msg();
            if (m_bitmail){
                m_bitmail->SendMsg(qsTo.toStdString(), qsMsg.toStdString());
            }
        }else{
            qDebug() << "tx queue fetch timeout";
        }
    }

    emit done();

    qDebug() << "Tx Thread quit";
}

void TxThread::stop()
{
    m_fStopFlag = true;
}
