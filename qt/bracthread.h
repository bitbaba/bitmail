#ifndef BRACTHREAD_H
#define BRACTHREAD_H

#include <QObject>
#include <QThread>

class Brac;

class BracThread : public QThread
{
    Q_OBJECT
public:
    BracThread();
public:
    void run();
    void stop();
signals:
    void signalBracThDone();
private:
    bool m_fStop;
    std::vector<Brac *> m_bracs;
};

#endif // BRACTHREAD_H
