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
#include <QTranslator>
#include <QLibraryInfo>
#include <QNetworkInterface>
#include <QMimeData>
#include <QUuid>
#include <QCryptographicHash>
#include <QMimeDatabase>
#include <QMimeType>

#include "optiondialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "assistantdialog.h"
#include <bitmailcore/bitmail.h>
#include "main.h"
#include "lock.h"

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
    qDebug() << "Application Locale: " << QLocale().name();
    // Query system locale by
    qDebug() << "System Locale: " << QLocale::system().name();

    // TODO: Locale from arguments
    //QLocale::Language lang = QLocale::Japanese;
    //QLocale::Country  cntry= QLocale::Japan;
    //QLocale::setDefault(QLocale(lang, cntry));

    // Query current locale: changed by above setting
    qDebug() << "Application Locale: " << QLocale().name();
    // Query system locale: intact
    qDebug() << "System Locale: " << QLocale::system().name();

    // Codec by UTF-8
    // http://doc.qt.io/qt-5/qtextcodec.html#setCodecForLocale
    // http://www.iana.org/assignments/character-sets/character-sets.xml
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    // Deprecated above Qt-5.0
    // default use UTF-8 for c-string & translation
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // Ref: http://doc.qt.io/qt-5/internationalization.html

    // i18n for qt
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_" + QLocale().name(), BMQTApplication::GetLocaleHome())){
        app.installTranslator(&qtTranslator);
    }

    // i18n for bitmail
    QTranslator appTranslator;
    if (appTranslator.load("bitmail_" + QLocale().name(), BMQTApplication::GetLocaleHome())){
        app.installTranslator(&appTranslator);
    }


    // Get Account Profile
    QString qsEmail, qsPassphrase;
    bool fAssistant = false;

    while (true){
        LoginDialog loginDialog;
        int dlgret = (loginDialog.exec());
        if (dlgret == QDialog::Rejected){
            qDebug() << "User close login window, bye!";
            return 1;
        }else if (dlgret == QDialog::Accepted){
            fAssistant = loginDialog.imAssistant();
            qsEmail = loginDialog.GetEmail();
            qsPassphrase = loginDialog.GetPassphrase();
            break;
        }else if (dlgret == LoginDialog::CreateNew){
            OptionDialog optDialog(true);
            if (optDialog.exec() != QDialog::Accepted){
                continue;
            }
            qsEmail = optDialog.GetEmail();
            qsPassphrase = optDialog.GetPassphrase();
            break;
        }
    };

    BitMail * bitmail = new BitMail(new BMLockFactory());
    if (!BMQTApplication::LoadProfile(bitmail, qsEmail, qsPassphrase)){
        qDebug() << "Failed to Load Profile, bye!";
        return 0;
    }

    if (!fAssistant){
        MainWindow mainWin(bitmail);
        mainWin.show();
        if (bitmail->GetTxUrl().empty()){
            mainWin.configNetwork();
            if (bitmail->GetTxUrl().empty()){
                qDebug() << "Main: no network configuration, exit";
                return 0;
            }
        }
        mainWin.startupNetwork();
        app.exec();
    }else{
        AssistantDialog assistantDlg(bitmail);
        assistantDlg.show();
        app.exec();
    }

    BMQTApplication::SaveProfile(bitmail);

    delete bitmail; bitmail = NULL;

    qDebug() << "BitMail quit! Bye";

    BMQTApplication::CloseLogger();

    return 0;
}

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
    QString GetLogHome()
    {
        QString qsProfileHome = QDir::homePath();
        qsProfileHome += "/";
        qsProfileHome += "bitmail";
        qsProfileHome += "/";
        qsProfileHome += "log";
        return qsProfileHome;
    }
    QString GetLanIp()
    {
        QString lanaddr;
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol
                    && !address.isLoopback()
                    && (address.isInSubnet(QHostAddress::parseSubnet("192.168.0.0/16"))
                        ||  address.isInSubnet(QHostAddress::parseSubnet("172.16.0.0/12"))
                        ||  address.isInSubnet(QHostAddress::parseSubnet("10.0.0.0/8")))
               )
            {
                 lanaddr = address.toString();
                 break; // take only one
            }
        }
        return lanaddr;
    }

    QString GetLocaleHome()
    {
        QString qsLocaleHome = GetAppHome();
        qsLocaleHome += "/";
        qsLocaleHome += "locale";
        return qsLocaleHome;
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
        QJsonObject joBrad;
        QJsonObject joBuddies;
        QJsonObject joGroups;
        (void)joGroups;
        QJsonArray jaSubscribes;
        (void)jaSubscribes;
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
            // Brad
            if (joProfile.contains("brad")){
                joBrad = joProfile["brad"].toObject();
                unsigned short port = joBrad["port"].toInt();
                bm->SetBradPort(BMQTApplication::GetLanIp().toStdString(), port);
            }
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

        if (joRoot.contains("friends"))
        {
            joBuddies = joRoot["friends"].toObject();
            for(QJsonObject::const_iterator it = joBuddies.constBegin()
                ; it != joBuddies.constEnd(); it++)
            {
                QString qsEmail = it.key();
                QJsonObject joValue = it.value().toObject();
                if (joValue.contains("cert")){
                    QString qsCert =  joValue["cert"].toString();
                    if (!bm->HasFriend(qsEmail.toStdString())){
                        bm->AddFriend(qsEmail.toStdString(), qsCert.toStdString());
                    }
                }
                if (joValue.contains("brad")){
                    QJsonObject joFriendBrad = joValue["brad"].toObject();
                    if (joFriendBrad.contains("extUrl")){
                        QString qsFriendBradExtUrl = joFriendBrad["extUrl"].toString();
                        bm->SetFriendBradExtUrl(qsEmail.toStdString(), qsFriendBradExtUrl.toStdString());
                    }
                }
            }
        }

        do {
            if (!joRoot.contains("groups")){
                break;
            }
            joGroups = joRoot["groups"].toObject();
            for (QJsonObject::const_iterator it = joGroups.constBegin()
                 ; it != joGroups.constEnd()
                 ; it ++){
                QString qsGroupId = it.key();
                QJsonObject joGroup = it.value().toObject();
                QString qsGroupName = "";
                if (joGroup.contains("name")){
                    qsGroupName = joGroup["name"].toString();
                }
                bm->AddGroup(qsGroupId.toStdString(), qsGroupName.toStdString());

                QString qsGroupCreator;
                if (joGroup.contains("creator")){
                    qsGroupCreator = joGroup["creator"].toString();
                }
                bm->SetGroupCreator(qsGroupId.toStdString(), qsGroupCreator.toStdString());

                if (joGroup.contains("members")){
                    QJsonArray jaMembers = joGroup["members"].toArray();
                    for (int i = 0; i < jaMembers.size(); i++){
                        QString qsMember = jaMembers.at(i).toString();
                        bm->AddGroupMember(qsGroupId.toStdString(), qsMember.toStdString());
                    }
                }
            }
        }while(0);

        do{
            if (!joRoot.contains("subscribes")){
                break;
            }
            jaSubscribes = joRoot["subscribes"].toArray();
            for (int i = 0; i < jaSubscribes.size(); i++){
                QString qsSub = jaSubscribes.at(i).toString();
                bm->Subscribe(qsSub.toStdString());
            }
        }while(0);

        if (joRoot.contains("proxy")){
            joProxy = joRoot["proxy"].toObject();
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
        QJsonObject joBrad;
        QJsonObject joBuddies;
        QJsonObject joGroups;     // Group chatting
        (void) joGroups;
        QJsonArray jaSubscribes; // Subscribing
        (void) jaSubscribes;
        QJsonObject joProxy;
        // Brad
        joBrad["port"] = (int) bm->GetBradPort();
        // Profile
        joProfile["email"] = QString::fromStdString(bm->GetEmail());
        joProfile["nick"] = QString::fromStdString(bm->GetNick());
        joProfile["key"] = QString::fromStdString(bm->GetKey());
        joProfile["cert"] = QString::fromStdString(bm->GetCert());
        joProfile["brad"] = joBrad;
        // Tx
        joTx["url"] = QString::fromStdString(bm->GetTxUrl());
        joTx["login"] = QString::fromStdString(bm->GetTxLogin());
        joTx["password"] = QString::fromStdString(bm->Encrypt(bm->GetTxPassword()));
        // Rx
        joRx["url"] = QString::fromStdString(bm->GetRxUrl());
        joRx["login"] = QString::fromStdString(bm->GetRxLogin());
        joRx["password"] = QString::fromStdString(bm->Encrypt(bm->GetRxPassword()));
        // friends
        std::vector<std::string > vecBuddies;
        bm->GetFriends(vecBuddies);
        for (std::vector<std::string>::const_iterator it = vecBuddies.begin(); it != vecBuddies.end(); ++it){
            std::string sBuddyCertPem = bm->GetFriendCert(*it);
            QJsonObject joBuddy;
            joBuddy["cert"]  = QString::fromStdString(sBuddyCertPem);
            if (false){ // no need to save friend's brad
                QJsonObject joFriendBrad;
                joFriendBrad["extUrl"] = QString::fromStdString(bm->GetFriendBradExtUrl(*it));
                joBuddy["brad"] = joFriendBrad;
            }
            QString qsEmail = QString::fromStdString(*it);
            joBuddies.insert(qsEmail, joBuddy);
        }
        // Groups
        std::vector<std::string> vecGroups;
        bm->GetGroups(vecGroups);
        for (std::vector<std::string>::const_iterator it = vecGroups.begin()
             ; it != vecGroups.end()
             ; ++it){
            QJsonObject joGroup;

            const std::string sGroupId = *it;
            std::string sGroupName;
            if (bmOk != bm->GetGroupName(sGroupId, sGroupName)){
                continue;
            }
            joGroup["name"] = QString::fromStdString(sGroupName);

            joGroup["creator"] = QString::fromStdString(bm->GetGroupCreator(sGroupId));

            std::vector<std::string> vecMembers;
            bm->GetGroupMembers(sGroupId, vecMembers);
            QJsonArray jaMembers;
            for (std::vector<std::string>::const_iterator it_member = vecMembers.begin()
                 ; it_member != vecMembers.end()
                 ; ++it_member){
                const std::string sMember = *it_member;
                jaMembers.append(((QString::fromStdString(sMember))));
            }
            joGroup["members"] = jaMembers;

            joGroups.insert(QString::fromStdString(sGroupId), joGroup);
        }
        // Subscribes
        std::vector<std::string> vecSubscribes;
        bm->GetSubscribes(vecSubscribes);
        for (std::vector<std::string>::const_iterator it = vecSubscribes.begin()
             ; it != vecSubscribes.end()
             ; ++it){
            jaSubscribes.append(QString::fromStdString(*it));
        }
        // proxy
        do {
            joProxy["ip"] = QString::fromStdString( bm->GetProxyIp());
            joProxy["port"] = (int)bm->GetProxyPort();
            joProxy["user"] = QString::fromStdString(bm->GetProxyUser());
            joProxy["password"] = QString::fromStdString(bm->Encrypt(bm->GetProxyPassword()));
        }while(0);

        // for more readable, instead of `profile'
        joRoot["Profile"] = joProfile;
        joRoot["tx"] = joTx;
        joRoot["rx"] = joRx;
        joRoot["friends"] = joBuddies;
        joRoot["proxy"] = joProxy;
        joRoot["groups"] = joGroups;
        joRoot["subscribes"] = jaSubscribes;
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
        QString qsLoggerPath = GetLogHome();
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
        if (gLoggerHandle != NULL){
            fclose(gLoggerHandle);
            gLoggerHandle = NULL;
        }
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
        if (fout == NULL){
            return ;
        }
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

    /**
     * @brief toMimeTextPlain, convert UTF-8 ByteArray to Part with header as following:
     * Content-Type: text/plain; charset=utf-8
     * Content-Transfer-Encoding: base64
     *
     * @param s
     * @return mime encoded text/plain
     */
    QString toMimeTextPlain(const QString &s)
    {
        QString mime = "Content-Type: text/plain; charset=utf-8\r\n";
        mime += "Content-Transfer-Encoding: base64\r\n";
        mime += "\r\n";
        mime += QString::fromStdString(BitMail::toBase64(s.toStdString()));
        return mime;
    }

    /**
     * @brief toMimeTextHtml, convert UTF-8 ByteArray to Part with header as following:
     * Content-Type: text/html; charset=utf-8
     * Content-Transfer-Encoding: base64
     *
     * @param s
     * @return mime encoded text/html
     */
    QString toMimeTextHtml(const QString & s)
    {
        QString mime = "Content-Type: text/html; charset=utf-8\r\n";
        mime += "Content-Transfer-Encoding: base64\r\n";
        mime += "\r\n";
        mime += QString::fromStdString(BitMail::toBase64(s.toStdString()));
        return mime;
    }

    /**
     * @brief toMimeImage, convert image to Part with header as following:
     * Content-Type: image/png; name=image1.png
     * Content-Disposition: inline; filename=image1.PNG
     * Content-Transfer-Encoding: base64
     * @param img
     * @return
     */
    QString toMimeImage(const QImage &img)
    {
        const char * format = "PNG";

        QString imagePath = BMQTApplication::GetDataHome() + "/" + QUuid::createUuid().toString().remove('{').remove('}') + "." + format;
        img.save(imagePath, format);

        QFileInfo finfo(imagePath);

        QFile imageFile(imagePath);
        if (!imageFile.open(QIODevice::ReadOnly)){
            return "";
        }
        QString hash = QString::fromStdString( QCryptographicHash::hash(imageFile.readAll(), QCryptographicHash::Sha256).toHex().toStdString() );
        imageFile.close();

        QString uniqPath = BMQTApplication::GetDataHome() + "/" + hash + "." + finfo.completeSuffix();
        if (!QFile(uniqPath).exists()){
            if (!imageFile.rename(uniqPath)){
                qDebug() << "Failed to rename to " << uniqPath;
            }
        }else{
            if (!imageFile.remove()){
                qDebug() << "Failed to remove file: " << imagePath;
            }
        }

        return toMimeAttachment(uniqPath, "inline");
    }

    QString toMimeAttachment(const QString & path, const QString & disposition)
    {
        // Ref: http://www.faqs.org/rfcs/rfc1873.html
        // http://www.faqs.org/rfcs/rfc822.html [6]
        /* http://www.faqs.org/rfcs/rfc2111.html
        The URLs take the form

        content-id    = url-addr-spec

        message-id    = url-addr-spec

        url-addr-spec = addr-spec  ; URL encoding of RFC 822 addr-spec

        cid-url       = "cid" ":" content-id

        mid-url       = "mid" ":" message-id [ "/" content-id ]

        Note: in Internet mail messages, the addr-spec in a Content-ID
        [MIME] or Message-ID [822] header are enclosed in angle brackets
        (<>).  Since addr-spec in a Message-ID or Content-ID might contain
        characters not allowed within a URL; any such character (including
        "/", which is reserved within the "mid" scheme) must be hex-
        encoded using the %hh escape mechanism in [URL].
        */

        QMimeDatabase db;
        QString mimeType = db.mimeTypeForFile(path).name();

        QFileInfo finfo(path);

        QFile file(path);

        QString mime = "Content-Type: " + mimeType + "; name=" + finfo.fileName() + "\r\n";
        mime += "Content-Disposition: " + disposition + "; filename=" + finfo.fileName() + "\r\n";
        /* http://www.faqs.org/rfcs/rfc2111.html
        Both message-id and content-id are required to be globally unique.
        That is, no two different messages will ever have the same Message-ID
        addr-spec; no different body parts will ever have the same Content-ID
        addr-spec.  A common technique used by many message systems is to use
        a time and date stamp along with the local host's domain name, e.g.,
        950124.162336@XIson.com.
        */
        /* a weak impl. here wihtout conforming the suggestions,
         * use Hash of content as left part of @, and uniq host as right part
         * */
        mime += "Content-ID: <" + finfo.fileName() + "@bitmail.bitbaba.com>\r\n";
        mime += "Content-Transfer-Encoding: base64\r\n";
        mime += "\r\n";

        if (!file.open(QIODevice::ReadOnly)){
            return "";
        }

        mime += QString::fromStdString(BitMail::toBase64(file.readAll().toStdString()));

        mime += "\r\n";

        file.close();

        return mime;
    }

    /**
     * @brief toMixed, compose a Http MultiPart with some HttpPart(s)
     * Content-Type: multipart/mixed; boundary=Apple-Mail-8188CA7B-642E-466C-B5A3-76C788466E6E
     * Content-Transfer-Encoding: 7bit
     * @param parts, serialized string by above toMimeTextPlain, etc.
     * @return
     */
    QString toMixed(const QStringList & parts)
    {
        // Ref: https://www.ietf.org/rfc/rfc2046.txt [5.1.1]

        QString boundary = QString("BitMail-Boundary-") + QUuid::createUuid().toString().remove('{').remove('}');

        QString mime = "Content-Type: multipart/mixed; boundary=" + boundary + "\r\n";
        mime += "Content-Transfer-Encoding: 7bit\r\n";
        mime += "\r\n";

        for (QStringList::const_iterator it = parts.constBegin(); it != parts.constEnd(); ++it){
            mime += "--";
            mime += boundary;
            mime += "\r\n";
            mime += *it;
            mime += "\r\n";
        }

        mime += "--";
        mime += boundary;
        mime += "--";
        mime += "\r\n\r\n";

        return mime;
    }

    /**
     * @brief toMixed, compose mixed MultiHttpPart from QVariantList
     * @param parts, a list of QVariant
     * @return
     */
    QString toMixed(const QVariantList & vparts)
    {
        QStringList parts;

        for (QVariantList::const_iterator it = vparts.constBegin(); it != vparts.constEnd(); ++it){
            QVariant v = *it;
            QString vtype = QString::fromStdString(v.typeName());
            if (vtype == "QString"){
                parts.append( toMimeTextPlain( v.toString() ) );
            }else if (vtype == "QImage" /*|| vtype == "QIcon" || vtype == "QPixmap" || vtype == "QBitmap"*/){
                parts.append( toMimeImage( qvariant_cast<QImage>(v) ) );
            }else if (vtype == "QFileInfo"){
                QFileInfo finfo = qvariant_cast<QFileInfo>(v);
                parts.append( toMimeAttachment(finfo.absoluteFilePath(), "attachement") );
            }
        }
        return toMixed(parts);
    }

}

