#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>

class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class BitMail;
class QTextEdit;

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MessageDialog(BitMail * btimail, QWidget *parent = 0);
    ~MessageDialog();

    void SetFrom(const QString & qsFrom);
    QString GetFrom() const;

    void SetCertID(const QString & qsCertID);
    QString GetCertID() const;

    void SetMessage(const QString & qsMessage);
    QString GetMessage() const;

    void SetCert(const QString & qsCert);
    QString GetCert() const;

    void EnableMakeFriend(bool fEnable);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_btnMakeFriend_clicked();

signals:
    void signalAddFriend(const QString & email);

private:
    Ui::MessageDialog *ui;
    BitMail * m_bitmail;

    QLineEdit * m_leFrom;
    QLineEdit * m_leCertID;
    QTextEdit * m_ptxtMessage;
    QPlainTextEdit * m_ptxtCert;
    QPushButton * m_btnMakeFriend;
};
#endif // MESSAGEDIALOG_H
