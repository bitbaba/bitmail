#include "serversetupdialog.h"

#include "createprofiledialog.h"
#include "ui_createprofiledialog.h"

#include <QDir>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <json/json.h>

#include <bitmailcore/x509cert.h>

CreateProfileDialog::CreateProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateProfileDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
}

CreateProfileDialog::~CreateProfileDialog()
{
    delete ui;
}

void CreateProfileDialog::on_buttonBox_accepted()
{
    m_sNick = this->findChild<QLineEdit *>("lineEdit")->text().toStdString();
    m_sEmail = this->findChild<QLineEdit *>("lineEdit_2")->text().toStdString();
    m_sKeyPass = this->findChild<QLineEdit *>("lineEdit_3")->text().toStdString();

    //TODO: save profile into home
    do {
        QDir::setCurrent(QDir::homePath() + QString("/BitMail/Accounts/") + QString::fromStdString(m_sEmail));
        qDebug() << "Current Working Path: " << QDir::currentPath();
    }while(0);

    do {
        CX509Cert x;
        x.Create(m_sNick, m_sEmail, m_sKeyPass);
        m_sCertPem = x.GetCertByPem();
        m_sKeyPem = x.GetPrivateKeyByEncryptedPem();
    }while(0);
}

std::string CreateProfileDialog::GetCertPem() const
{
    return m_sCertPem;
}

std::string CreateProfileDialog::GetKeyPem() const
{
    return m_sKeyPem;
}

