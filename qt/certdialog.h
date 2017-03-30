#ifndef CERTDIALOG_H
#define CERTDIALOG_H

#include <QDialog>


class QLineEdit;
class QLabel;
class QPlainTextEdit;
class BitMail;

namespace Ui {
class CertDialog;
}

class CertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CertDialog( BitMail * bm, QWidget *parent = 0);
    ~CertDialog();

    void SetEmail(const QString & e);
    QString GetEmail() const;

    void SetNick(const QString & n);
    QString GetNick() const;

    void SetCertID(const QString & cid);
    QString GetCertID() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_btnSubscribe_clicked();

private:
    Ui::CertDialog *ui;
    BitMail   *m_bitmail;
    QLineEdit *m_leEmail;
    QLineEdit *m_leNick;
    QLineEdit *m_leCertID;
    QLabel *m_lbQrCode;
    QPlainTextEdit *m_txtCert;
};

#endif // CERTDIALOG_H
