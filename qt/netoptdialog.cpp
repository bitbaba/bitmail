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
        m_leImapUrl      = findChild<QLineEdit*>("leImapUrl");
        m_leLogin        = findChild<QLineEdit*>("leLogin");
        m_lePassword     = findChild<QLineEdit*>("lePassword");
        m_leSocks5       = findChild<QLineEdit*>("leSocks5");
    }while(0);

    m_leSmtpUrl->setPlaceholderText("smtps://smtp.somesite.net/");
    m_leSmtpUrl->setValidator(new QRegExpValidator(QRegExp("smtps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leSmtpUrl));
    m_leImapUrl->setPlaceholderText("imaps://imap.somesite.net/");
    m_leImapUrl->setValidator(new QRegExpValidator(QRegExp("imaps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leImapUrl));
    m_leSocks5->setPlaceholderText("socks5://login:password@127.0.0.1:1080/");
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

void NetOptionDialog::socks5(const QString &proxy)
{
    m_leSocks5->setText(proxy);
}

QString NetOptionDialog::socks5() const
{
    return m_leSocks5->text();
}
