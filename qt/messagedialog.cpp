#include "messagedialog.h"
#include "ui_messagedialog.h"
#include <QLineEdit>
#include <QPlainTextEdit>

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::SetFrom(const QString & qsFrom)
{
    m_leFrom->setText(qsFrom);
}

QString MessageDialog::GetFrom() const
{
    return m_leFrom->text();
}

void MessageDialog::SetCertID(const QString & qsCertID)
{
    m_leCertID->setText(qsCertID);
}

QString MessageDialog::GetCertID() const
{
    return m_leCertID->text();
}

void MessageDialog::SetMessage(const QString & qsMessage)
{
    m_ptxtMessage->setPlainText(qsMessage);
}

QString MessageDialog::GetMessage() const
{
    return m_ptxtMessage->toPlainText();
}

void MessageDialog::SetCert(const QString & qsCert)
{
    m_ptxtCert->setPlainText(qsCert);
}

QString MessageDialog::GetCert() const
{
    return m_ptxtCert->toPlainText();
}

void MessageDialog::on_buttonBox_accepted()
{

}

void MessageDialog::on_buttonBox_rejected()
{

}
