#ifndef MAIN_H
#define MAIN_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QPixmap>

class BitMail;

namespace BMQTApplication {
    QString GetAppHome();
    QString GetProfileHome();
    QString GetDataHome();
    QString GetTempHome();
    QString GetLogHome();
    QString GetLocaleHome();
    QString GetEmojiHome();
    QString GetLanIp();
    QStringList GetProfiles();
    QString GetProfilePath(const QString & email);
    bool IsValidPofile(const QString & profilePath);
    bool SaveProfile(BitMail * bm);
    bool LoadProfile(BitMail * bm, const QString & email, const QString & passphrase);
    bool InitLogger();
    FILE * GetLogger();
    void FlushLogger();
    bool CloseLogger();
    void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);


    /**
     * @brief toQrImage, convert a string to qr-encoded image
     * @param s
     * @return QImage
     */
    QPixmap toQrImage(const QString & s);

    /**
     * @brief toMimeTextPlain, convert UTF-8 ByteArray to Part with header as following:
     * Content-Type: text/plain; charset=utf-8
     * Content-Transfer-Encoding: base64
     *
     * @param s
     * @return mime encoded text/plain
     */
    QString toMimeTextPlain(const QString & s);

    /**
     * @brief toMimeTextHtml, convert UTF-8 ByteArray to Part with header as following:
     * Content-Type: text/html; charset=utf-8
     * Content-Transfer-Encoding: base64
     *
     * @param s
     * @return mime encoded text/html
     */
    QString toMimeTextHtml(const QString & s);

    /**
     * @brief toMimeImage, convert image to Part with header as following:
     * Content-Type: image/png; name=image1.png
     * Content-Disposition: inline; filename=image1.PNG
     * Content-Transfer-Encoding: base64
     * @param img
     * @return
     */
    QString toMimeImage(const QImage & img);

    /**
     * @brief toMimeAttachment, general purpose routine to process attachement
     * @param path
     * @return
     */
    QString toMimeAttachment(const QString & path);

    /**
     * @brief toMixed, compose mixed MultiHttpPart from QVariantList
     * @param parts, a list of QVariant
     * @return
     */
    QString toMixed(const QVariantList & parts);

    /**
     * @brief toMixed, compose a Http MultiPart with some HttpPart(s)
     * @param parts, serialized string by above toMimeTextPlain, etc.
     * @return
     */
    QString toMixed(const QStringList & parts);

    /**
     * TODO: multipart/mixed parsing and renderring
     *
     */
    QStringList fromMixed(const QString & mparts);

    QString fromMimeTextPlain(const QString & part);

    QString fromMimeTextHtml(const QString & part);

    QString fromMimeAttachemnt(const QString & part);

    QImage fromMimeImage(const QString & part);

    QVariantList fromMime(const QString & mime);

    // Utilities
    QStringList toQStringList(const std::vector<std::string> & vec);

    std::vector<std::string> toStdStringList(const QStringList & qsl);
}

#endif // MAIN_H

