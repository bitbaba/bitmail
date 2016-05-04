#include "dlgstrangerrequest.h"
#include "ui_dlgstrangerrequest.h"

#include <QLineEdit>

DlgStrangerRequest::DlgStrangerRequest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStrangerRequest)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    this->findChild<QLineEdit*>("lineEdit_1")->setText(tr("SHA1:"));
}

DlgStrangerRequest::~DlgStrangerRequest()
{
    delete ui;
}

std::string DlgStrangerRequest::GetEmailAddress() const
{
    return this->findChild<QLineEdit*>("lineEdit")->text().toStdString();
}

std::string DlgStrangerRequest::GetCertID() const
{
    return this->findChild<QLineEdit*>("lineEdit_1")->text().toStdString();
}
