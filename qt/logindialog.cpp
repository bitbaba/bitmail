#include "logindialog.h"
#include "ui_logindialog.h"

#include "optiondialog.h"
#include "netoptdialog.h"

#include <QPainter>
#include <QFileInfo>
#include <bitmailcore/bitmail.h>

#include "main.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    QLabel * lbLogo = findChild<QLabel *>("lbLogo");    
    lbLogo->setPixmap(QPixmap(BMQTApplication::GetImageResHome() + "/login.png"));

    m_lePassphrase = findChild<QLineEdit*>("lePassphrase");
    m_lePassphrase->setFixedHeight(32);
    m_lePassphrase->setFocus();

    m_cbEmail = findChild<QComboBox*>("cbEmail");
    m_cbEmail->setFixedHeight(48);
    m_cbEmail->setIconSize(QSize(48,48));

    setWindowIcon(QIcon(BMQTApplication::GetImageResHome() + "/bitmail.png"));

    reloadProfiles();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_cmdCreate_clicked()
{
    OptionDialog optDlg(true, this);
    if (optDlg.exec() != QDialog::Accepted){
        return ;
    }

    if (optDlg.email().isEmpty()
        || optDlg.passphrase().isEmpty()
        || optDlg.nick().isEmpty()
        || !optDlg.bits())
    {
        return ;
    }

    if (!createProfile(optDlg.email()
                       , optDlg.nick()
                       , optDlg.passphrase()
                       , optDlg.bits()
                       , optDlg.txUrl(), optDlg.login(), optDlg.password()
                       , optDlg.rxUrl(), optDlg.login(), optDlg.password()
                       , optDlg.socks5()))
    {
        return ;
    }

    reloadProfiles();
}

QString LoginDialog::GetEmail() const
{
    return m_cbEmail->currentText();
}

QString LoginDialog::GetPassphrase() const
{
    return m_lePassphrase->text();
}

void LoginDialog::on_btnEnter_clicked()
{
    done(QDialog::Accepted);
}

void LoginDialog::reloadProfiles()
{
    m_cbEmail->clear();

    QStringList sList = BMQTApplication::GetProfiles();
    for (QStringList::iterator it = sList.begin(); it != sList.end(); it++){
        m_cbEmail->addItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"),  QFileInfo(*it).fileName());
    }
}

bool LoginDialog::createProfile(const QString & qsEmail
                                , const QString & qsNick
                                , const QString & qsPassphrase
                                , int nBits
                                , const QString & txUrl, const QString & txLogin, const QString & txPass
                                , const QString & rxUrl, const QString & rxLogin, const QString & rxPass
                                , const QString & proxy)
{
    if (qsEmail.isEmpty()||qsNick.isEmpty()||qsPassphrase.isEmpty() || !nBits){
        return false;
    }

    BitMail * bm = BitMail::New();
    bool ret = bm->Genesis(nBits, qsNick.toStdString()
                      , qsEmail.toStdString()
                      , qsPassphrase.toStdString()
                      , txUrl.toStdString(), rxUrl.toStdString()
                      , txLogin.toStdString(), txPass.toStdString(), proxy.toStdString());
    if (!ret){
        BitMail::Free(bm); bm = NULL;
        return false;
    }
    // 1) add yourself as your friend
    bm->addContact(bm->email());

    // 2) save profile
    if (!BMQTApplication::SaveProfile(bm)){
        BitMail::Free(bm); bm = NULL;
        return false;
    }

    BitMail::Free(bm); bm = NULL;
    return true;
}
