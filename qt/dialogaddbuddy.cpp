#include "dialogaddbuddy.h"
#include "ui_dialogaddbuddy.h"
#include <QTextEdit>

#include <bitmailcore/x509cert.h>

DialogAddBuddy::DialogAddBuddy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddBuddy)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    m_edit = this->findChild<QTextEdit*>("textEdit");
}

DialogAddBuddy::~DialogAddBuddy()
{
    delete ui;
}

std::string DialogAddBuddy::GetCertPem() const{
    return m_edit->toPlainText().toStdString();
}

void DialogAddBuddy::SetCertPem(const std::string &sCertPem)
{
    m_edit->setText(QString::fromStdString(sCertPem));
    this->findChild<QLineEdit*>("lineEdit")->setDisabled(true);
    this->findChild<QLineEdit*>("lineEdit_2")->setDisabled(true);
    this->findChild<QLineEdit*>("lineEdit_3")->setDisabled(true);
}

void DialogAddBuddy::UpdateGUI(const std::string & sCertPem)
{
    CX509Cert x;
    x.LoadCertFromPem(sCertPem);
    this->findChild<QLineEdit*>("lineEdit")->setText(QString::fromStdString(x.GetEmail()));
    this->findChild<QLineEdit*>("lineEdit_2")->setText(QString::fromStdString(x.GetCommonName()));
    this->findChild<QLineEdit*>("lineEdit_3")->setText(QString::fromStdString(x.GetID()));
}

void DialogAddBuddy::on_textEdit_textChanged()
{
    UpdateGUI(GetCertPem());
}
