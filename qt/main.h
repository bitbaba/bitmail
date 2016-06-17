#ifndef MAIN_H
#define MAIN_H

#include <QString>
#include <QStringList>

class BitMail;

namespace BMQTApplication {
    QString GetAppHome();
    QString GetProfileHome();
    QString GetDataHome();
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
}

#endif // MAIN_H

