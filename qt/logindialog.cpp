#include "createprofiledialog.h"
#include "serversetupdialog.h"
#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDebug>

#include <bitmailcore/x509cert.h>

#define WRITE_FILE(p,s) \
    QFile fout; \
    fout.setFileName(QString::fromStdString(p)); \
    fout.open(QIODevice::WriteOnly | QIODevice::Text); \
    fout.write(s.c_str(), s.length()); \
    fout.close();

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowIcon(QIcon( QApplication::applicationDirPath() + "/res/images/bitmail.png"));

    m_ctrlKeyPass = this->findChild<QLineEdit *>("lineEdit_2");
    m_comboEmail = this->findChild<QComboBox * >("comboBox");

    QDir dirHome = QDir::home();
    dirHome.mkpath(QString("bitmail/accounts/"));
    dirHome.cd(QString("bitmail/accounts/"));

    QStringList subDirs = dirHome.entryList();
    for (QStringList::iterator it = subDirs.begin(); it != subDirs.end(); it++){
        if (!it->compare(".") || !it->compare("..")){
            continue;
        }
        qDebug() << QString("Append Buddy to ComboBox: ") << *it << "\n";
        m_comboEmail->addItem(*it);
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::ChangeProfileHome(const std::string & email)
{
    QDir dirHome = QDir::home();
    QString qsHome = QString("bitmail/accounts/") + QString::fromStdString(email);
    dirHome.mkpath(qsHome);
    QDir::setCurrent(QDir::homePath() + QString("/") + qsHome);
}

void LoginDialog::on_buttonBox_accepted()
{
    if (m_sEmail.empty()){
        QMessageBox::information(this, "","account is empty. quit");
        return ;
    }
    m_sKeyPass = m_ctrlKeyPass->text().toStdString();
}

std::string LoginDialog::GetEmail() const
{
    return m_sEmail;
}

std::string LoginDialog::GetKeyPass() const
{
    return m_sKeyPass;
}

/**
 * @brief Create a new profile
 */
void LoginDialog::on_pushButton_clicked()
{
    CreateProfileDialog profileDlg;
    std::string sCertPem, sKeyPem;
    if (profileDlg.exec() == QDialog::Accepted){
        sCertPem = profileDlg.GetCertPem();
        sKeyPem  = profileDlg.GetKeyPem();
    }
    if (sCertPem.empty() || sKeyPem.empty()){
        QMessageBox::information(this, tr("Create Profile"), tr("Failed to create profile"));
        return ;
    }

    CX509Cert cert;
    cert.LoadCertFromPem(sCertPem);
    if (!cert.IsValid()){
        QMessageBox::information(this, tr("Create Profile"), tr("Profile certificate is invalid"));
        return ;
    }

    std::string sEmail = cert.GetEmail();
    m_comboEmail->addItem(QString::fromStdString(sEmail));
    m_comboEmail->setCurrentIndex(m_comboEmail->count() - 1);

    if (!sCertPem.empty()){
        WRITE_FILE("./profile.crt", sCertPem);
    }
    if (!sKeyPem.empty()){
        WRITE_FILE("./profile.key", sKeyPem);
    }
}

/**
 * @brief Setup network
 */
void LoginDialog::on_pushButton_2_clicked()
{
    // Prompt email address
    if (m_sEmail.empty()){
        QMessageBox::information(this, this->windowTitle(), "Profile email is not selected!");
        return ;
    }

    ServerSetupDialog ssDlg;
    ssDlg.SetEmailForGuess(m_sEmail);

    std::string sNetworkConfig;
    if (ssDlg.exec() == QDialog::Accepted){
        sNetworkConfig = ssDlg.GetConfigStr();
    }

    if (!sNetworkConfig.empty()){
        WRITE_FILE("./network.json", sNetworkConfig);
    }
}

void LoginDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    qDebug() << "Profile changed to: " << arg1;
    m_sEmail = m_comboEmail->currentText().toStdString();
    ChangeProfileHome(m_sEmail);
}
