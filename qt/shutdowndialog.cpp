#include <QLabel>

#include "shutdowndialog.h"
#include "ui_shutdowndialog.h"

ShutdownDialog::ShutdownDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShutdownDialog)
{
    ui->setupUi(this);
    m_lbMessage = findChild<QLabel*>("lbMessage");
}

ShutdownDialog::~ShutdownDialog()
{
    delete ui;
}

void ShutdownDialog::SetMessage(const QString &msg)
{
    (void)msg;
    if (m_lbMessage){
        m_lbMessage->setText(msg);
    }
}
