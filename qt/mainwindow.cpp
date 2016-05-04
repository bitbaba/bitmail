#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "threadgetmessage.h"
#include "dialogaddbuddy.h"
#include "serversetupdialog.h"
#include "dlgstrangerrequest.h"
#include "dlgshowqrcode.h"

#include <fstream>

#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QProgressBar>
#include <QTime>
#include <QDateTime>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QStringList>

#include <bitmailcore/x509cert.h>
#include <bitmailcore/bitmail.h>


#define WRITE_FILE(p,s) {\
    QFile fout; \
    fout.setFileName(QString::fromStdString(p)); \
    fout.open(QIODevice::WriteOnly | QIODevice::Text); \
    fout.write(s.c_str(), s.length()); \
    fout.close(); \
    }

enum ListRole{
    // 0 for text
    // 1 for icon
    // so , do not conflict.
    role_email = QListWidgetItem::UserType,
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    do {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags()
                             | Qt::MSWindowsFixedSizeDialogHint
                             | Qt::WindowMinimizeButtonHint
                             );
        this->setFixedSize(640,480);
        this->setWindowIcon(QIcon( QApplication::applicationDirPath() + "/res/images/bitmail.png"));

        m_input = this->findChild<QTextEdit *>("textEdit");
        m_msg   = this->findChild<QListWidget*>("listWidget_2");
        m_buddy = this->findChild<QListWidget*>("listWidget");
        m_stbar = this->findChild<QStatusBar*>("statusbar");
        m_progbar = new QProgressBar(this);
        m_progbar->setValue(0);
        m_progbar->setTextVisible(false);
        m_progbar->setMaximumWidth(320);
        m_stbar->addPermanentWidget(m_progbar, 1);
        m_stbar->showMessage("Online");

        m_buddy->setViewMode(QListView::IconMode);
        m_buddy->setResizeMode(QListView::Adjust);
        m_buddy->setSpacing(10);

        m_msg->setViewMode(QListView::ListMode);
        m_msg->setResizeMode(QListView::Adjust);
        m_msg->setSpacing(10);

        m_input->clear();
    }while(0);
}

bool MainWindow::Initialize(const std::string & sEmail, const std::string & sKeyPass)
{
    (void )sEmail;
    std::string sKeyPem;
    std::string sCertPem;
    Json::Value jsonBuddyIndex;
    do {
        sKeyPem  = this->ReadPlainTextFile("./profile.key");
        sCertPem = this->ReadPlainTextFile("./profile.crt");
    }while(0);

    m_mail = new BitMail();
    if (m_mail->LoadProfile(sKeyPass, sKeyPem, sCertPem)){
        QMessageBox::information(this, tr("Load Profile"), tr("Failed to load profile"));
        return false;
    }else{
        ResetTitle();
    }

    Json::Value smtpsConfig;
    Json::Value pop3sConfig;
    do{
        Json::Value networkConfig;
        this->LoadJsonConfig("./network.json", networkConfig);
        smtpsConfig = networkConfig["smtp"];
        m_mail->InitTx(smtpsConfig.toStyledString());
        pop3sConfig = networkConfig["pop3"];
        m_mail->InitRx(pop3sConfig.toStyledString());
    }while(0);

    /**
    * Loading buddies
    */
    this->LoadJsonConfig("./buddyIndex.json", jsonBuddyIndex);
    for (unsigned int i = 0; i < jsonBuddyIndex.size(); ++i){
        Json::Value jsonBuddy = jsonBuddyIndex[i];
        if (!jsonBuddy.isMember("email")){
            continue;
        }
        std::string buddyEmail = jsonBuddy["email"].asString();
        if (buddyEmail.empty()){
            continue ;
        }

        std::string sBuddyCert = ReadPlainTextFile(std::string("./Buddies/")
                                                 + jsonBuddy["email"].asString()
                                                 + std::string(".crt"));
        CX509Cert buddyCert;
        buddyCert.LoadCertFromPem(sBuddyCert);
        LoadBuddy(sBuddyCert);
    }

    /**
     * Fork a receiving thread.
     */
    do {
        unsigned int checkInterval = 3; // in seconds
        if (pop3sConfig.isMember("interval")){
            checkInterval = pop3sConfig["interval"].asUInt();
        }
        m_pThGetMessage = new ThreadGetMessage(m_mail, checkInterval);
        QObject::connect(m_pThGetMessage
                         , SIGNAL(MessageGot(QString ))
                         , this
                         , SLOT(onMessageGot(QString )));


        QObject::connect(m_pThGetMessage
                         , SIGNAL(StrangerRequest(QString ))
                         , this
                         , SLOT(onStrangerRequest(QString )));

        m_pThGetMessage->start();
    }while(0);

    return true;
}

