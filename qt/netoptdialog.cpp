#include "netoptdialog.h"
#include "ui_netoptdialog.h"
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
    }while(0); // Network settings

    do {
        m_leBradPort       = findChild<QLineEdit*>("leBradPort");
        //m_leBradPort->setInputMask("00000");

        m_leBradExtUrl     = findChild<QLineEdit*>("leBradExtUrl");
        m_leBradExtUrl->setReadOnly(true);
    }while(0); // Proxy settings

    do {
        m_leProxyIP       = findChild<QLineEdit*>("leProxyIP");
        //m_leProxyIP->setInputMask("000.000.000.000");

        m_leProxyPort     = findChild<QLineEdit*>("leProxyPort");
        //m_leProxyPort->setInputMask("00000");

        m_leProxyLogin    = findChild<QLineEdit*>("leProxyLogin");

        m_leProxyPassword = findChild<QLineEdit*>("leProxyPassword");
    }while(0); // Proxy settings
}

NetOptionDialog::~NetOptionDialog()
{
    delete ui;
}

void NetOptionDialog::on_buttonBox_accepted()
{
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

void NetOptionDialog::BradPort(unsigned short port)
{
    m_leBradPort->setText(QString("%1").arg(port));
}

unsigned short NetOptionDialog::BradPort() const
{
    return m_leBradPort->text().toInt();
}

void NetOptionDialog::BradExtUrl(const QString & exturl)
{
    m_leBradExtUrl->setText(exturl);
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
