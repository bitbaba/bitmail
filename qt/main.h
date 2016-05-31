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
    bool IsValidPofile(const QString & profile);
    bool SaveProfile(BitMail * bm, const QString &fileName);
    void LoadProfile(BitMail * bm, const QString &fileName, const QString & passphrase);
}

#endif // MAIN_H

