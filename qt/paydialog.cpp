#include "paydialog.h"
#include "ui_paydialog.h"

PayDialog::PayDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PayDialog)
{
    ui->setupUi(this);
}

PayDialog::~PayDialog()
{
    delete ui;
}

void PayDialog::on_buttonBox_accepted()
{

}

void PayDialog::on_buttonBox_rejected()
{

}
