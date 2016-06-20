#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QSpinBox;

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(bool fNew = false, QWidget *parent = 0);
    ~OptionDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_cbProxyEnable_clicked();

    void on_cbProxyEnable_stateChanged(int arg1);

    void on_cbUPnP_stateChanged(int arg1);

private:
    Ui::OptionDialog *ui;
public:
    void SetEmail(const QString & e);
    QString GetEmail() const;

    void SetNick(const QString & n);
    QString GetNick() const;

    void SetPassphrase(const QString & p);
    QString GetPassphrase() const;

    void SetBits(const int & n);
    int GetBits() const;

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

    void SetEnableUPnP(bool enable);
    bool GetEnableUPnP() const;

    void SetProxyEnable(bool enable);
    bool GetProxyEnable() const;

    void SetProxyIP(const QString & ip);
    QString GetProxyIP() const;

    void SetProxyPort(unsigned short port);
    unsigned short GetProxyPort() const;

    void SetProxyLogin(const QString & login);
    QString GetProxyLogin() const;

    void SetProxyPassword(const QString & pass);
    QString GetProxyPassword() const;

    void SetRemoteDNS(bool enable);
    bool GetRemoteDNS() const;

private:
    QLineEdit *m_leEmail;
    QLineEdit *m_leNick;
    QLineEdit *m_lePassphrase;
    QSpinBox  *m_sbBits;

    QLineEdit *m_leSmtpUrl;    
    QLineEdit *m_leImapUrl;
    QLineEdit *m_leLogin;
    QLineEdit *m_lePassword;
    QCheckBox *m_cbEnableUPnP;

    QCheckBox *m_cbProxyEnable;
    QLineEdit *m_leProxyIP;
    QLineEdit *m_leProxyPort;
    QLineEdit *m_leProxyLogin;
    QLineEdit *m_leProxyPassword;
    QCheckBox *m_cbRemoteDNS;

private:
    bool       m_fNew;
};

#endif // OPTIONDIALOG_H
