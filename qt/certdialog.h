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

    void comment(const QString & cmt);
    QString comment() const;

    void qrImage(const QPixmap & p);

    void head(const QIcon & hd);

private slots:
    void on_btnSetComment_clicked();

    void on_btnLogo_clicked();

    void on_btnRemove_clicked();

    void on_btnExportCert_clicked();

    void on_btnExportKey_clicked();

private:
    Ui::CertDialog *ui;
    BitMail   *m_bitmail;
    QLineEdit *m_leEmail;
    QLineEdit *m_leNick;
    QLineEdit *m_leCertID;
    QLineEdit *m_leComment;
    QLabel *m_lbQrCode;
    QPlainTextEdit *m_txtCert;
    QPushButton *m_btnLogo;

signals:
    void friendChanged();
};

#endif // CERTDIALOG_H
