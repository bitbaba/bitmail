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
#include <QTimeZone>

#include "logindialog.h"
#include "mainwindow.h"
#include "assistantdialog.h"
#include <bitmailcore/bitmail.h>
#include <qrencode.h>
#include "lock.h"
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
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted){
        qsEmail = loginDialog.GetEmail();
        qsPassphrase = loginDialog.GetPassphrase();
    }else{
        return 0;
    }

    BitMail * bitmail = BitMail::getInst();
    if (bitmail == NULL){
        return 0;
    }

    BitMail::getInst()->SetupLock(new BMLockCraft());

    if (!BMQTApplication::LoadProfile(bitmail, qsEmail, qsPassphrase)){
        BitMail::freeInst();
        return 0;
    }

    MainWindow mainWin(bitmail);

    mainWin.show();

    app.exec();

    BMQTApplication::SaveProfile(bitmail);

    BitMail::freeInst();

    qDebug() << "BitMail quit!";

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
    QString GetResHome()
    {
        QString qsResHome = GetAppHome();
        qsResHome += "/";
        qsResHome += "res";
        QDir qdir(qsResHome);
        if (!qdir.exists()){
            qdir.mkpath(qsResHome);
        }
        return qsResHome;
    }
    QString GetEmResHome()
    {
        QString qsEmHome = GetResHome();
        qsEmHome += "/";
        qsEmHome += "em";
        QDir qdir(qsEmHome);
        if (!qdir.exists()){
            qdir.mkpath(qsEmHome);
        }
        return qsEmHome;
    }
    QString GetImageResHome()
    {
        QString qsImageResHome = GetResHome();
        qsImageResHome += "/";
        qsImageResHome += "image";
        QDir qdir(qsImageResHome);
        if (!qdir.exists()){
            qdir.mkpath(qsImageResHome);
        }
        return qsImageResHome;
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

    QString GetSigTime(const QString &sigtime)
    {
        QString generalTime = "";
        if (sigtime.startsWith("GT:", Qt::CaseInsensitive)){
            generalTime = sigtime.mid(3);
            return QDateTime::fromString(generalTime, Qt::ISODate).toLocalTime().toString();
        }
        else if (sigtime.startsWith("UTC:", Qt::CaseInsensitive)){
            generalTime += sigtime.mid(4);
            QDateTime st = QDateTime::fromString(generalTime, "yyMMddHHmmssZ").addYears(100);
            st.setTimeSpec(Qt::UTC);
            return st.toLocalTime().toString();
        }else{
            return sigtime;
        }
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
        QString qsProfilePath = GetProfilePath(email);
        QFile file(qsProfilePath);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return false;
        }
        file.setTextModeEnabled(true);
        QTextStream in(&file);
        QString qsProfile = in.readAll().toUtf8();
        file.close();
        return bm->Import(passphrase.toStdString(), qsProfile.toStdString());
    }
    bool SaveProfile(BitMail * bm)
    {
        QString qsProfilePath = GetProfilePath(QString::fromStdString(bm->email()));
        QFile file(qsProfilePath);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            return false;
        }
        file.setTextModeEnabled(true);
        QTextStream out(&file);
        QByteArray bytes = QByteArray::fromStdString(bm->Export());
        out << bytes;
        file.close();
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

    QString guessTxUrl(const QString &email)
    {
        if (email.split("@").size() > 1)
            return QString("smtps://smtp.%1/").arg(email.split("@").at(1));
        else
            return "";
    }

    QString guessRxUrl(const QString &email)
    {
        if (email.split("@").size() > 1)
            return QString("imaps://imap.%1/").arg(email.split("@").at(1));
        else
            return "";
    }

    QIcon Iconfy(const QByteArray & b64line)
    {
        if(!b64line.isEmpty()){
            QString tmplogofile = BMQTApplication::GetTempFile(".ico");
            std::string logofiledata = BitMail::fromBase64Line(b64line.toStdString());
            QFile fout(tmplogofile);
            if (fout.open(QIODevice::WriteOnly)){
               fout.setTextModeEnabled(false);
               fout.write(logofiledata.data(), logofiledata.length());
               fout.close();
               return QIcon(tmplogofile);
            }
        }
        return QIcon(BMQTApplication::GetImageResHome() + "/head.png");
    }

    QByteArray StringifyIcon(const QIcon & icon)
    {
        QImage ico = icon.pixmap(48,48).toImage();
        QString icofile = BMQTApplication::GetTempFile(".ico");
        ico.save(icofile);

        QFile fin(icofile);
        if (!fin.open(QIODevice::ReadOnly)){
            return QByteArray();
        }

        fin.setTextModeEnabled(false);
        QByteArray bytes = fin.readAll();
        fin.close();

        std::string b64logo = BitMail::toBase64Line(bytes.toStdString());

        return QByteArray(b64logo.data(), b64logo.length());
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

        QString path = BMQTApplication::GetDataHome() + "/" + hash;
        if (!origFileInfo.completeSuffix().isEmpty()){
           path += ".";
           path += origFileInfo.completeSuffix();
        }
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
        if (mimeType.startsWith("text/", Qt::CaseInsensitive)){
            // Note: transfer plain text file as attachement
            // As, we do not known the charset-encoding.
            mimeType = "application/octet-stream";
        }

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
        QString fileName = finfo.fileName();
        QString mime = "Content-Type: " + mimeType + "; name=" + fileName + "\r\n";
        mime += "Content-Disposition: inline; filename=" + fileName + "\r\n";
        if (false){
            mime += "Content-ID: <" + fileName + ">\r\n";
        }
        mime += "Content-Transfer-Encoding: base64\r\n";
        mime += "\r\n";
        if (!file.open(QIODevice::ReadOnly)){
            return "";
        }
        file.setTextModeEnabled(false);
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

        for (std::vector<std::string>::const_iterator it = parts.begin(); it != parts.end(); ++it){
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
            return qsText;
        }

        if (qsEncoding == "base64" && !qsCharset.isEmpty()){
            content = BitMail::fromBase64(content + "\r\n");
            qsText = QTextCodec::codecForName(qsCharset.toStdString().c_str())->toUnicode(content.data(), content.length());
            return qsText;
        }

        if (qsEncoding == "quoted-printable" && !qsCharset.isEmpty()){
            // ASCII TABLE segment: 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F
            const int hexVal[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15};
            QByteArray temp;
            QString input = QString::fromStdString(content);
            for (int i = 0; i < input.length(); ++i)
            {
                if (input.at(i).toLatin1() == '='
                    && i + 2 < input.length()
                    && ((input.at(i+1).toLatin1() >= '0' && input.at(i+1).toLatin1() <= '9') || (input.at(i+1).toLatin1() >= 'A' && input.at(i+1).toLatin1() <= 'F'))
                    && ((input.at(i+2).toLatin1() >= '0' && input.at(i+2).toLatin1() <= '9') || (input.at(i+2).toLatin1() >= 'A' && input.at(i+2).toLatin1() <= 'F')))
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
            return qsText;
        }

        return "";
    }

    /**
     * @brief fromMimeTextHtml, use QByteArray to store HTML text.
     * @param qsPart
     * @return
     */
    QByteArray fromMimeTextHtml(const QString & qsPart)
    {
        return fromMimeTextPlain(qsPart).toUtf8();
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
            fout.setTextModeEnabled(false);
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

        if (qsMsgType.startsWith("text/plain")){
            varlist.append( BMQTApplication::fromMimeTextPlain(qsMsg) );
        }else if (qsMsgType.startsWith("text/html")){
            varlist.append( BMQTApplication::fromMimeTextHtml(qsMsg) );
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

                if (qsPartType.startsWith("text/plain")){
                    varlist.append( BMQTApplication::fromMimeTextPlain(qsPart) );
                }else if (qsPartType.startsWith("text/html")){
                    varlist.append( BMQTApplication::fromMimeTextHtml(qsPart) );
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

