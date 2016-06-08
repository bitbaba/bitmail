#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>

#include <bitmailcore/bitmail.h>

class QComboBox;
class QLineEdit;

namespace Ui {
class ProxyDialog;
}

class ProxyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyDialog(QWidget *parent = 0);
    ~ProxyDialog();

    void SetProxyType(ProxyType pt);
    ProxyType GetProxyType() const;

    void SetProxyIp(const QString & ip);
    QString GetProxyIp() const;

    void SetProxyPort(const QString & port);
    QString GetProxyPort() const;

    void SetAuthUser(const QString & user);
    QString GetAuthUser();

    void SetAuthPassword(const QString & password);
    QString GetAuthPassword() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ProxyDialog *ui;

    QComboBox * m_cbProxyType;
    QLineEdit * m_leProxyIp;
    QLineEdit * m_leProxyPort;
    QLineEdit * m_leAuthUser;
    QLineEdit * m_leAuthPassword;
};

#endif // PROXYDIALOG_H
