#include "invitedialog.h"
#include "ui_invitedialog.h"

#include <QLineEdit>
#include <QFileDialog>

InviteDialog::InviteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InviteDialog)
{
    ui->setupUi(this);

    m_leEmail = findChild<QLineEdit*>("leEmail");
    m_leWhisper = findChild<QLineEdit*>("leWhisper");
    m_leAttachment = findChild<QLineEdit*>("leAttachment");

    m_leEmail->setFocus();
}

InviteDialog::~InviteDialog()
{
    delete ui;
}

QString InviteDialog::GetEmail() const
{
    return m_leEmail->text();
}

QString InviteDialog::GetWhisper() const
{
    if( m_leWhisper->text().isEmpty())
    {
        return "Hi";
    }
    return m_leWhisper->text();
}

QStringList InviteDialog::attachments() const
{
    return m_leAttachment->text().split("|");
}

void InviteDialog::on_buttonBox_accepted()
{

}

void InviteDialog::on_buttonBox_rejected()
{

}

void InviteDialog::on_btnChoose_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select some attachment(s)"));
    m_leAttachment->setText(files.join("|"));
}
