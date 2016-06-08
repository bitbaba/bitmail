#include "proxydialog.h"
#include "ui_proxydialog.h"

#include <QLineEdit>
#include <QComboBox>

ProxyDialog::ProxyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProxyDialog)
{
    ui->setupUi(this);
}

ProxyDialog::~ProxyDialog()
{
    delete ui;
}

void ProxyDialog::on_buttonBox_accepted()
{

}

void ProxyDialog::on_buttonBox_rejected()
{

}
