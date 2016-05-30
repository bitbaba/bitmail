#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    QLabel * lbLogo = findChild<QLabel *>("lbLogo");
    lbLogo->setPixmap(QPixmap(":/images/bkgrd.png"));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
