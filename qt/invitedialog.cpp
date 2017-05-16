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
    return m_leWhisper->text().isEmpty() ? tr("Hi") : m_leWhisper->text();
}