void MainWindow::onMessageGot(QString sJsonMsg)
{
    (void )sJsonMsg;
    Json::Reader reader;
    Json::Value jsonMsg;
    reader.parse(sJsonMsg.toStdString(), jsonMsg);
    std::string from = jsonMsg["from"].asString();
    std::string sBuddyName;
    if (!from.empty()){
        CX509Cert x= m_mail->GetBuddyCert(from);
        sBuddyName = x.GetCommonName();
    }

    QString itemText = QString("[")
            + QString::fromStdString(sBuddyName.empty() ? from : sBuddyName)
            + QString("]: (")
            + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
            + QString(" )\r\n\r\n")
            + QString::fromStdString(jsonMsg["msg"].asString()) ;

    QListWidgetItem * item = new QListWidgetItem(itemText);
    item->setForeground(Qt::red);
    m_msg->addItem(item);
    m_msg->scrollToBottom();

    this->activateWindow();
    return ;
}

void MainWindow::onStrangerRequest(QString sJsonParam)
{
    this->activateWindow();

    (void )sJsonParam;
    Json::Value jsonMsgParam;
    Json::Reader reader;
    reader.parse(sJsonParam.toStdString(), jsonMsgParam);
    std::string sCertId = jsonMsgParam["certId"].asString();
    std::string sEmail = jsonMsgParam["email"].asString();
    std::string sCommonName = jsonMsgParam["commonName"].asString();
    std::string sCertPem = jsonMsgParam["cert"].asString();
    qDebug() << QString("\r\nStranger Request:\r\n\tcertId:") << QString::fromStdString(sCertId)
             << QString("\r\n\temail: ") << QString::fromStdString(sEmail)
             << QString("\r\n\tcommonName: ") << QString::fromStdString(sCommonName);

    DialogAddBuddy addBuddyDlg;
    addBuddyDlg.setWindowTitle(tr("Confirm to add new buddy"));
    addBuddyDlg.SetCertPem(sCertPem);
    if (addBuddyDlg.exec() == QDialog::Accepted){
        LoadBuddy(sCertPem);
        Config_AddBody(sCertPem);
    }
    return ;
}

MainWindow::~MainWindow()
{
    delete ui;
}

static int fn_progress(const std::string & info, void * userdata)
{
    MainWindow * pThis = (MainWindow *)userdata;
    if (pThis == NULL){
        return 0;
    }
    Json::Value jsonInfo;
    Json::Reader reader;
    if (info.empty()){
        return -1;
    }
    reader.parse(info,jsonInfo);
    if (!jsonInfo.isObject()){
        return -2;
    }
    std::string tip;
    if (jsonInfo.isMember("msg")){
        tip = jsonInfo["msg"].asString();
    }
    unsigned int prog = 0;
    if (jsonInfo.isMember("progress")){
        prog = jsonInfo["progress"].asUInt();
    }

    pThis->statusBar()->showMessage(QString::fromStdString(tip));
    pThis->UpdateProgress(prog);
    return 0;
}

void MainWindow::UpdateProgress(int prog)
{
    m_progbar->setValue(prog);
}

