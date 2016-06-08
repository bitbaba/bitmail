#include "invitedialog.h"
#include "ui_invitedialog.h"

#include <QLineEdit>

InviteDialog::InviteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InviteDialog)
{
    ui->setupUi(this);

    m_leEmail = findChild<QLineEdit*>("leEmail");
    m_leWhisper = findChild<QLineEdit*>("leWhisper");

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


void InviteDialog::on_buttonBox_accepted()
{

}

void InviteDialog::on_buttonBox_rejected()
{

}
