#include "messagedialog.h"
#include "ui_messagedialog.h"
#include <QLineEdit>
#include <QPlainTextEdit>

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);

    m_leFrom = findChild<QLineEdit*>("leFrom");
    m_leCertID = findChild<QLineEdit*>("leCertID");

    m_ptxtCert = findChild<QPlainTextEdit*>("ptxtCert");
    m_ptxtMessage = findChild<QPlainTextEdit*>("ptxtMessage");

    m_btnMakeFriend = findChild<QPushButton*>("btnMakeFriend");
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

void MessageDialog::EnableMakeFriend(bool fEnable)
{
    m_btnMakeFriend->setEnabled(fEnable);
}

void MessageDialog::on_btnMakeFriend_clicked()
{
    QString qsFrom = GetFrom();
    QString qsCert = GetCert();
    (void)qsFrom;
    (void)qsCert;
    //emit signalMakeFriend(qsFrom, qsCert);
    emit signalMakeFriend(m_leFrom->text(), m_ptxtCert->toPlainText());
}
