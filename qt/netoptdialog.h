#ifndef NETOPTIONDIALOG_H
#define NETOPTIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QSpinBox;

namespace Ui {
class NetOptionDialog;
}

class NetOptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetOptionDialog(QWidget *parent = 0);
    ~NetOptionDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::NetOptionDialog *ui;
public:
    void SetSmtpUrl(const QString & u);
    QString GetSmtpUrl() const;

    void SetSmtpLogin(const QString & l);
    QString GetSmtpLogin() const;

    void SetSmtpPassword(const QString & p);
    QString GetSmtpPassword() const;

    void SetImapUrl(const QString & u);
    QString GetImapUrl() const;

    void SetImapLogin(const QString & l);
    QString GetImapLogin() const;

    void SetImapPassword(const QString & p);
    QString GetImapPassword() const;

    void socks5(const QString & proxy);
    QString socks5() const;

private:
    QLineEdit *m_leSmtpUrl;    
    QLineEdit *m_leImapUrl;
    QLineEdit *m_leLogin;
    QLineEdit *m_lePassword;
    QLineEdit *m_leSocks5;
};

#endif // NETOPTIONDIALOG_H
