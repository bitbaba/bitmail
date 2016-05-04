#include "serversetupdialog.h"
#include "ui_serversetupdialog.h"

#include "json/json.h"
#include <QFile>
#include <QMessageBox>

ServerSetupDialog::ServerSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerSetupDialog)
{
    ui->setupUi(this);
    do {
        //TODO: Initialize fields by guess from email.
        m_cTxUrl = this->findChild<QLineEdit*>("lineEdit");
        m_cTxLogin = this->findChild<QLineEdit*>("lineEdit_2");
        m_cTxPass = this->findChild<QLineEdit*>("lineEdit_3");

        m_cRxUrl = this->findChild<QLineEdit*>("lineEdit_4");
        m_cRxLogin = this->findChild<QLineEdit*>("lineEdit_5");
        m_cRxPass = this->findChild<QLineEdit*>("lineEdit_6");
    }while(0);
}

ServerSetupDialog::~ServerSetupDialog()
{
    delete ui;
}

void ServerSetupDialog::on_buttonBox_accepted()
{
    do {
        m_txUrl = this->findChild<QLineEdit *>("lineEdit")->text().toStdString();
        m_txLogin = this->findChild<QLineEdit *>("lineEdit_2")->text().toStdString();
        m_txPass = this->findChild<QLineEdit *>("lineEdit_3")->text().toStdString();

        m_rxUrl = this->findChild<QLineEdit *>("lineEdit_4")->text().toStdString();
        m_rxLogin = this->findChild<QLineEdit *>("lineEdit_5")->text().toStdString();
        m_rxPass = this->findChild<QLineEdit *>("lineEdit_6")->text().toStdString();
        m_nPop3Interval = this->findChild<QLineEdit*>("lineEdit_7")->text().toInt();
        m_fAllowStrangers = this->findChild<QCheckBox*>("checkBox")->isChecked();
    }while(0);

    do {
        Json::Value jsonServerConfig;

        Json::Value jsonSmtpConfig;
        jsonSmtpConfig ["url"] = m_txUrl;
        jsonSmtpConfig ["account"] = m_txLogin;
        jsonSmtpConfig ["password"] = m_txPass;
        jsonServerConfig ["smtp"] = jsonSmtpConfig;

        Json::Value jsonPop3Config;
        jsonPop3Config ["url"] = m_rxUrl;
        jsonPop3Config ["account"] = m_rxLogin;
        jsonPop3Config ["password"] = m_rxPass;
        jsonPop3Config ["interval"] = m_nPop3Interval;
        jsonPop3Config ["allow_strangers"] = m_fAllowStrangers;
        jsonServerConfig ["pop3"] = jsonPop3Config;

        m_sConfig = jsonServerConfig.toStyledString();
    }while(0);
}

void ServerSetupDialog::SetEmailForGuess(const std::string &email)
{
    m_sEmailGuess = email;
    //TODO: Nslookup for MX records
    // or from local records' cache.
    std::string sDomain;
    if (m_sEmailGuess.find("@") != std::string::npos
            && m_sEmailGuess.find("@") < m_sEmailGuess.length()){
        sDomain = m_sEmailGuess.substr(m_sEmailGuess.find("@")+1);
    }
    if (sDomain.empty()){
        return ;
    }
    m_cTxUrl->setText(QString("smtps://smtp.") + QString::fromStdString(sDomain) + QString("/"));
    m_cTxLogin->setText(QString::fromStdString(m_sEmailGuess));

    m_cRxUrl->setText(QString("pop3s://pop.") + QString::fromStdString(sDomain) + QString("/"));
    m_cRxLogin->setText(QString::fromStdString(m_sEmailGuess));
}

void ServerSetupDialog::SetDefaultValueFromConfigration(const std::string & sJsonParam)
{
    Json::Reader reader;
    Json::Value jsonParam;
    reader.parse(sJsonParam, jsonParam);

    if (jsonParam.isMember("smtp")){
        Json::Value smtpConfig = jsonParam["smtp"];
        m_cTxUrl->setText(QString::fromStdString(smtpConfig["url"].asString()));
        m_cTxLogin->setText(QString::fromStdString(smtpConfig["account"].asString()));
        m_cTxPass->setText(QString::fromStdString(smtpConfig["password"].asString()));
    }

    if (jsonParam.isMember("pop3")){
        Json::Value pop3Config = jsonParam["pop3"];
        m_cRxUrl->setText(QString::fromStdString(pop3Config["url"].asString()));
        m_cRxLogin->setText(QString::fromStdString(pop3Config["account"].asString()));
        m_cRxPass->setText(QString::fromStdString(pop3Config["password"].asString()));
        this->findChild<QLineEdit*>("lineEdit_7")->setText(QString::number(static_cast<double>(pop3Config["interval"].asUInt())));
        if (pop3Config["allow_strangers"].asBool()){
            this->findChild<QCheckBox*>("checkBox")->setCheckState(Qt::Checked);
        }
    }
    return ;
}

std::string ServerSetupDialog::GetConfigStr(void) const
{
    return m_sConfig;
}

void ServerSetupDialog::on_checkBox_stateChanged(int arg1)
{
    if (arg1){
        //QMessageBox::information(this, "Network", "allow stranger(s)' request will spam your pop3 mailbox");
    }
    else{
        // Nothing.
    }
}
