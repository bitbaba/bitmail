#ifndef BRADTHREAD_H
#define BRADTHREAD_H

#include <QObject>
#include <QThread>

class BradThread : public QThread
{
    Q_OBJECT
public:
    BradThread(unsigned short port);
public:
    void run();
    void stop();
protected:
    static int OnInboundConnection(int sockfd, void * userp);
    void onCreateBrac(int sockfd);

signals:
    void signalCreateBrac(int sockfd);
    void signalBradThDone();
private:
    bool m_fStop;
    unsigned short m_port;
};

#endif // BRADTHREAD_H
