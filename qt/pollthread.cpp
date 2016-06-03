#include <QDebug>

#include "pollthread.h"

#include <bitmailcore/bitmail.h>

static
int PollEventHandler(unsigned int count, void * p);

PollThread::PollThread(BitMail * bm)
    : m_bitmail(bm)
    , m_lastCount(0)
    , m_reIdleInterval(60*1000) // RFC recommended 30 minutes, here less than that.
    , m_fStopFlag(false)
{

}

PollThread::~PollThread()
{

}

void PollThread::run()
{
    m_bitmail->OnPollEvent(PollEventHandler, this);

    while(!m_fStopFlag){

        m_bitmail->StartIdle(m_reIdleInterval);

        qDebug() << "poll timeout";
    }

    qDebug() << "Poll Thread quit";
}

void PollThread::stop()
{
    m_fStopFlag = true;
}

void PollThread::NotifyInboxPollEvent(unsigned int count)
{
    if (m_lastCount == count){
        return ;
    }

    m_lastCount = count;

    emit inboxPollEvent();

    return ;
}

int PollEventHandler(unsigned int count, void * p)
{
    (void)count;    (void)p;
    PollThread * self = (PollThread *)p;
    self->NotifyInboxPollEvent(count);
    return 0;
}
