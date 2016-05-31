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

#include "optiondialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include <bitmailcore/bitmail.h>

#include "main.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(bitmail);
    QApplication app(argc, argv);
    app.setOrganizationName("QtProject");
    app.setApplicationName("Application Example");

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
        OptionDialog optDialog;
        if (optDialog.exec() != QDialog::Accepted){
            return 2;
        }
        qsEmail = optDialog.GetEmail();
        qsPassphrase = optDialog.GetPassphrase();
    }

    MainWindow mainWin(qsEmail, qsPassphrase);
    mainWin.show();
    return app.exec();
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

    void LoadProfile(BitMail * bm, const QString &email, const QString & passphrase)
    {
        QString qsProfile = GetProfilePath(email);

        QFile file(qsProfile);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return;
        }

        QTextStream in(&file);

        QJsonDocument jdoc;
        jdoc = QJsonDocument::fromJson(in.readAll().toLatin1());

        QJsonObject joRoot = jdoc.object();
        QJsonObject joProfile;
        QJsonObject joTx;
        QJsonObject joRx;
        QJsonArray  jaBuddies;

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
            bm->LoadProfile(passphrase.toStdString()
                            , qsKey.toStdString()
                            , qsCert.toStdString());

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

        if (joRoot.contains("buddies")){
            jaBuddies = joRoot["buddies"].toArray();
            for(QJsonArray::const_iterator it = jaBuddies.constBegin()
                ; it != jaBuddies.constEnd()
                ; it++){
                const QJsonObject & joBuddy = (*it).toObject();
                QString qsEmail = joBuddy["email"].toString();
                QString qsCert =  joBuddy["cert"].toString();
                (void )qsEmail;
                bm->AddBuddy(qsCert.toStdString());
            }
        }
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
        QJsonArray  jaBuddies;

        joProfile["email"] = QString::fromStdString(bm->GetEmail());
        joProfile["nick"] = QString::fromStdString(bm->GetCommonName());
        joProfile["key"] = QString::fromStdString(bm->GetKey());
        joProfile["cert"] = QString::fromStdString(bm->GetCert());

        joTx["url"] = QString::fromStdString(bm->GetTxUrl());
        joTx["login"] = QString::fromStdString(bm->GetTxLogin());
        joTx["password"] = QString::fromStdString(bm->Encrypt(bm->GetTxPassword()));

        joRx["url"] = QString::fromStdString(bm->GetRxUrl());
        joRx["login"] = QString::fromStdString(bm->GetRxLogin());
        joRx["password"] = QString::fromStdString(bm->Encrypt(bm->GetRxPassword()));
        joRx["stranger"] = bm->AllowStranger();

        std::vector<std::string > vecBuddies;
        bm->GetBuddies(vecBuddies);
        for (std::vector<std::string>::const_iterator it = vecBuddies.begin(); it != vecBuddies.end(); ++it){
            std::string sBuddyCertPem = bm->GetBuddyCert(*it);
            QJsonObject joBuddy;
            joBuddy["email"] = QString::fromStdString(*it);
            joBuddy["cert"]  = QString::fromStdString(sBuddyCertPem);
            jaBuddies.append(joBuddy);
        }

        // for more readable, instead of `profile'
        joRoot["Profile"] = joProfile;
        joRoot["tx"] = joTx;
        joRoot["rx"] = joRx;
        joRoot["buddies"] = jaBuddies;

        QJsonDocument jdoc(joRoot);

        QFile file(qsProfile);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }

        QTextStream out(&file);

        out << jdoc.toJson();

        return true;
    }

    void CreateProfile()
    {
#if 0
        OptionDialog optDialog(true, this);
        if (QDialog::Accepted != optDialog.exec()){
            return ;
        }

        QString qsEmail = optDialog.GetEmail();
        QString qsNick = optDialog.GetNick();
        QString qsPassphrase = optDialog.GetPassphrase();
        int nBits = optDialog.GetBits();

        QString qsSmtpUrl = optDialog.GetSmtpUrl();
        QString qsSmtpLogin = optDialog.GetSmtpLogin();
        QString qsSmtpPassword = optDialog.GetSmtpPassword();

        QString qsImapUrl = optDialog.GetImapUrl();
        QString qsImapLogin = optDialog.GetImapLogin();
        QString qsImapPassword = optDialog.GetImapPassword();
        bool fImapAllowStranger = optDialog.GetImapAllowStranger();

        m_bitmail->CreateProfile(qsNick.toStdString()
                          , qsEmail.toStdString()
                          , qsPassphrase.toStdString()
                          , nBits);

        m_bitmail->InitNetwork(qsSmtpUrl.toStdString()
                        , qsSmtpLogin.toStdString()
                        , qsSmtpPassword.toStdString()
                        , qsImapUrl.toStdString()
                        , qsImapLogin.toStdString()
                        , qsImapPassword.toStdString());

        m_bitmail->AllowStranger(fImapAllowStranger);
#endif
    }

}


