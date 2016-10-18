#include "bracthread.h"
#include <QDebug>
#include <bitmailcore/bitmail.h>


BracThread::BracThread(BitMail * bitmail)
    : m_fStop(false)
    , m_bitmail(bitmail)
    , m_txq(1000)
{

}

void BracThread::run()
{
    qDebug() << "BracThread:: run";

    while(!m_fStop){
        qDebug() << "BracThread: Fetch a copy of bracs to poll";
        std::vector<Brac *> copyBracs;
        do {
           QMutexLocker lock(&m_mutex);
           copyBracs = m_bracs;
        }while(0);

        // Do receiving
        qDebug() << "BracThread:: Polling";
        m_bitmail->PollBracs(copyBracs, 500);
        qDebug() << "BracThread:: Polling done";

        // Do sending
        while(m_txq.readable(0)){
            RTXMessage rtxMsg = m_txq.pop();
            if (rtxMsg.recip().size() > 1) { // do NOT support group sending
                ByMx(rtxMsg);
                continue ;
            }
            QString to = rtxMsg.recip().at(0);
            Brac * brac = GetBrac(copyBracs, to);
            if (NULL == brac){ // have NO brac connection yet.
                ByMx(rtxMsg);
                continue;
            }

            if (!brac->IsSendable()){ // check brac connection status
                ByMx(rtxMsg);
                continue;
            }

            BMMessage bmMsg;
            if (!bmMsg.Load(rtxMsg.content())){
                qDebug() << "Invalid BMMessage format";
                continue;
            }
            if (!brac->Send(bmMsg.Serialize().toStdString(), NULL, NULL)){
                ByMx(rtxMsg);
                continue;
            }
        }

        qDebug() << "BracThread: Merge bracs with copy";
        do{
            QMutexLocker lock(&m_mutex);
            // Update bracs if not-available brac;
        }while(0);
    }

    qDebug() << "BracThread:: done";

    emit signalBracThDone();
}

void BracThread::stop()
{
    m_fStop = true;
}

void BracThread::Add(Brac *brac)
{
    QMutexLocker lock(&m_mutex);
    m_bracs.push_back(brac);
}

void BracThread::ByMx(const RTXMessage &rtxMsg)
{
    (void)rtxMsg;
    //TODO: emit signal to TxThread;
}

Brac * BracThread::GetBrac(const std::vector<Brac*> & bracs, const QString &to)
{
    for (std::vector<Brac*>::const_iterator it = bracs.begin(); it != bracs.end(); it++){
        Brac * brac = *it;
        if (!brac->email().empty()
                && to == QString::fromStdString(brac->email())){
            return brac;
        }
    }
    return NULL;
}
