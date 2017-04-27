#include "messagedialog.h"
#include "ui_messagedialog.h"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <bitmailcore/bitmail.h>
#include <QMessageBox>
#include "main.h"

MessageDialog::MessageDialog(BitMail * bitmail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog),
    m_bitmail(bitmail)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(BMQTApplication::GetImageResHome() + "/bitmail.png"));

    m_leFrom = findChild<QLineEdit*>("leFrom");
    m_leCertID = findChild<QLineEdit*>("leCertID");

    m_ptxtCert = findChild<QPlainTextEdit*>("ptxtCert");
    m_ptxtMessage = findChild<QTextEdit*>("ptxtMessage");

    m_btnMakeFriend = findChild<QPushButton*>("btnMakeFriend");

    m_leGroupId = findChild<QLineEdit*>("leGroupId");
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
    m_ptxtMessage->setText(qsMessage);
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

QString MessageDialog::group() const
{
    return m_leGroupId->text();
}

void MessageDialog::group(const QString & groupmembers)
{
    m_leGroupId->setText(groupmembers);
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

    if (!m_bitmail->attrib(qsFrom.toStdString(), "cert", qsCert.toStdString())){
        QMessageBox::warning(this, tr("Add Friend"), tr("Failed to set certificate of contact"), QMessageBox::Ok);
        return ;
    }

    emit friendsChanged();
}

/**
 * @brief MessageDialog::on_btnJoinGroup_clicked
 * Qt has a feature, the standard function signature:
 * such as on_DUMMYID_clicked() is bound with DUMMYID,
 * autumatically.
 */
void MessageDialog::on_btnJoinGroup_clicked()
{
    m_bitmail->addContact(group().toStdString());

    emit groupsChanged();
}
