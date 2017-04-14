#ifndef PROCTH_H
#define PROCTH_H

#include <QThread>
#include <QObject>

class ProcTh : public QThread
{
    Q_OBJECT
public:
    ProcTh(const QString & sessKey, const QString & app, const QStringList & args, const QString & output);
public:
    void run();
signals:
    void procDone( const QString & sessKey, const QString & output);
private:
    QString SessKey;
    QString App;
    QStringList Args;
    QString Output;
};

#endif // PROCTH_H
