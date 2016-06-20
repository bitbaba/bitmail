/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
//! [0]
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextStream>
#include <QTextCodec>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include "optiondialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include <bitmailcore/bitmail.h>
#include "main.h"
static
FILE * gLoggerHandle = NULL;
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(bitmail);
    QApplication app(argc, argv);
    app.setOrganizationName(("BitMail"));
    app.setApplicationName(("BitMail Qt Client"));
    // About logger
    if (!BMQTApplication::InitLogger()){
        return 1;
    }else{
        qInstallMessageHandler(BMQTApplication::myMessageOutput);
    }
    qDebug() << "BitMail logger works.";
    // About locale
    // http://doc.qt.io/qt-4.8/qlocale.html
    // 1) track system locale, that is, you do NOT call QLocale::setDefault(locale);
    // 2) or use locale specified by custom in arguments, like this: QLocale::setDefault(QLocale(lang, coutry));
    // 3) use UTF-8 as default text codec, between unicode and ansi
    // Query current locale by
    QLocale().name();
    // Query system locale by
    QLocale::system().name();
    // TODO: Locale from arguments
    // int lang = QLocale::Chinese;
    // int cout = QLocale::China;
    // QLocale::setDefault(QLocale(lang, cout));
    // Codec by UTF-8
    // http://doc.qt.io/qt-5/qtextcodec.html#setCodecForLocale
    // http://www.iana.org/assignments/character-sets/character-sets.xml
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    // Deprecated above Qt-5.0
    // default use UTF-8 for c-string & translation
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    // Get Account Profile
    QString qsEmail, qsPassphrase;
    LoginDialog loginDialog;
    int dlgret = (loginDialog.exec());
    if (dlgret == QDialog::Rejected){
        return 1;
    }
    if (dlgret == QDialog::Accepted){
        qsEmail = loginDialog.GetEmail();
        qsPassphrase = loginDialog.GetPassphrase();
    }
    if (dlgret == LoginDialog::CreateNew){
        OptionDialog optDialog(true);
        if (optDialog.exec() != QDialog::Accepted){
            return 2;
        }
        qsEmail = optDialog.GetEmail();
        qsPassphrase = optDialog.GetPassphrase();
    }
    BitMail * bitmail = new BitMail();
    if (!BMQTApplication::LoadProfile(bitmail, qsEmail, qsPassphrase)){
        return 0;
    }
    MainWindow mainWin(bitmail);
    mainWin.show();
    int retCode = app.exec();

    BMQTApplication::CloseLogger();
    return retCode;
}
//! [0]
namespace BMQTApplication {
    QString GetAppHome()
    {
        return QApplication::applicationDirPath();
    }
    QString GetProfileHome()
    {
        QString qsProfileHome = QDir::homePath();
        qsProfileHome += "/";
        qsProfileHome += "bitmail";
        qsProfileHome += "/";
        qsProfileHome += "profile";
        return qsProfileHome;
    }
    QString GetDataHome()
    {
        QString qsProfileHome = QDir::homePath();
        qsProfileHome += "/";
        qsProfileHome += "bitmail";
        qsProfileHome += "/";
        qsProfileHome += "data";
        return qsProfileHome;
    }
    QStringList GetProfiles()
    {
        QString qsProfileHome = GetProfileHome();
        QDir profileDir(qsProfileHome);
        if (!profileDir.exists()){
             profileDir.mkpath(qsProfileHome);
        }
        QStringList slist;
        foreach(QFileInfo fi, profileDir.entryInfoList()){
            if (fi.isFile() && IsValidPofile(fi.absoluteFilePath())){
                slist.append(fi.absoluteFilePath());
            }
        }
        return slist;
    }
    QString GetProfilePath(const QString &email)
    {
        QString qsProfilePath = GetProfileHome();
        qsProfilePath += "/";
        qsProfilePath += email;
        return qsProfilePath;
    }
    bool IsValidPofile(const QString & qsProfile)
    {
        QFile profile(qsProfile);
        if (!profile.exists()){
            return false;
        }
        if (!profile.open(QFile::ReadOnly | QFile::Text)){
            return false;
        }
        QJsonDocument jdoc;
        jdoc = QJsonDocument::fromJson(profile.readAll());
        if (!jdoc.isObject()){
            return false;
        }
        QJsonObject joRoot = jdoc.object();
        if (!joRoot.contains("Profile")){
            return false;
        }
        return true;
    }
    bool LoadProfile(BitMail * bm, const QString &email, const QString & passphrase)
    {
        QString qsProfile = GetProfilePath(email);
        QFile file(qsProfile);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return false;
        }
        QTextStream in(&file);
        QJsonDocument jdoc;
        jdoc = QJsonDocument::fromJson(in.readAll().toLatin1());
        QJsonObject joRoot = jdoc.object();
        QJsonObject joProfile;
        QJsonObject joTx;
        QJsonObject joRx;
        QJsonObject joBuddies;
        QJsonObject joGroups;
        (void)joGroups;
        QJsonObject joSubscribes;
        (void)joSubscribes;
        QJsonObject joProxy;
        if (joRoot.contains("Profile")){
            joProfile = joRoot["Profile"].toObject();
            QString qsEmail;
            if (joProfile.contains("email")){
                qsEmail = joProfile["email"].toString();
            }
            QString qsNick;
            if (joProfile.contains("nick")){
                qsNick = joProfile["nick"].toString();
            }
            QString qsCert;
            if (joProfile.contains("cert")){
                qsCert = joProfile["cert"].toString();
            }
            QString qsKey;
            if (joProfile.contains("key")){
                qsKey = joProfile["key"].toString();
            }
            (void)passphrase;
            if (bmOk != bm->LoadProfile(passphrase.toStdString()
                            , qsKey.toStdString()
                            , qsCert.toStdString())){
                return false;
            }
        }
        QString qsTxUrl, qsTxLogin, qsTxPassword;
        if (joRoot.contains("tx")){
            joTx = joRoot["tx"].toObject();
            if (joTx.contains("url")){
                qsTxUrl = joTx["url"].toString();
            }
            if (joTx.contains("login")){
                qsTxLogin = joTx["login"].toString();
            }
            if (joTx.contains("password")){
                qsTxPassword = QString::fromStdString( bm->Decrypt(joTx["password"].toString().toStdString()));
            }
        }
        QString qsRxUrl, qsRxLogin, qsRxPassword;
        if (joRoot.contains("rx")){
            joRx = joRoot["rx"].toObject();
            if (joRx.contains("url")){
                qsRxUrl = joRx["url"].toString();
            }
            if (joRx.contains("login")){
                qsRxLogin = joRx["login"].toString();
            }
            if (joRx.contains("password")){
                qsRxPassword = QString::fromStdString( bm->Decrypt(joRx["password"].toString().toStdString()));
            }
        }
        bm->InitNetwork(qsTxUrl.toStdString(), qsTxLogin.toStdString(), qsTxPassword.toStdString()
                        , qsRxUrl.toStdString(), qsRxLogin.toStdString(), qsRxPassword.toStdString());
        if (joRoot.contains("buddies"))
        {
            joBuddies = joRoot["buddies"].toObject();
            for(QJsonObject::const_iterator it = joBuddies.constBegin()
                ; it != joBuddies.constEnd(); it++)
            {
                QString qsEmail = it.key();
                QJsonObject joValue = it.value().toObject();
                QString qsCert =  joValue["cert"].toString();
                if (!bm->HasFriend(qsEmail.toStdString())){
                    bm->AddFriend(qsEmail.toStdString(), qsCert.toStdString());
                }
            }
        }
        if (joRoot.contains("proxy")){
            joProxy = joRoot["proxy"].toObject();
            if (joProxy.contains("enable")){
                bool fProxyEnable = joProxy["enable"].toBool();
                bm->EnableProxy(fProxyEnable);
            }else{
                bm->EnableProxy(false);
            }
            do {
                if (joProxy.contains("ip")){
                    QString qsVal = joProxy["ip"].toString();
                    bm->SetProxyIp(qsVal.toStdString());
                }
                if (joProxy.contains("port")){
                    unsigned short port = joProxy["port"].toInt();
                    bm->SetProxyPort(port);
                }
                if (joProxy.contains("user")){
                    QString qsVal = joProxy["user"].toString();
                    bm->SetProxyUser(qsVal.toStdString());
                }
                if (joProxy.contains("password")){
                    QString qsVal = joProxy["password"].toString();
                    bm->SetProxyPassword(bm->Decrypt(qsVal.toStdString()));
                }
                if (joProxy.contains("remoteDNS")){
                    bool fRemoteDNS = joProxy["remoteDNS"].toBool();
                    bm->RemoteDNS(fRemoteDNS);
                }
            }while(0);
        }
        return true;
    }
    bool SaveProfile(BitMail * bm)
    {
        QString qsEmail = QString::fromStdString(bm->GetEmail());
        QString qsProfile = GetProfilePath(qsEmail);
        // Format Profile to QJson
        QJsonObject joRoot;
        QJsonObject joProfile;
        QJsonObject joTx;
        QJsonObject joRx;
        QJsonObject joBuddies;
        QJsonObject joGroups;     // Group chatting
        (void) joGroups;
        QJsonObject joSubscribes; // Subscribing
        (void) joSubscribes;
        QJsonObject joProxy;
        joProfile["email"] = QString::fromStdString(bm->GetEmail());
        joProfile["nick"] = QString::fromStdString(bm->GetNick());
        joProfile["key"] = QString::fromStdString(bm->GetKey());
        joProfile["cert"] = QString::fromStdString(bm->GetCert());
        joTx["url"] = QString::fromStdString(bm->GetTxUrl());
        joTx["login"] = QString::fromStdString(bm->GetTxLogin());
        joTx["password"] = QString::fromStdString(bm->Encrypt(bm->GetTxPassword()));
        joRx["url"] = QString::fromStdString(bm->GetRxUrl());
        joRx["login"] = QString::fromStdString(bm->GetRxLogin());
        joRx["password"] = QString::fromStdString(bm->Encrypt(bm->GetRxPassword()));
        std::vector<std::string > vecBuddies;
        bm->GetFriends(vecBuddies);
        for (std::vector<std::string>::const_iterator it = vecBuddies.begin(); it != vecBuddies.end(); ++it){
            std::string sBuddyCertPem = bm->GetFriendCert(*it);
            QJsonObject joBuddy;
            joBuddy["cert"]  = QString::fromStdString(sBuddyCertPem);
            QString qsEmail = QString::fromStdString(*it);
            joBuddies.insert(qsEmail, joBuddy);
        }
        joProxy["enable"] = bm->EnableProxy();
        do {
            joProxy["ip"] = QString::fromStdString( bm->GetProxyIp());
            joProxy["port"] = (int)bm->GetProxyPort();
            joProxy["user"] = QString::fromStdString(bm->GetProxyUser());
            joProxy["password"] = QString::fromStdString(bm->Encrypt(bm->GetProxyPassword()));
            joProxy["remoteDNS"] = bm->RemoteDNS();
        }while(0);
        // for more readable, instead of `profile'
        joRoot["Profile"] = joProfile;
        joRoot["tx"] = joTx;
        joRoot["rx"] = joRx;
        joRoot["buddies"] = joBuddies;
        joRoot["proxy"] = joProxy;
        QJsonDocument jdoc(joRoot);
        QFile file(qsProfile);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        QTextStream out(&file);
        out << jdoc.toJson();
        return true;
    }
    bool InitLogger(){
        QString qsLoggerPath = GetDataHome();
        QDir qDataDir = QDir(qsLoggerPath);
        if (!qDataDir.exists()){
            qDataDir.mkpath(qsLoggerPath);
        }
        qsLoggerPath += "/";
        qsLoggerPath += "bitmail.log";
        if (gLoggerHandle != NULL){
            fclose(gLoggerHandle);
            gLoggerHandle = NULL;
        }
        gLoggerHandle = fopen(qsLoggerPath.toStdString().c_str(), "w");
        return gLoggerHandle != NULL;
    }
    bool CloseLogger(){
        fclose(gLoggerHandle);
        gLoggerHandle = NULL;
        return true;
    }
    FILE * GetLogger(){
        return gLoggerHandle;
    }
    void FlushLogger(){
        if (gLoggerHandle){
            fflush(gLoggerHandle);
        }
    }
    void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        (void) context;
        FILE * fout = GetLogger();
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
        case QtDebugMsg:
            fprintf(fout, "[%s] - Debug: %s \n", QDateTime::currentDateTime().toString().toStdString().c_str(), localMsg.constData());
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            fprintf(fout, "[%s] - Info: %s \n", QDateTime::currentDateTime().toString().toStdString().c_str(), localMsg.constData());
            break;
#endif
        case QtWarningMsg:
            fprintf(fout, "[%s] - Warning: %s \n", QDateTime::currentDateTime().toString().toStdString().c_str(), localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(fout, "[%s] - Critical: %s \n", QDateTime::currentDateTime().toString().toStdString().c_str(), localMsg.constData());
            break;
        case QtFatalMsg:
            fprintf(fout, "[%s] - Fatal: %s \n", QDateTime::currentDateTime().toString().toStdString().c_str(), localMsg.constData());
            abort();
        }
        FlushLogger();
    }
}

