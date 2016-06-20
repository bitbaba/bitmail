/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
//! [0]
#include <QtWidgets>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonValue>
#include <QJsonDocument>
#include <QByteArray>
#include <bitmailcore/bitmail.h>
#include "mainwindow.h"
//! [0]
#include "pollthread.h"
#include "rxthread.h"
#include "txthread.h"
#include "optiondialog.h"
#include "logindialog.h"
#include "paydialog.h"
#include "shutdowndialog.h"
#include "certdialog.h"
#include "invitedialog.h"
#include "messagedialog.h"
#include "main.h"
//! [1]
MainWindow::MainWindow(BitMail * bitmail)
    : m_bitmail(bitmail)
    , m_pollth(NULL)
    , m_rxth(NULL)
    , m_txth(NULL)
    , m_shutdownDialog(NULL)
//! [1] //! [2]
{
    /**
      * Icon
      */
#if defined(MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowIcon(QIcon(":/images/bitmail.png"));
    /** Title */
    do {
        QString qsEmail = QString::fromStdString(m_bitmail->GetEmail());
        QString qsNick = QString::fromStdString(m_bitmail->GetNick());
        QString qsTitle = tr("BitMail");
        qsTitle += " - ";
        qsTitle += qsNick;
        qsTitle += "(";
        qsTitle += qsEmail;
        qsTitle += ")";
        setWindowTitle(qsTitle);
    }while(0);
    /** Toolbars */
    createActions();
    createToolBars();
    createStatusBar();
    /** Form */
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout * rightLayout = new QVBoxLayout;
    QHBoxLayout * btnLayout = new QHBoxLayout;
    blist = new QListWidget;
    blist->setIconSize(QSize(48,48));
    blist->setFixedWidth(256);
    leftLayout->addWidget(blist);
    mainLayout->addLayout(leftLayout);
    msgView = new QListWidget;
    msgView->setIconSize(QSize(48,48));
    msgView->setSpacing(2);
    textEdit = new QTextEdit;
    textEdit->setMinimumWidth(400);
    textEdit->setFixedHeight(96);
    textEdit->setFocus();
    btnSend = new QPushButton(tr("Send"));
    btnSend->setToolTip(tr("Ctrl+Enter"));
    btnSend->setFixedWidth(64);
    btnSend->setFixedHeight(32);
    btnSend->setEnabled(false);
    btnSend->setShortcut(QKeySequence("Ctrl+Return"));
    btnLayout->addWidget(btnSend);
    btnLayout->setAlignment(btnSend, Qt::AlignLeft);
    rightLayout->addWidget(msgView);
    rightLayout->addWidget(chatToolbar);
    rightLayout->addWidget(textEdit);
    rightLayout->addLayout(btnLayout);
    mainLayout->addLayout(rightLayout);
    QWidget * wrap = new QWidget(this);
    wrap->setMinimumWidth(720);
    wrap->setMinimumHeight(640);
    wrap->setLayout(mainLayout);
    setCentralWidget(wrap);
    // Add signals
    connect(btnSend, SIGNAL(clicked()) , this, SLOT(onSendBtnClicked()));
    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    connect(blist, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onCurrentBuddy(QListWidgetItem*,QListWidgetItem*)));
    connect(blist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onBuddyDoubleClicked(QListWidgetItem*)));
    connect(msgView, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onMessageDoubleClicked(QListWidgetItem*)));
    // Populate buddies list
    populateBuddies();
    // startup network
    startupNetwork();
}
//! [2]
MainWindow::~MainWindow()
{
}
void MainWindow::startupNetwork()
{
    m_pollth = new PollThread(m_bitmail);
    m_rxth = (new RxThread(m_bitmail));
    m_txth = (new TxThread(m_bitmail));
    connect(this, SIGNAL(readyToSend(QString,QString)), m_txth, SLOT(onSendMessage(QString,QString)));
    connect(m_pollth, SIGNAL(inboxPollEvent()), m_rxth, SLOT(onInboxPollEvent()));
    connect(m_rxth, SIGNAL(gotMessage(QString,QString, QString,QString)), this, SLOT(onNewMessage(QString,QString, QString,QString)));
    connect(m_pollth, SIGNAL(done()), this, SLOT(onPollDone()));
    connect(m_pollth, SIGNAL(inboxPollProgress(QString)), this, SLOT(onPollProgress(QString)));
    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));
    connect(m_rxth, SIGNAL(rxProgress(QString)), this, SLOT(onRxProgress(QString)));
    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));
    connect(m_txth, SIGNAL(txProgress(QString)), this, SLOT(onTxProgress(QString)));
    m_pollth->start(); m_rxth->start(); m_txth->start();
}
void MainWindow::shutdownNetwork()
{
    if (m_rxth ){
        m_rxth->stop();
    }
    if (m_txth) {
        m_txth->stop();
    }
    if (m_pollth) {
        m_pollth->stop();
    }
}
void MainWindow::onRxDone()
{
    if (!m_shutdownDialog)
        return ;
    qDebug() << "Rx thread done";
    m_shutdownDialog->SetMessage(tr("Rx thread done."));
    m_rxth->wait(1000); delete m_rxth; m_rxth = NULL;
    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}
