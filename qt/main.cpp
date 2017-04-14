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
#include <QPainter>
#include <QMessageBox>
#include <QStyleFactory>

#include "optiondialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "assistantdialog.h"
#include <bitmailcore/bitmail.h>
#include <qrencode.h>
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

    // Skin
    QStringList styles = QStyleFactory::keys();
    qDebug() << styles.join(";");
    app.setStyle("Fusion");

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
        QMessageBox::warning(NULL, QString("Login"), QString("Failed to load profile!"));
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
        QDir qdir(qsProfileHome);
        if (!qdir.exists()){
            qdir.mkpath(qsProfileHome);
        }
        return qsProfileHome;
    }
    QString GetDataHome()
    {
        QString qsDataHome = QDir::homePath();
        qsDataHome += "/";
        qsDataHome += "bitmail";
        qsDataHome += "/";
        qsDataHome += "data";
        QDir qdir(qsDataHome);
        if (!qdir.exists()){
            qdir.mkpath(qsDataHome);
        }
        return qsDataHome;
    }
    QString GetTempHome()
    {
        QString qsTempHome = QDir::homePath();
        qsTempHome += "/";
        qsTempHome += "bitmail";
        qsTempHome += "/";
        qsTempHome += "temp";
        QDir qdir(qsTempHome);
        if (!qdir.exists()){
            qdir.mkpath(qsTempHome);
        }
        return qsTempHome;
    }
    QString GetTempFile(const QString & dotsuffix)
    {
        QString qsTempFile = GetTempHome();
        qsTempFile += "/";
        qsTempFile += QUuid::createUuid().toString().remove("{").remove("}");
        qsTempFile += dotsuffix;
        return qsTempFile;
    }
    QString GetLogHome()
    {
        QString qsLogHome = QDir::homePath();
        qsLogHome += "/";
        qsLogHome += "bitmail";
        qsLogHome += "/";
        qsLogHome += "log";
        QDir qdir(qsLogHome);
        if (!qdir.exists()){
            qdir.mkpath(qsLogHome);
        }
        return qsLogHome;
    }
    QString GetLocaleHome()
    {
        QString qsLocaleHome = GetAppHome();
        qsLocaleHome += "/";
        qsLocaleHome += "locale";
        QDir qdir(qsLocaleHome);
        if (!qdir.exists()){
            qdir.mkpath(qsLocaleHome);
        }
        return qsLocaleHome;
    }
    QString GetEmojiHome()
    {
        QString qsEmojiHome = GetAppHome();
        qsEmojiHome += "/";
        qsEmojiHome += "emoji";
        QDir qdir(qsEmojiHome);
        if (!qdir.exists()){
            qdir.mkpath(qsEmojiHome);
        }
        return qsEmojiHome;
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
    QString GetExeSuffix()
    {
        #if defined(WIN32)
            return ".exe";
        #else
            return "";
        #endif
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
        // the file may contains multi-bytes encoding charactors
        // so use UTF-8 to transform.
        jdoc = QJsonDocument::fromJson(in.readAll().toUtf8());
        QJsonObject joRoot = jdoc.object();
        QJsonObject joProfile;
        QJsonObject joTx;
        QJsonObject joRx;
        QJsonObject joBuddies;
        QJsonArray jaGroups;
        QJsonObject joSessNames;
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
            }
        }

        do {
            if (!joRoot.contains("groups")){
                break;
            }
            jaGroups = joRoot["groups"].toArray();
            for (QJsonArray::const_iterator it = jaGroups.constBegin()
                 ; it != jaGroups.constEnd()
                 ; it ++)
            {
                QString memberlist = (*it).toString();
                bm->AddGroup(memberlist.toStdString());
            }
        }while(0);

        do {
            if (!joRoot.contains("sessionNames")){
                break;
            }
            joSessNames = joRoot["sessionNames"].toObject();
            for (QJsonObject::const_iterator it = joSessNames.constBegin()
                 ; it != joSessNames.constEnd()
                 ; it ++)
            {
                QString sessKey = it.key();
                QString sessName = it.value().toString();
                bm->sessionName(sessKey.toStdString(), sessName.toStdString());
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
        QJsonObject joBuddies;
        QJsonArray jaGroups;     // Group chatting
        QJsonObject joSessNames;
        QJsonObject joProxy;
        // Profile
        joProfile["email"] = QString::fromStdString(bm->GetEmail());
        joProfile["nick"] = QString::fromStdString(bm->GetNick());
        joProfile["key"] = QString::fromStdString(bm->GetKey());
        joProfile["cert"] = QString::fromStdString(bm->GetCert());
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
            QString qsEmail = QString::fromStdString(*it);
            joBuddies.insert(qsEmail, joBuddy);
        }
        // Groups
        std::vector<std::string> vecGroups;
        bm->GetGroups(vecGroups);
        for (std::vector<std::string>::const_iterator it = vecGroups.begin(); it != vecGroups.end(); ++it)
        {
            jaGroups.append(QString::fromStdString(*it));
        }
        // Session Names
        std::map<std::string, std::string> sessNames = bm->sessNames();
        for (std::map<std::string, std::string>::const_iterator it = sessNames.begin(); it != sessNames.end(); ++it){
            QString sessKey  = QString::fromStdString(it->first);
            QString sessName = QString::fromStdString(it->second);
            joSessNames[sessKey] = sessName;
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
        joRoot["groups"] = jaGroups;
        joRoot["sessionNames"] = joSessNames;
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
     * @brief toQrImage, convert a string to qr-encoded image
     * @param s
     * @return QImage
     */
    QPixmap toQrImage(const QString & s)
    {
        QRcode * qrcode = ::QRcode_encodeString(s.toStdString().c_str(), 0/*auto-choose-minimum*/, QR_ECLEVEL_L, QR_MODE_8, 1);
        const int SCALE = 3;
        QImage image(qrcode->width * SCALE, qrcode->width * SCALE, QImage::Format_Mono);
        QPainter painter(&image);
        QColor bgrdWhite(Qt::white);
        painter.setBrush(bgrdWhite);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, image.width(), image.height());
        QColor fgrdBlack(Qt::black);
        painter.setBrush(fgrdBlack);
        for (int row = 0; row < qrcode->width; row++){
            for (int col = 0; col < qrcode->width; col++){
                unsigned char dot = qrcode->data[row * qrcode->width + col];
                if (dot & 0x01){
                    QRect rect(col * SCALE, row * SCALE, SCALE, SCALE);
                    painter.drawRect(rect);
                }
            }
        }
        ::QRcode_free(qrcode);
        return QPixmap::fromImage(image);
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

        QString imagePath = BMQTApplication::GetTempHome() + "/" + QUuid::createUuid().toString().remove('{').remove('}') + "." + format;
        img.save(imagePath, format);

        QString mime = toMimeAttachment(imagePath);

        QFile(imagePath).remove();

        return mime;
    }

    QString toMimeAttachment(const QString & orig_path)
    {
        QFile origFile(orig_path);
        QFileInfo origFileInfo(orig_path);
        if (!origFile.open(QIODevice::ReadOnly)){
            return "";
        }
        QString hash = QString::fromStdString( QCryptographicHash::hash(origFile.readAll(), QCryptographicHash::Sha256).toHex().toStdString() );
        origFile.close();

        QString path = BMQTApplication::GetDataHome() + "/" + hash + "." + origFileInfo.completeSuffix();
        QFile file(path);
        QFileInfo finfo(path);

        origFile.copy(path);
        if (!origFile.exists() || !origFile.size()){
            return "";
        }

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

        /* http://www.faqs.org/rfcs/rfc2111.html
        Both message-id and content-id are required to be globally unique.
        That is, no two different messages will ever have the same Message-ID
        addr-spec; no different body parts will ever have the same Content-ID
        addr-spec.  A common technique used by many message systems is to use
        a time and date stamp along with the local host's domain name, e.g.,
        950124.162336@XIson.com.
        */

        /* a weak impl. here wihtout conforming the suggestions,
         * use Hash of content as left part of @, and left the left part void.
        */

        /**
         * @brief if 'Content-ID:' in use, the mobile phone client will promot user
         * to `tap to download'.
         * So, if direct display is needed, remove the header `Content-ID:'
         */
        QString mime = "Content-Type: " + mimeType + "; name=" + finfo.fileName() + "\r\n";
        mime += "Content-Disposition: inline; filename=" + finfo.fileName() + "\r\n";
        if (false){
            mime += "Content-ID: <" + finfo.fileName() + ">\r\n";
        }
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
                parts.append( toMimeAttachment(finfo.absoluteFilePath()) );
            }
        }
        return toMixed(parts);
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

    QStringList fromMixed(const QString & mparts)
    {
        QStringList qslParts;

        std::vector<std::string> parts;
        BitMail::splitMultiparts(mparts.toStdString(), parts);

        for (std::vector<std::string>::const_iterator it = parts.cbegin(); it != parts.cend(); ++it){
            std::string part = (*it);
            if (false){
                qDebug() << QString::fromStdString(BitMail::partType(part));
                qDebug() << QString::fromStdString(BitMail::partEncoding(part));
                qDebug() << QString::fromStdString(BitMail::partParam(part, "content-type", "charset"));
                qDebug() << QString::fromStdString(BitMail::partParam(part, "content-type", "name"));
                qDebug() << QString::fromStdString(BitMail::partParam(part, "content-disposition", "filename"));
                qDebug() << QString::fromStdString(BitMail::partContent(part));
            }
            qslParts.append(QString::fromStdString(part));
        }
        return qslParts;
    }

    QString fromMimeTextPlain(const QString & qsPart)
    {
        QString qsText;

        std::string part = qsPart.toStdString();

        QString qsCharset  = QString::fromStdString(BitMail::partParam(part, "content-type", "charset"));

        QString qsEncoding = QString::fromStdString(BitMail::partEncoding(part));

        std::string content = BitMail::partContent(part);

        if (qsEncoding == "7bit"){ // latin1
            qsText = QString::fromStdString(content);
        }else if (qsEncoding == "base64"){
            content = BitMail::fromBase64(content + "\r\n");
            qsText = QTextCodec::codecForName(qsCharset.toStdString().c_str())->toUnicode(content.data(), content.length());
        }else if (qsEncoding == "quoted-printable"){
            // ASCII TABLE segment: 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F
            const int hexVal[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15};
            QByteArray temp;
            QString input = QString::fromStdString(content);
            for (int i = 0; i < input.length(); ++i)
            {
                if (input.at(i).toLatin1() == '=')
                {
                    char ch = (hexVal[input.at(++i).toLatin1() - '0'] << 4);
                    ch += hexVal[input.at(++i).toLatin1() - '0'];
                    temp.append( ch );
                }
                else
                {
                    temp.append(input.at(i).toLatin1());
                }
            }
            qsText = QTextCodec::codecForName(qsCharset.toStdString().c_str())->toUnicode(temp);
        }else{
            qDebug() << "Unsupported text decoding";
        }
        return qsText;
    }

    QString fromMimeTextHtml(const QString & qsPart)
    {
        return fromMimeTextPlain(qsPart);
    }

    QString fromMimeAttachemnt(const QString & qsPart)
    {
        QString filepath;

        std::string part = qsPart.toStdString();

        QString qsAttachBaseName  = QString::fromStdString(BitMail::partParam(part, "content-disposition", "filename"));
        if (qsAttachBaseName.isEmpty()){
            qDebug() << "compatible issue";
            return filepath;
        }

        QString qsEncoding = QString::fromStdString(BitMail::partEncoding(part));
        if (qsEncoding.compare("base64", Qt::CaseInsensitive)){
            qDebug() << "unsupported encoding except base64.";
            return filepath;
        }

        std::string content = BitMail::fromBase64(BitMail::partContent(part));

        QString hash = QString::fromStdString( QCryptographicHash::hash(QByteArray::fromStdString(content), QCryptographicHash::Sha256).toHex().toStdString() );
        filepath = BMQTApplication::GetDataHome() + "/" + hash + "." + QFileInfo(qsAttachBaseName).completeSuffix();

        QFile fout(filepath);
        if (fout.exists()){
            fout.remove();
        }

        if (fout.open(QIODevice::WriteOnly)){
            fout.write(content.data(), content.length());
            fout.close();
        }

        return filepath;
    }

    QImage fromMimeImage(const QString & part)
    {
        QString filePath = fromMimeAttachemnt(part);
        QImage image(filePath);
        return image;
    }

    QVariantList fromMime(const QString & qsMsg)
    {
        QVariantList varlist;

        QString qsMsgType = QString::fromStdString(BitMail::partType(qsMsg.toStdString()));

        if (qsMsgType.startsWith("text/")){
            varlist.append( BMQTApplication::fromMimeTextPlain(qsMsg) );
        }
        else if (qsMsgType.startsWith("image/")){
            QImage image = BMQTApplication::fromMimeImage(qsMsg);
            varlist.append(image);
        }
        else if (qsMsgType.startsWith("multipart/")){
            QStringList qslParts = BMQTApplication::fromMixed(qsMsg);
            for (QStringList::const_iterator it = qslParts.constBegin(); it != qslParts.constEnd(); ++it)
            {
                QString qsPart = *it;

                QString qsPartType = QString::fromStdString(BitMail::partType(qsPart.toStdString()));

                if (qsPartType.startsWith("text/")){
                    varlist.append( BMQTApplication::fromMimeTextPlain(qsPart) );
                }else if (qsPartType.startsWith("image/")){
                    QImage image = BMQTApplication::fromMimeImage(qsPart);
                    varlist.append(image);
                }else if (qsPartType.startsWith("multipart/")){
                    varlist += fromMime(qsPart);
                }else{
                    QString filePath = BMQTApplication::fromMimeAttachemnt(qsPart);
                    varlist.append(QVariant::fromValue(QFileInfo(filePath)));
                }
            }
        }
        else{
            QString filePath = BMQTApplication::fromMimeAttachemnt(qsMsg);
            varlist.append(QVariant::fromValue(QFileInfo(filePath)));
        }

        return varlist;
    }

    QStringList toQStringList(const std::vector<std::string> & vec)
    {
        QStringList qsl;
        for(std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it){
            qsl.append(QString::fromStdString(*it));
        }
        return qsl;
    }

    std::vector<std::string> toStdStringList(const QStringList & qsl)
    {
        std::vector<std::string> v;
        for(QStringList::const_iterator it = qsl.cbegin(); it != qsl.cend(); ++it){
            v.push_back(it->toStdString());
        }
        return v;
    }
}

