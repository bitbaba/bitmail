#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(bool fNew, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::OptionDialog)
    , m_fNew(fNew)
{
    ui->setupUi(this);

    m_leEmail        = findChild<QLineEdit*>("leEmail");
    if (!m_fNew) m_leEmail->setEnabled(false);
    m_leNick         = findChild<QLineEdit*>("leNick");
    if (!m_fNew) m_leNick->setEnabled(false);
    m_lePassphrase   = findChild<QLineEdit*>("lePassphrase");
    m_sbBits         = findChild<QSpinBox*>("sbBits");
    if (!m_fNew) m_sbBits->setEnabled(false);

    m_leSmtpUrl      = findChild<QLineEdit*>("leSmtpUrl");
    m_leSmtpLogin    = findChild<QLineEdit*>("leSmtpLogin");
    m_leSmtpPassword = findChild<QLineEdit*>("leSmtpPassword");

    m_leImapUrl      = findChild<QLineEdit*>("leImapUrl");
    m_leImapLogin    = findChild<QLineEdit*>("leImapLogin");
    m_leImapPassword = findChild<QLineEdit*>("leImapPassword");

    m_cbAllowStranger= findChild<QCheckBox*>("cbAllowStranger");
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_buttonBox_accepted()
{

}

void OptionDialog::on_buttonBox_rejected()
{

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
    (void)l;
    m_leSmtpLogin->setText(l);
}

QString OptionDialog::GetSmtpLogin() const
{
    return m_leSmtpLogin->text();
}

void OptionDialog::SetSmtpPassword(const QString & p)
{
    (void)p;
    m_leSmtpPassword->setText(p);
}

QString OptionDialog::GetSmtpPassword() const
{
    return m_leSmtpPassword->text();
}

void OptionDialog::SetImapUrl(const QString & u)
{
    (void)u;
    m_leImapUrl->setText(u);
}

QString OptionDialog::GetImapUrl() const
{
    return m_leImapUrl->text();
}

void OptionDialog::SetImapLogin(const QString & l)
{
    (void)l;
    m_leImapLogin->setText(l);
}

QString OptionDialog::GetImapLogin() const
{
    return m_leImapLogin->text();
}

void OptionDialog::SetImapPassword(const QString & p)
{
    (void)p;
    m_leImapPassword->setText(p);
}

QString OptionDialog::GetImapPassword() const
{
    return m_leImapPassword->text();
}

void OptionDialog::SetImapAllowStranger(const bool & s)
{
    (void)s;
    m_cbAllowStranger->setChecked(s);
}

bool OptionDialog::GetImapAllowStranger() const
{
    return m_cbAllowStranger->isChecked();
}
