#include "optiondialog.h"
#include "ui_optiondialog.h"
#include "main.h"
#include <bitmailcore/bitmail.h>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDebug>

OptionDialog::OptionDialog(bool fNew, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::OptionDialog)
    , m_fNew(fNew)
{
    ui->setupUi(this);

    do {
        m_leEmail        = findChild<QLineEdit*>("leEmail");
        if (!m_fNew) m_leEmail->setEnabled(false);
        else m_leEmail->setPlaceholderText("someone@somesite.net");
        m_leEmail->setValidator(new QRegExpValidator(QRegExp("[_a-zA-Z0-9.]+@[a-zA-Z0-9.]+"), m_leEmail));
        m_leNick         = findChild<QLineEdit*>("leNick");
        if (!m_fNew) m_leNick->setEnabled(false);
        m_sbBits         = findChild<QSpinBox*>("sbBits");
        if (!m_fNew) m_sbBits->setEnabled(false);
        m_lePassphrase   = findChild<QLineEdit*>("lePassphrase");
    }while(0);//Profile settings

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

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_buttonBox_accepted()
{
    if (!m_fNew){
        return ;
    }

    QString qsEmail = GetEmail();
    QString qsNick = GetNick();
    QString qsPassphrase = GetPassphrase();
    int nBits = GetBits();

    QString qsSmtpUrl = GetSmtpUrl();
    QString qsSmtpLogin = GetSmtpLogin();
    QString qsSmtpPassword = GetSmtpPassword();

    QString qsImapUrl = GetImapUrl();
    QString qsImapLogin = GetImapLogin();
    QString qsImapPassword = GetImapPassword();

    BitMail * bm = new BitMail();

    bm->CreateProfile(qsNick.toStdString()
                      , qsEmail.toStdString()
                      , qsPassphrase.toStdString()
                      , nBits);

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

void OptionDialog::on_buttonBox_rejected()
{
    return ;
}

void OptionDialog::SetEmail(const QString & e)
{
    (void)e;
    m_leEmail->setText(e);
}

QString OptionDialog::GetEmail() const
{
    return m_leEmail->text();
}

void OptionDialog::SetNick(const QString & n)
{
    (void)n;
    m_leNick->setText(n);
}

QString OptionDialog::GetNick() const
{
    return m_leNick->text();
}

void OptionDialog::SetPassphrase(const QString & p)
{
    (void)p;
    m_lePassphrase->setText(p);
}

QString OptionDialog::GetPassphrase() const
{
    return m_lePassphrase->text();
}

void OptionDialog::SetBits(const int &n)
{
    /**
    * there is no alignment limit on `openssl genrsa [bits]'
    * e.g. openssl genrsa 1234
    */
    m_sbBits->setValue((n + m_sbBits->singleStep() - 1)/m_sbBits->singleStep() * m_sbBits->singleStep());
}

int OptionDialog::GetBits() const
{
    return m_sbBits->value();
}

void OptionDialog::SetSmtpUrl(const QString & u)
{
    (void)u;
    m_leSmtpUrl->setText(u);
}

QString OptionDialog::GetSmtpUrl() const
{
    return m_leSmtpUrl->text();
}

void OptionDialog::SetSmtpLogin(const QString & l)
{
    m_leLogin->setText(l);
}

QString OptionDialog::GetSmtpLogin() const
{
    return m_leLogin->text();
}

void OptionDialog::SetSmtpPassword(const QString & p)
{
    m_lePassword->setText(p);
}

QString OptionDialog::GetSmtpPassword() const
{
    return m_lePassword->text();
}

void OptionDialog::SetImapUrl(const QString & u)
{
    m_leImapUrl->setText(u);
}

QString OptionDialog::GetImapUrl() const
{
    return m_leImapUrl->text();
}

void OptionDialog::SetImapLogin(const QString & l)
{
    m_leLogin->setText(l);
}

QString OptionDialog::GetImapLogin() const
{
    return m_leLogin->text();
}

void OptionDialog::SetImapPassword(const QString & p)
{
    m_lePassword->setText(p);
}

QString OptionDialog::GetImapPassword() const
{
    return m_lePassword->text();
}

void OptionDialog::on_cbProxyEnable_clicked()
{

}

void OptionDialog::on_cbProxyEnable_stateChanged(int arg1)
{
    m_leProxyIP->setEnabled(arg1 == Qt::Checked);
    m_leProxyPort->setEnabled(arg1 == Qt::Checked);
    m_leProxyLogin->setEnabled(arg1 == Qt::Checked);
    m_leProxyPassword->setEnabled(arg1 == Qt::Checked);
    m_cbRemoteDNS->setEnabled(arg1 == Qt::Checked);
}

void OptionDialog::on_cbUPnP_stateChanged(int arg1)
{
    (void) arg1;
}

void OptionDialog::SetEnableUPnP(bool enable)
{
    m_cbEnableUPnP->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

bool OptionDialog::GetEnableUPnP() const
{
    return m_cbEnableUPnP->checkState() == Qt::Checked;
}

void OptionDialog::SetProxyEnable(bool enable)
{
    m_cbProxyEnable->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
    //on_cbProxyEnable_stateChanged(enable ? Qt::Checked : Qt::Unchecke);
}

bool OptionDialog::GetProxyEnable() const
{
    return m_cbProxyEnable->checkState() == Qt::Checked;
}

void OptionDialog::SetProxyIP(const QString & ip)
{
    m_leProxyIP->setText(ip);
}

QString OptionDialog::GetProxyIP() const
{
    return m_leProxyIP->text();
}

void OptionDialog::SetProxyPort(unsigned short port)
{
    m_leProxyPort->setText(QString("%1").arg(port));
}

unsigned short OptionDialog::GetProxyPort() const
{
    return m_leProxyPort->text().toUShort();
}

void OptionDialog::SetProxyLogin(const QString & login)
{
    m_leProxyLogin->setText(login);
}

QString OptionDialog::GetProxyLogin() const
{
    return m_leProxyLogin->text();
}

void OptionDialog::SetProxyPassword(const QString & pass)
{
    m_leProxyPassword->setText(pass);
}

QString OptionDialog::GetProxyPassword() const
{
    return m_leProxyPassword->text();
}

void OptionDialog::SetRemoteDNS(bool enable)
{
    m_cbRemoteDNS->setCheckState(enable ? Qt::Checked : Qt::Unchecked);
}

bool OptionDialog::GetRemoteDNS() const
{
    return m_cbRemoteDNS->checkState() == Qt::Checked;
}
