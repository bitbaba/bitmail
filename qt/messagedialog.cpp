#include "messagedialog.h"
#include "ui_messagedialog.h"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <bitmailcore/bitmail.h>
#include <QMessageBox>

MessageDialog::MessageDialog(BitMail * bitmail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog),
    m_bitmail(bitmail)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/bitmail.png"));

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
    QString qsCertID = GetCertID();

    if (m_bitmail->HasFriend(qsFrom.toStdString())){
        if (m_bitmail->IsFriend(qsFrom.toStdString(), qsCert.toStdString())){
            QMessageBox::information(this
                                     , tr("Friend")
                                     , tr("You are already friends.")
                                     , QMessageBox::Ok);
            return ;
        }
        QString qsPrevCertID = QString::fromStdString(m_bitmail->GetFriendID(qsFrom.toStdString()));
        int ret = QMessageBox::question(this
                                        , tr("Add Friend")
                                        , tr("Replace with this certificate?\r\n")
                                        + tr("Old: [") + qsPrevCertID + QString("]\r\n")
                                        + tr("New: [") + qsCertID + QString("]\r\n")
                                        , QMessageBox::Yes | QMessageBox::No);
        if (ret != QMessageBox::Yes){
            return ;
        }
    }

    if (bmOk != m_bitmail->AddFriend(qsFrom.toStdString(), qsCert.toStdString())){
        QMessageBox::warning(this, tr("Add Friend"), tr("Failed to add friend"), QMessageBox::Ok);
        return ;
    }

    emit signalAddFriend(qsFrom);
}
