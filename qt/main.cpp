#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "mainwindow.h"
#include "logindialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    LoginDialog loginDlg;
    if (loginDlg.exec() == QDialog::Accepted){
        if (w.Initialize(loginDlg.GetEmail(), loginDlg.GetKeyPass())){
            w.show();
            return a.exec();
        }
    }
    a.exit(-1);
    return -1;
}
