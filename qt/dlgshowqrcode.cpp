#include "dlgshowqrcode.h"
#include "ui_dlgshowqrcode.h"

#include <QLabel>

DlgShowQrcode::DlgShowQrcode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgShowQrcode)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    m_ctrlLabel = this->findChild<QLabel*>("label");
    this->setWindowTitle(tr("Qrcode"));
}

DlgShowQrcode::~DlgShowQrcode()
{
    delete ui;
}

void DlgShowQrcode::ShowQrImage(const std::string &qrimagepath, const std::string & email, const std::string & certid)
{
    QPixmap pmap(QString::fromStdString(qrimagepath));
    m_ctrlLabel->setPixmap(pmap.scaled(m_ctrlLabel->width(), m_ctrlLabel->height()));
    this->findChild<QLineEdit*>("lineEdit")->setText(QString::fromStdString(email));
    this->findChild<QLineEdit*>("lineEdit_2")->setText(QString::fromStdString(certid));
}
