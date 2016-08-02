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
        else {
            m_leEmail->setPlaceholderText("someone@somesite.net");
            m_leEmail->setFocus();
        }
        m_leEmail->setValidator(new QRegExpValidator(QRegExp("[_a-zA-Z0-9.]+@[a-zA-Z0-9.]+"), m_leEmail));
        m_leNick         = findChild<QLineEdit*>("leNick");
        if (!m_fNew) m_leNick->setEnabled(false);
        m_sbBits         = findChild<QSpinBox*>("sbBits");
        if (!m_fNew) m_sbBits->setEnabled(false);
        m_lePassphrase   = findChild<QLineEdit*>("lePassphrase");
        if (!m_fNew){
            m_lePassphrase->setFocus();
        }
    }while(0);//Profile settings
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

    BitMail * bm = new BitMail();

    bm->CreateProfile(qsNick.toStdString()
                      , qsEmail.toStdString()
                      , qsPassphrase.toStdString()
                      , nBits);

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
