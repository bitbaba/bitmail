#include "procth.h"

#include <QProcess>

ProcTh::ProcTh(const QString & sessKey, const QString & app, const QStringList & args, const QString & output)
    : SessKey(sessKey)
    , App(app)
    , Args(args)
    , Output(output)
{

}

void ProcTh::run()
{
    QProcess proc;
    Args.append("-o");
    Args.append(Output);
    proc.start(App, Args);
    if (!proc.waitForStarted(-1)){
        return ;
    }
    if (!proc.waitForFinished(-1)){
        return ;
    }
    emit procDone(SessKey, Output);
}
