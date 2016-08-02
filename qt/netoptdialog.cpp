#include "netoptdialog.h"
#include "ui_netoptdialog.h"
#include "main.h"
#include <bitmailcore/bitmail.h>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDebug>

NetOptionDialog::NetOptionDialog(QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::NetOptionDialog)
{
    ui->setupUi(this);

    do {
        m_leSmtpUrl      = findChild<QLineEdit*>("leSmtpUrl");
        m_leSmtpUrl->setPlaceholderText("smtps://smtp.somesite.net/");
        m_leSmtpUrl->setValidator(new QRegExpValidator(QRegExp("smtps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leSmtpUrl));
        m_leImapUrl      = findChild<QLineEdit*>("leImapUrl");
        m_leImapUrl->setPlaceholderText("imaps://imap.somesite.net/");
        m_leImapUrl->setValidator(new QRegExpValidator(QRegExp("imaps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leImapUrl));
        m_leLogin        = findChild<QLineEdit*>("leLogin");
        m_lePassword     = findChild<QLineEdit*>("lePassword");
        m_cbEnableUPnP   = findChild<QCheckBox*>("cbUPnP");
    }while(0); // Network settings

    do {
        m_cbProxyEnable   = findChild<QCheckBox*>("cbProxyEnable");

        m_leProxyIP       = findChild<QLineEdit*>("leProxyIP");
        m_leProxyIP->setInputMask("000.000.000.000");
        m_leProxyIP->setEnabled(false);

        m_leProxyPort     = findChild<QLineEdit*>("leProxyPort");
        m_leProxyPort->setInputMask("00000");
        m_leProxyPort->setEnabled(false);

        m_leProxyLogin    = findChild<QLineEdit*>("leProxyLogin");
        m_leProxyLogin->setEnabled(false);

        m_leProxyPassword = findChild<QLineEdit*>("leProxyPassword");
        m_leProxyPassword->setEnabled(false);

        m_cbRemoteDNS     = findChild<QCheckBox*>("cbRemoteDNS");
        m_cbRemoteDNS->setEnabled(false);

        // Note: fowlling state setup will NOT trigger slot
        m_cbProxyEnable->setCheckState(Qt::Unchecked);
    }while(0); // Proxy settings
}

NetOptionDialog::~NetOptionDialog()
{
    delete ui;
}

void NetOptionDialog::on_buttonBox_accepted()
{
    QString qsSmtpUrl = GetSmtpUrl();
    QString qsSmtpLogin = GetSmtpLogin();
    QString qsSmtpPassword = GetSmtpPassword();

    QString qsImapUrl = GetImapUrl();
    QString qsImapLogin = GetImapLogin();
    QString qsImapPassword = GetImapPassword();

    BitMail * bm = new BitMail();

    bm->InitNetwork(qsSmtpUrl.toStdString()
                    , qsSmtpLogin.toStdString()
                    , qsSmtpPassword.toStdString()
                    , qsImapUrl.toStdString()
                    , qsImapLogin.toStdString()
                    , qsImapPassword.toStdString());

    bm->EnableProxy(GetProxyEnable());
    if (bm->EnableProxy()){
        bm->SetProxyIp(GetProxyIP().toStdString());
        bm->SetProxyPort(GetProxyPort());
        bm->SetProxyUser(GetProxyLogin().toStdString());
        bm->SetProxyPassword(GetProxyPassword().toStdString());
        bm->RemoteDNS(GetRemoteDNS());
    }

    BMQTApplication::SaveProfile(bm);

    delete bm; bm = NULL;

    return ;
}

void NetOptionDialog::on_buttonBox_rejected()
{
    return ;
}

void NetOptionDialog::SetSmtpUrl(const QString & u)
{
    (void)u;
    m_leSmtpUrl->setText(u);
}

QString NetOptionDialog::GetSmtpUrl() const
{
    return m_leSmtpUrl->text();
}

void NetOptionDialog::SetSmtpLogin(const QString & l)
{
    m_leLogin->setText(l);
}

QString NetOptionDialog::GetSmtpLogin() const
{
    return m_leLogin->text();
}

void NetOptionDialog::SetSmtpPassword(const QString & p)
{
    m_lePassword->setText(p);
}

QString NetOptionDialog::GetSmtpPassword() const
{
    return m_lePassword->text();
}

void NetOptionDialog::SetImapUrl(const QString & u)
{
    m_leImapUrl->setText(u);
}

QString NetOptionDialog::GetImapUrl() const
{
    return m_leImapUrl->text();
}

void NetOptionDialog::SetImapLogin(const QString & l)
{
    m_leLogin->setText(l);
}

QString NetOptionDialog::GetImapLogin() const
{
    return m_leLogin->text();
}

void NetOptionDialog::SetImapPassword(const QString & p)
{
    m_lePassword->setText(p);
}

QString NetOptionDialog::GetImapPassword() const
{
    return m_lePassword->text();
}

void NetOptionDialog::on_cbProxyEnable_clicked()
{

}

void NetOptionDialog::on_cbProxyEnable_stateChanged(int arg1)
{
    m_leProxyIP->setEnabled(arg1 == Qt::Checked);
    m_leProxyPort->setEnabled(arg1 == Qt::Checked);
    m_leProxyLogin->setEnabled(arg1 == Qt::Checked);
    m_leProxyPassword->setEnabled(arg1 == Qt::Checked);
    m_cbRemoteDNS->setEnabled(arg1 == Qt::Checked);
}

void NetOptionDialog::on_cbUPnP_stateChanged(int arg1)
{
    (void) arg1;
}

void NetOptionDialog::SetEnableUPnP(bool enable)
{
    m_cbEnableUPnP->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

bool NetOptionDialog::GetEnableUPnP() const
{
    return m_cbEnableUPnP->checkState() == Qt::Checked;
}

void NetOptionDialog::SetProxyEnable(bool enable)
{
    m_cbProxyEnable->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
    //on_cbProxyEnable_stateChanged(enable ? Qt::Checked : Qt::Unchecke);
}

bool NetOptionDialog::GetProxyEnable() const
{
    return m_cbProxyEnable->checkState() == Qt::Checked;
}

void NetOptionDialog::SetProxyIP(const QString & ip)
{
    m_leProxyIP->setText(ip);
}

QString NetOptionDialog::GetProxyIP() const
{
    return m_leProxyIP->text();
}

void NetOptionDialog::SetProxyPort(unsigned short port)
{
    m_leProxyPort->setText(QString("%1").arg(port));
}

unsigned short NetOptionDialog::GetProxyPort() const
{
    return m_leProxyPort->text().toUShort();
}

void NetOptionDialog::SetProxyLogin(const QString & login)
{
    m_leProxyLogin->setText(login);
}

QString NetOptionDialog::GetProxyLogin() const
{
    return m_leProxyLogin->text();
}

void NetOptionDialog::SetProxyPassword(const QString & pass)
{
    m_leProxyPassword->setText(pass);
}

QString NetOptionDialog::GetProxyPassword() const
{
    return m_leProxyPassword->text();
}

void NetOptionDialog::SetRemoteDNS(bool enable)
{
    m_cbRemoteDNS->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

bool NetOptionDialog::GetRemoteDNS() const
{
    return m_cbRemoteDNS->checkState() == Qt::Checked;
}
