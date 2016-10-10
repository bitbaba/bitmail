#include "bracthread.h"
#include <QDebug>
#include <bitmailcore/bitmail.h>

BracThread::BracThread()
    : m_fStop(false)
{

}

void BracThread::run()
{
    qDebug() << "BracThread:: run";

    while(!m_fStop){
        qDebug() << "BracThread:: Polling";
        QThread::sleep(5);
        qDebug() << "BracThread:: Polling done";
    }

    qDebug() << "BracThread:: done";

    emit signalBracThDone();
}

void BracThread::stop()
{
    m_fStop = true;
}