void MainWindow::onRxProgress(const QString &info)
{
    qDebug() << info;
    statusBar()->showMessage(info, 2000);
    return ;
}
void MainWindow::onTxProgress(const QString &info)
{
    qDebug() << info;
    statusBar()->showMessage(info, 2000);
    return ;
}
void MainWindow::onPollProgress(const QString &info)
{
    qDebug() << info;
    statusBar()->showMessage(info, 2000);
}

void MainWindow::onTxDone()
{
    if (!m_shutdownDialog)
        return ;
    qDebug() << "Tx thread done";
    m_shutdownDialog->SetMessage(tr("Tx thread done."));
    m_txth->wait(1000); delete m_txth; m_txth = NULL;
    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}
void MainWindow::onPollDone()
{
    if (!m_shutdownDialog)
        return ;
    qDebug() << ("Poll thread done");
    m_shutdownDialog->SetMessage(tr("Poll thread done."));
    m_pollth->wait(1000); delete m_pollth; m_pollth = NULL;
    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}
//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    shutdownNetwork();
    /**
    * Model here
    */
    if (m_rxth || m_txth || m_pollth){
        m_shutdownDialog = new ShutdownDialog(this);
        m_shutdownDialog->exec();
    }
    if (m_bitmail != NULL){
        BMQTApplication::SaveProfile(m_bitmail);
        delete m_bitmail;
        m_bitmail = NULL;
    }
    qDebug() << "BitMail quit! Bye";
    event->accept();
}
//! [4]
void MainWindow::onBuddyDoubleClicked(QListWidgetItem *actItem)
{
    if (actItem == NULL){
        return ;
    }
    QString qsEmail = actItem->data(Qt::UserRole).toString();
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(qsEmail.toStdString()));
    QString qsCertID = QString::fromStdString(m_bitmail->GetFriendID(qsEmail.toStdString()));
    CertDialog certDialog(this);
    certDialog.SetEmail(qsEmail);
    certDialog.SetNick(qsNick);
    certDialog.SetCertID(qsCertID);
    if (QDialog::Rejected == certDialog.exec()){
        return ;
    }
    (void)qsEmail;
    return ;
}
//! [17]
void MainWindow::createActions()
//! [17] //! [18]
{
    do {
        configAct = new QAction(QIcon(":/images/config.png"), tr("&Configure"), this);
        configAct->setStatusTip(tr("Configure current account"));
        connect(configAct, SIGNAL(triggered()), this, SLOT(onConfigBtnClicked()));
    }while(0);
    do {
        inviteAct = new QAction(QIcon(":/images/invite.png"), tr("&Invite"), this);
        inviteAct->setStatusTip(tr("Invite a new friend by send a request message."));
        connect(inviteAct, SIGNAL(triggered()), this, SLOT(onInviteBtnClicked()));
    }while(0);
    do{
        snapAct = new QAction(QIcon(":/images/snap.png"), tr("&Snapshot"), this);
        snapAct->setStatusTip(tr("Snapshot"));
        fileAct = new QAction(QIcon(":/images/file.png"), tr("&File"), this);
        fileAct->setStatusTip(tr("File"));
        emojAct = new QAction(QIcon(":/images/emoj.png"), tr("&Emoji"), this);
        emojAct->setStatusTip(tr("Emoji"));
        soundAct = new QAction(QIcon(":/images/sound.png"), tr("&Sound"), this);
        soundAct->setStatusTip(tr("Sound"));
        videoAct = new QAction(QIcon(":/images/video.png"), tr("&Video"), this);
        videoAct->setStatusTip(tr("Video"));
        liveAct = new QAction(QIcon(":/images/live.png"), tr("&Live"), this);
        liveAct->setStatusTip(tr("Live"));
        payAct = new QAction(QIcon(":/images/bitcoin.png"), tr("&Pay"), this);
        payAct->setStatusTip(tr("Pay by Bitcoin"));
        connect(payAct, SIGNAL(triggered()), this, SLOT(onPayBtnClicked()));
    }while(0);
    do{
        walletAct = new QAction(QIcon(":/images/wallet.s.png"), tr("&BitCoinWallet"), this);
        walletAct->setStatusTip(tr("Configure Bitcoin wallet"));
    }while(0);
}
//! [24]
//! [29] //! [30]
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(configAct);
    editToolBar = addToolBar(tr("Buddies"));
    editToolBar->addAction(inviteAct);
    walletToolbar = addToolBar(tr("Wallet"));
    walletToolbar->addAction(walletAct);
    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->addAction(emojAct);
    chatToolbar->addAction(snapAct);
    chatToolbar->addAction(fileAct);
    chatToolbar->addAction(soundAct);
    chatToolbar->addAction(videoAct);
    chatToolbar->addAction(liveAct);
    chatToolbar->addAction(payAct);
    chatToolbar->setIconSize(QSize(24,24));
}
//! [30]
//! [32]
void MainWindow::createStatusBar()
//! [32] //! [33]
{
    statusBar()->showMessage(tr("Ready"));
}
//! [33]
void MainWindow::onSendBtnClicked()
{
    QString qsMsg;
    qsMsg = textEdit->toPlainText();
    if (qsMsg.isEmpty()){
        statusBar()->showMessage(tr("no message to send"), 3000);
        return ;
    }
    // If you have not setup a QTextCodec for QString & C-String(ANSI-MB)
    // toLatin1() ignore any codec;
    // toLocal8Bit use QTextCodec::codecForLocale(),
    // toAscii() use QTextCodec::setCodecForCStrings()
    // toUtf8() use UTF8 codec
    // QTextCodec::codecForLocale() guess the MOST suitable codec for current locale,
    // if application has not set codec for locale by setCodecForLocale;
    std::string sMsg = qsMsg.toStdString();
    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    if (blist->currentItem() == NULL){
        statusBar()->showMessage(tr("no current buddy selected"), 3000);
        return ;
    }
    QString qsTo = blist->currentItem()->data(Qt::UserRole).toString();

    QJsonObject joMsg;
    joMsg["time"]   = QDateTime::currentDateTime().toMSecsSinceEpoch();
    joMsg["latitude"]    = 0.0;
    joMsg["longtitude"]  = 0.0;
    joMsg["client"] = "qt";
    joMsg["version"]= (int)m_bitmail->GetVersion();
    joMsg["type"]   = "user"; // others e.g. "system"
    joMsg["msg"]    = qsMsg;

    QJsonDocument jdoc(joMsg);

    QString qsWrappedMsg = jdoc.toJson();

    emit readyToSend(qsTo, qsWrappedMsg);
    populateMessage(true
                    , qsFrom
                    , qsTo
                    , qsWrappedMsg
                    , QString::fromStdString(m_bitmail->GetID())
                    , QString::fromStdString(m_bitmail->GetCert()));

    textEdit->clear();
    // There are bugs in Qt-Creator's memory view utlity;
    // open <address> in memory window,
    // select the value of the <address>,
    // jump to <value>(little-endien) in a NEW memory window, note: NEW
    return ;
}
void MainWindow::onPayBtnClicked()
{
    PayDialog payDialog;
    if (payDialog.exec() != QDialog::Accepted){
        return ;
    }
    return ;
}
void MainWindow::onConfigBtnClicked()
{
    OptionDialog optDialog(false, this);
    optDialog.SetEmail(QString::fromStdString(m_bitmail->GetEmail()));
    optDialog.SetNick(QString::fromStdString(m_bitmail->GetNick()));
    optDialog.SetPassphrase(QString::fromStdString(m_bitmail->GetPassphrase()));
    optDialog.SetBits(m_bitmail->GetBits());
    optDialog.SetSmtpUrl(QString::fromStdString(m_bitmail->GetTxUrl()));
    optDialog.SetSmtpLogin(QString::fromStdString(m_bitmail->GetTxLogin()));
    optDialog.SetSmtpPassword(QString::fromStdString(m_bitmail->GetTxPassword()));
    optDialog.SetImapUrl(QString::fromStdString(m_bitmail->GetRxUrl()));
    optDialog.SetImapLogin(QString::fromStdString(m_bitmail->GetRxLogin()));
    optDialog.SetImapPassword(QString::fromStdString(m_bitmail->GetRxPassword()));
    optDialog.SetProxyEnable(m_bitmail->EnableProxy());
    optDialog.SetProxyIP(QString::fromStdString(m_bitmail->GetProxyIp()));
    optDialog.SetProxyPort(m_bitmail->GetProxyPort());
    optDialog.SetProxyLogin(QString::fromStdString(m_bitmail->GetProxyUser()));
    optDialog.SetProxyPassword(QString::fromStdString(m_bitmail->GetProxyPassword()));
    optDialog.SetRemoteDNS(m_bitmail->RemoteDNS());
    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }
    QString qsPassphrase = optDialog.GetPassphrase();
    m_bitmail->SetPassphrase(qsPassphrase.toStdString());
    QString qsTxUrl = optDialog.GetSmtpUrl();
    QString qsTxLogin = optDialog.GetSmtpLogin();
    QString qsTxPassword = optDialog.GetSmtpPassword();
    QString qsRxUrl = optDialog.GetImapUrl();
    QString qsRxLogin = optDialog.GetImapLogin();
    QString qsRxPassword = optDialog.GetImapPassword();
    m_bitmail->InitNetwork(qsTxUrl.toStdString()
                           , qsTxLogin.toStdString()
                           , qsTxPassword.toStdString()
                           , qsRxUrl.toStdString()
                           , qsRxLogin.toStdString()
                           , qsRxPassword.toStdString());
    m_bitmail->EnableProxy(optDialog.GetProxyEnable());
    do {
        m_bitmail->SetProxyIp(optDialog.GetProxyIP().toStdString());
        m_bitmail->SetProxyPort(optDialog.GetProxyPort());
        m_bitmail->SetProxyUser(optDialog.GetProxyLogin().toStdString());
        m_bitmail->SetProxyPassword(optDialog.GetProxyPassword().toStdString());
        m_bitmail->RemoteDNS(optDialog.GetRemoteDNS());
    }while(0);
    return ;
}
//! [15]
void MainWindow::documentWasModified()
//! [15] //! [16]
{
}
//! [16]
void MainWindow::onWalletBtnClicked()
{
}
void MainWindow::populateMessage(bool fTx
                                 , const QString &from
                                 , const QString & to
                                 , const QString & msg
                                 , const QString & certid
                                 , const QString & cert)
{
    bool fIsBuddy =  (m_bitmail->IsFriend(from.toStdString(), cert.toStdString()));
    QString qsFromNick = fTx ? QString::fromStdString(m_bitmail->GetNick())
                             : QString::fromStdString(m_bitmail->GetFriendNick(from.toStdString()));
    QString qsToNick = fTx ? QString::fromStdString(m_bitmail->GetFriendNick(to.toStdString()))
                           : QString::fromStdString(m_bitmail->GetNick());
    QString qsDisp = FormatBMMessage(fTx, from, qsFromNick, to, qsToNick, msg);
    QListWidgetItem * msgElt = new QListWidgetItem(QIcon(":/images/bubble.png")
                                                   , qsDisp);
    msgElt->setFlags((Qt::ItemIsSelectable
                      | Qt::ItemIsEditable)
                      | Qt::ItemIsEnabled);
    msgElt->setBackgroundColor((fTx)
                               ? (Qt::lightGray)
                               : (fIsBuddy ? (QColor(Qt::green).lighter()) : (Qt::red)));
    QStringList vecMsgData;
    vecMsgData.push_back(fTx ? "tx" : "rx");
    vecMsgData.push_back(from);
    vecMsgData.push_back(msg);
    vecMsgData.push_back(certid);
    vecMsgData.push_back(cert);
    (void)vecMsgData;
    msgElt->setData(Qt::UserRole, QVariant(vecMsgData));
    msgView->addItem(msgElt);
    msgView->scrollToBottom();
    return;
}
void MainWindow::onNewMessage(const QString &from, const QString &msg, const QString & certid, const QString &cert)
{
    //TODO: cert check and buddy management
    (void) cert;
    QString qsTo = QString::fromStdString(m_bitmail->GetEmail());
    //show message
    populateMessage(false, from, qsTo, msg, certid, cert);
    //GUI notification
    activateWindow();
}
void MainWindow::populateBuddies()
{
    // Add buddies
    std::vector<std::string> vecEmails;
    m_bitmail->GetFriends(vecEmails);
    for (std::vector<std::string>::const_iterator it = vecEmails.begin()
         ; it != vecEmails.end(); ++it)
    {
        std::string sBuddyNick = m_bitmail->GetFriendNick(*it);
        QString qsNick = QString::fromStdString(sBuddyNick);
        populateBuddy(QString::fromStdString(*it), qsNick);
    }
    return ;
}
void MainWindow::populateBuddy(const QString &email, const QString &nick)
{
    QString qsItemDisplay = nick;
    qsItemDisplay += "\n";
    qsItemDisplay += "(";
    qsItemDisplay += email;
    qsItemDisplay += ")";
    QListWidgetItem *buddy = new QListWidgetItem(QIcon(":/images/head.png"), qsItemDisplay);
    buddy->setData(Qt::UserRole, QVariant(email));
    blist->addItem(buddy);
    return ;
}
void MainWindow::clearMsgView()
{
    return ;
}
void MainWindow::populateMsgView(const QString &email)
{
    (void )email;
    return ;
}
void MainWindow::onCurrentBuddy(QListWidgetItem * current, QListWidgetItem * previous)
{
    (void)current;
    (void)previous;
    btnSend->setEnabled(true);
}
void MainWindow::onInviteBtnClicked()
{
    InviteDialog inviteDialog(this);
    if (QDialog::Accepted != inviteDialog.exec()){
        return ;
    }
    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsEmail = inviteDialog.GetEmail();
    QString qsWhisper = inviteDialog.GetWhisper();

    QJsonObject joMsg;
    joMsg["time"]   = QDateTime::currentDateTime().toMSecsSinceEpoch();
    joMsg["latitude"]    = 0.0;
    joMsg["longtitude"]  = 0.0;
    joMsg["client"] = "qt";
    joMsg["version"]= (int)m_bitmail->GetVersion();
    joMsg["type"]   = "user"; // others e.g. "system"
    joMsg["msg"]    = qsWhisper;
    QJsonDocument jdoc(joMsg);

    QString qsWrappedMsg = jdoc.toJson();

    if (!qsEmail.isEmpty()){

        emit readyToSend(qsEmail, qsWrappedMsg);

        populateMessage(true
                        , qsFrom
                        , qsEmail
                        , qsWrappedMsg
                        , QString::fromStdString(m_bitmail->GetID())
                        , QString::fromStdString(m_bitmail->GetCert()));
    }
    return ;
}
void MainWindow::onMessageDoubleClicked(QListWidgetItem * actItem)
{
    (void)actItem;
    QStringList vecMsgData = actItem->data(Qt::UserRole).toStringList();
    QString qsFlag = vecMsgData.front();
    if (qsFlag == "tx"){
        return ;
    }
    vecMsgData.pop_front();
    QString qsFrom = vecMsgData.front();
    vecMsgData.pop_front();
    QString qsMessage = vecMsgData.front();
    vecMsgData.pop_front();    
    QString qsCertID = vecMsgData.front();
    vecMsgData.pop_front();
    QString qsCert = vecMsgData.front();
    vecMsgData.pop_front();
    MessageDialog messageDialog(m_bitmail);
    messageDialog.SetFrom(qsFrom);
    messageDialog.SetMessage(qsMessage);
    messageDialog.SetCertID(qsCertID);
    messageDialog.SetCert(qsCert);
    connect(&messageDialog, SIGNAL(signalAddFriend(QString))
            , this, SLOT(onAddFriend(QString)));
    if (messageDialog.exec() != QDialog::Accepted){
        return ;
    }
    return ;
}
void MainWindow::onAddFriend(const QString &email)
{
    if (blist->findItems(email, Qt::MatchContains).size()){
        // Already exist;
        return ;
    }
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateBuddy(email, qsNick);
}
QString MainWindow::FormatBMMessage(bool fTx
                                    , const QString &from
                                    , const QString &fromnick
                                    , const QString &to
                                    , const QString &tonick
                                    , const QString &msg)
{
    QString qsFullFrom = "";
    QString qsFullTo = "";
    if (fTx){
        qsFullFrom += "[";
        qsFullFrom += tr("me");
        qsFullFrom += "]";
        qsFullTo += "[";
        qsFullTo += tonick;
        qsFullTo += "(";
        qsFullTo += to;
        qsFullTo += ")";
        qsFullTo += "]";
    }else{
        qsFullFrom += "[";
        qsFullFrom += fromnick;
        qsFullFrom += "(";
        qsFullFrom += from;
        qsFullFrom += ")";
        qsFullFrom += "]";
        qsFullTo += "[";
        qsFullTo += tr("me");
        qsFullTo += "]";
    }
    QString qsNow = QDateTime::currentDateTime().toString();
    QString qsMessageDisplay = qsNow;
    qsMessageDisplay += "\n";
    qsMessageDisplay += qsFullFrom;
    qsMessageDisplay += tr(" Say: ");
    qsMessageDisplay += qsFullTo;
    qsMessageDisplay += "\n\n";
    qsMessageDisplay += msg;
    qsMessageDisplay += "\n\n";
    return qsMessageDisplay;
}
