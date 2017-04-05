#include "ardialog.h"
#include "ui_ardialog.h"

arDialog::arDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::arDialog)
{
    ui->setupUi(this);
    lblStatus = this->findChild<QLabel*>("lblAudioStatus");
}

arDialog::~arDialog()
{
    delete ui;
}

void arDialog::ShowStatus(const QString &st)
{
    lblStatus->setText(st);
}
