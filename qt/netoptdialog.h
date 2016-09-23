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

    void BradPort(unsigned short port);
    unsigned short BradPort() const;

    void BradExtUrl(const QString & exturl);
    QString BradExtUrl() const;

    void SetProxyIP(const QString & ip);
    QString GetProxyIP() const;

    void SetProxyPort(unsigned short port);
    unsigned short GetProxyPort() const;

    void SetProxyLogin(const QString & login);
    QString GetProxyLogin() const;

    void SetProxyPassword(const QString & pass);
    QString GetProxyPassword() const;

private:
    QLineEdit *m_leSmtpUrl;    
    QLineEdit *m_leImapUrl;
    QLineEdit *m_leLogin;
    QLineEdit *m_lePassword;

    QLineEdit *m_leBradPort;
    QLineEdit *m_leBradExtUrl;

    QLineEdit *m_leProxyIP;
    QLineEdit *m_leProxyPort;
    QLineEdit *m_leProxyLogin;
    QLineEdit *m_leProxyPassword;
};

#endif // NETOPTIONDIALOG_H
