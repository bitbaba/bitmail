#include "optiondialog.h"
#include "ui_optiondialog.h"
#include "main.h"

#include <bitmailcore/bitmail.h>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <bitmailcore/bitmail.h>

OptionDialog::OptionDialog(bool fNew, QWidget *parent) :
    QDialog(parent)
    , ui(new Ui::OptionDialog)
    , newProfile(fNew)
{
    ui->setupUi(this);

    m_leEmail        = findChild<QLineEdit*>("leEmail"     );
    m_leNick         = findChild<QLineEdit*>("leNick"      );
    m_sbBits         = findChild<QSpinBox*> ("sbBits"      );
    m_lePassphrase   = findChild<QLineEdit*>("lePassphrase");

    m_leTxUrl        = findChild<QLineEdit*>("leSmtpUrl"   );
    m_leRxUrl        = findChild<QLineEdit*>("leImapUrl"   );
    m_leLogin        = findChild<QLineEdit*>("leLogin"     );
    m_lePassword     = findChild<QLineEdit*>("lePassword"  );
    m_leSocks5       = findChild<QLineEdit*>("leSocks5"    );

    m_btnExCert      = findChild<QPushButton*>("btnExportCert");
    m_btnExKey       = findChild<QPushButton*>("btnExportKey" );

    m_leEmail->setEnabled(newProfile);

    m_leNick->setEnabled(newProfile);

    m_sbBits->setEnabled(newProfile);

    m_btnExCert->setEnabled(!newProfile);

    m_btnExKey->setEnabled(!newProfile);

    m_lePassphrase->setEnabled(true);

    if (newProfile){
        m_leEmail->setFocus();
    }else{
        m_lePassphrase->setFocus();
    }

    m_leTxUrl->setPlaceholderText("smtps://smtp.somesite.net/");
    m_leTxUrl->setValidator(new QRegExpValidator(QRegExp("smtps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leTxUrl));

    m_leRxUrl->setPlaceholderText("imaps://imap.somesite.net/");
    m_leRxUrl->setValidator(new QRegExpValidator(QRegExp("imaps://[a-ZA-Z0-9.]+/", Qt::CaseInsensitive), m_leRxUrl));

    m_leSocks5->setPlaceholderText("socks5://login:password@127.0.0.1:1080/");
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::email(const QString & e)
{
    m_leEmail->setText(e);
}

QString OptionDialog::email() const
{
    return m_leEmail->text();
}

void OptionDialog::nick(const QString & n)
{
    m_leNick->setText(n);
}

QString OptionDialog::nick() const
{
    return m_leNick->text();
}

void OptionDialog::passphrase(const QString & p)
{
    m_lePassphrase->setText(p);
}

QString OptionDialog::passphrase() const
{
    return m_lePassphrase->text();
}

void OptionDialog::bits(const int &n)
{
    /**
    * there is no alignment limit on `openssl genrsa [bits]'
    * e.g. openssl genrsa 1234
    */
    m_sbBits->setValue((n + m_sbBits->singleStep() - 1)/m_sbBits->singleStep() * m_sbBits->singleStep());
}

int OptionDialog::bits() const
{
    return m_sbBits->value();
}

void OptionDialog::txUrl(const QString & txurl)
{
    m_leTxUrl->setText(txurl);
}

QString OptionDialog::txUrl(void) const
{
    return m_leTxUrl->text();
}

void OptionDialog::rxUrl(const QString & rxurl)
{
    m_leRxUrl->setText(rxurl);
}

QString OptionDialog::rxUrl(void) const\
{
    return m_leRxUrl->text();
}

void OptionDialog::login(const QString & l)
{
    m_leLogin->setText(l);
}

QString OptionDialog::login(void) const
{
    return m_leLogin->text();
}

void OptionDialog::password(const QString & p)
{
    m_lePassword->setText(p);
}

QString OptionDialog::password(void) const
{
    return m_lePassword->text();
}

void OptionDialog::socks5(const QString & s)
{
    m_leSocks5->setText(s);
}

QString OptionDialog::socks5(void) const
{
    return m_leSocks5->text();
}

void OptionDialog::on_leEmail_textChanged(const QString &arg1)
{
    if (arg1.contains("@")){
        txUrl(BMQTApplication::guessTxUrl(arg1));
        rxUrl(BMQTApplication::guessRxUrl(arg1));
    }
    login(arg1);
}

void OptionDialog::on_btnExportCert_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Export Certificate"));
    if (QFileInfo(path).completeSuffix() != "crt"
        || QFileInfo(path).completeSuffix() != "cer"){
        path += ".crt";
    }
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.setTextModeEnabled(false);
    std::string cert = BitMail::getInst()->cert();
    file.write(cert.data(), cert.length());
    file.close();
}

void OptionDialog::on_btnExportKey_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Export Private Key"));
    if (QFileInfo(path).completeSuffix() != "pfx"
        || QFileInfo(path).completeSuffix() != "p12"){
        path += ".p12";
    }
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.setTextModeEnabled(false);
    std::string pkcs12 = BitMail::getInst()->pkcs12();
    file.write(pkcs12.data(), pkcs12.length());
    file.close();
}