void MainWindow::on_pushButton_clicked()
{
    std::string msg;
    do {
        msg= m_input->toPlainText().toStdString();
    }while(0);

    if (m_currentBuddy.empty()){
        QMessageBox::information(this, QString("unknown recepient"), QString("please select a buddy."));
        return ;
    }
    UpdateProgress(0);
    if (!m_mail->SendMsg(m_currentBuddy, msg, fn_progress, this)){

        QString itemText = QString("[ Me ] ( ")
                + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                + QString(" )\r\n\r\n")
                + QString::fromStdString(msg)
                + QString("\r\n");

        QListWidgetItem * item = new QListWidgetItem(itemText);
        item->setForeground(Qt::green);
        m_msg->addItem(item);
        m_msg->scrollToBottom();
        m_input->clear();
    }else{
        QMessageBox::information(this, "Send", "failed to send message.");
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
   qDebug() << "Selected Buddy : "<< item->text();
   m_currentBuddy = item->data(role_email).toString().toStdString();
   ResetTitle();
}

void MainWindow::on_textEdit_textChanged()
{

}

std::string MainWindow::ReadPlainTextFile(const std::string &path)
{
    std::ifstream ifs;
    ifs.open(path.c_str(), std::ios_base::binary | std::ios_base::in);
    if (!ifs.is_open()){
        return "";
    }

    std::string txt;
    while(!ifs.eof()){
        char c;
        ifs.get(c);
        txt += c;
    }
    ifs.close();

    return txt;
}

bool MainWindow::LoadJsonConfig(const std::string & configPath, Json::Value & jsonConfig)
{
    std::string sJsonConfig = ReadPlainTextFile(configPath);
    if (sJsonConfig.empty()){
        return 0;
    }
    Json::Reader reader;
    return reader.parse(sJsonConfig, jsonConfig);
}


void MainWindow::on_actionAdd_triggered()
{
    DialogAddBuddy addBuddyDlg;
    std::string sCertPem;
    if (addBuddyDlg.exec() == QDialog::Accepted){
        sCertPem = addBuddyDlg.GetCertPem();
        if (sCertPem.empty()){
            return ;
        }
        LoadBuddy(sCertPem);
        Config_AddBody(sCertPem);
    }
}

void MainWindow::Config_AddBody(const std::string & sCertPem)
{
    if (sCertPem.empty()){
        return ;
    }
    CX509Cert xcert;
    xcert.LoadCertFromPem(sCertPem);
    std::string sCommonName = xcert.GetCommonName();
    std::string sEmail = xcert.GetEmail();
    std::string sCertID = xcert.GetID();

    Json::Value jsonBuddyIndex;
    jsonBuddyIndex ["nick"] = sCommonName;
    jsonBuddyIndex ["email"] = sEmail;
    jsonBuddyIndex ["id"] = sCertID; //TODO: hash cert with ripemd16
    Json::Value jsonBuddies;
    std::string sJson = ReadPlainTextFile("./buddyIndex.json");
    Json::Reader reader;
    if (!sJson.empty()) {
        reader.parse(sJson, jsonBuddies);
    }
    jsonBuddies.append(jsonBuddyIndex);
    WRITE_FILE("./buddyIndex.json", jsonBuddies.toStyledString());

    QDir curDir = QDir::current();
    curDir.mkdir(QString("Buddies"));
    std::string certPath = "./Buddies/";
    certPath += sEmail;
    certPath += ".crt";
    WRITE_FILE(certPath, sCertPem);
}

void MainWindow::Config_RemoveBody(const std::string & email)
{
    Json::Value jsonBuddies;
    std::string sJson = ReadPlainTextFile("./buddyIndex.json");
    Json::Reader reader;
    if (!sJson.empty()) {
        reader.parse(sJson, jsonBuddies);
    }
    Json::ArrayIndex removeIndex = ~0;
    for (Json::ArrayIndex i = 0; i < jsonBuddies.size(); i++){
        Json::Value jsonBuddy = jsonBuddies[i];
        std::string testEmail = jsonBuddy["email"].asString();
        if (testEmail == email){
            removeIndex = i;
            break;
        }
    }
    Json::Value jsonRemoved;
    jsonBuddies.removeIndex(removeIndex, &jsonRemoved);
    WRITE_FILE("./buddyIndex.json", jsonBuddies.toStyledString());
}

void MainWindow::LoadBuddy(const std::string & sCertPem)
{
    CX509Cert xcert;
    xcert.LoadCertFromPem(sCertPem);
    std::string sCommonName = xcert.GetCommonName();
    std::string sEmail = xcert.GetEmail();
    m_mail->AddBuddy(xcert);

    do {
        QListWidgetItem *item = new QListWidgetItem();
        //TODO, gender configuration
        item->setIcon(QIcon( QApplication::applicationDirPath() + "/res/head/alice.png"));
        item->setData(role_email, QVariant::fromValue(QString::fromStdString(sEmail)));
        item->setText(QString::fromStdString(sCommonName) + "(" + QString::fromStdString(sEmail) + ")");
        item->setForeground(Qt::green);
        m_buddy->addItem(item);
        m_buddy->update();
    }while(0);
}

void MainWindow::UnloadSelectedBuddy()
{
    std::string email = m_currentBuddy;
    m_buddy->takeItem(m_buddy->currentRow());
    m_buddy->update();
    m_mail->RemoveBuddy(email);
    Config_RemoveBody(email);
}

void MainWindow::ResetTitle()
{
    std::string sCommonName = m_mail->m_profile.GetCommonName();
    this->setWindowTitle(QString::fromStdString(sCommonName)
                         + QString("(")
                         + QString::fromStdString(m_mail->m_profile.GetEmail())
                         + QString(")"));

    if (!m_currentBuddy.empty()){
        QString sTitle = this->windowTitle();
        sTitle += " Talking to [ ";
        sTitle += QString::fromStdString(m_currentBuddy);
        sTitle += " ]";
        this->setWindowTitle(sTitle);
    }
}

void MainWindow::on_actionNetwork_triggered()
{
    std::string sNetworkConfig0 = ReadPlainTextFile("./network.json");
    Json::Reader reader0;
    Json::Value jsonConfig0;
    reader0.parse(sNetworkConfig0, jsonConfig0);

    ServerSetupDialog ssDlg;
    ssDlg.SetDefaultValueFromConfigration(sNetworkConfig0);

    std::string sNetworkConfig;
    Json::Reader reader;
    Json::Value jsonConfig;
    if (ssDlg.exec() == QDialog::Accepted){
        sNetworkConfig = ssDlg.GetConfigStr();
        reader.parse(sNetworkConfig, jsonConfig);
    }else{

    }
    if (!sNetworkConfig.empty() && jsonConfig0 != jsonConfig){
        WRITE_FILE("./network.json", sNetworkConfig);
        if (QMessageBox::Yes == QMessageBox::question(this, "Network", "Restart bitmail to take effect?", QMessageBox::Yes|QMessageBox::No)){
            QStringList args = QApplication::arguments();
            args.removeFirst();
            QProcess::startDetached(QApplication::applicationFilePath(), args);
            QCoreApplication::quit();
        }
    }
}

void MainWindow::on_actionRemove_triggered()
{
    UnloadSelectedBuddy();
}

void MainWindow::on_actionView_triggered()
{
    if (m_currentBuddy.empty()){
        return ;
    }

    DialogAddBuddy viewDlg;

    CX509Cert x = m_mail->GetBuddyCert(m_currentBuddy);

    if (!x.IsValid()) {
        return ;
    }

    viewDlg.setWindowTitle(tr("Buddy's certificate information"));
    viewDlg.SetCertPem(x.GetCertByPem());

    viewDlg.exec();
}

void MainWindow::on_actionProfile_triggered()
{
    DialogAddBuddy viewDlg;

    CX509Cert x = m_mail->m_profile;

    if (!x.IsValid()) {
        return ;
    }

    viewDlg.setWindowTitle(tr("Profile certificate's information"));
    viewDlg.SetCertPem(x.GetCertByPem());

    viewDlg.exec();
}

void MainWindow::on_actionRequest_triggered()
{
    std::string sStranger;
    std::string sCertID;
    DlgStrangerRequest reqDlg;
    if (reqDlg.exec() == QDialog::Accepted){
        sStranger = reqDlg.GetEmailAddress();
        if (!sStranger.empty() && sStranger.find('@') != sStranger.npos){
            this->m_mail->SendMsg(sStranger, tr("Helo").toStdString(), NULL, NULL);
            QMessageBox::information(this, tr("Requst"), tr("Send out successfully."));
        }
        sCertID = reqDlg.GetCertID();
        if (!sCertID.empty()
                && sCertID.find("SHA1:") != sCertID.npos
                && sCertID.length() == 45 /*SHA1:0+40*/){
            //TODO: Append id to buddy list and sign to protection.
            // then, when stranger reply with same CertID,
            // no confirmation is needed.

            QMessageBox::information(this, tr("Pre-Trust"), QString::fromStdString(sCertID));
        }
    }
}

void MainWindow::on_actionQrcode_triggered()
{
    if (m_currentBuddy.empty()) {
        QMessageBox::information(this, tr("Qrcode"), tr("select a buddy"));
        return ;
    }

    CX509Cert xcert = m_mail->GetBuddyCert(m_currentBuddy);
    if (!xcert.IsValid()){
        QMessageBox::information(this, tr("Qrcode"), tr("failed to get certificate info."));
        return ;
    }
    std::string certInfo = "BitMail:";
    certInfo += xcert.GetEmail();
    certInfo += "#";
    certInfo += xcert.GetID();

    QString qrpath = QDir::currentPath();
    qrpath += "/Buddies/";
    qrpath += QString::fromStdString(m_currentBuddy);
    qrpath += ".png";

    if (CreateQrImage(certInfo, qrpath.toStdString())){
        //TODO: display it
        DlgShowQrcode dlgShowQr;
        //QMessageBox::information(this, qrpath, qrpath);
        dlgShowQr.ShowQrImage(qrpath.toStdString(), xcert.GetEmail(), xcert.GetID());
        dlgShowQr.exec();
    }
}

void MainWindow::on_actionQrcode_2_triggered()
{
    CX509Cert xcert = m_mail->m_profile;
    if (!xcert.IsValid()){
        QMessageBox::information(this, tr("Qrcode"), tr("failed to get certificate info."));
        return ;
    }
    std::string certInfo = "BitMail:";
    certInfo += xcert.GetEmail();
    certInfo += "#";
    certInfo += xcert.GetID();

    QString qrpath = QDir::currentPath();
    qrpath += "/Profile.png";

    if (CreateQrImage(certInfo, qrpath.toStdString())){
        //TODO: display it
        DlgShowQrcode dlgShowQr;
        //QMessageBox::information(this, qrpath, qrpath);
        dlgShowQr.ShowQrImage(qrpath.toStdString(), xcert.GetEmail(), xcert.GetID());
        dlgShowQr.exec();
    }
}

void MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("http://imharrywu.github.io/bitmail-qt/bitmail-intro.html#help"));
}

void MainWindow::on_actionVersion_triggered()
{
    QDesktopServices::openUrl(QUrl("http://imharrywu.github.io/bitmail-qt/bitmail-intro.html#version"));
}

void MainWindow::on_actionThanks_triggered()
{
    QDesktopServices::openUrl(QUrl("http://imharrywu.github.io/bitmail-qt/bitmail-intro.html#thanks"));
}

void MainWindow::on_actionDonation_triggered()
{
    QDesktopServices::openUrl(QUrl("http://imharrywu.github.io/bitmail-qt/bitmail-intro.html#donation"));
}

bool MainWindow::CreateQrImage(const std::string &info, const std::string &path)
{
    QString app = QApplication::applicationDirPath();
    app += QString("\\qrencode.exe");

    QStringList args;
    args.push_back("-o"); args.push_back(QString::fromStdString(path));
    args.push_back(QString::fromStdString(info));

    if (0 == QProcess::execute(app, args)){
        //QMessageBox::information(this, tr("Qrcode"), tr("Qrcode generated."));
        return true;
    }else{
        //QMessageBox::information(this, tr("Qrcode"), tr("Qrcode failed."));
        return false;
    }
}
