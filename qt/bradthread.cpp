#include "bradthread.h"
#include "bracthread.h"
#include <QDebug>
#include <bitmailcore/bitmail.h>

BradThread::BradThread(unsigned short port)
    : m_fStop(false)
    , m_port(port)
{

}

void BradThread::run()
{
    qDebug() << "BradThread:: run";

    Brad * brad = new Brad(m_port, OnInboundConnection, this);
    brad->Startup();

    while(!m_fStop){
        qDebug() << "BradThread: Waiting for connections";
        brad->WaitForConnections(5000);
        qDebug() << "BradThread: timeout";
    }

    if (brad){
        brad->Shutdown();
        delete brad; brad = NULL;
    }

    qDebug() << "BradThread:: done";
    emit signalBradThDone();
}

void BradThread::stop()
{
    m_fStop = true;
}

int BradThread::OnInboundConnection(int sockfd, void *userp)
{
    qDebug() << "BradThread:: *Connection coming";
    BradThread * self = (BradThread *)userp;
    self->onCreateBrac(sockfd);
    return bmOk;
}

void BradThread::onCreateBrac(int sockfd)
{
    emit signalCreateBrac(sockfd);
}
