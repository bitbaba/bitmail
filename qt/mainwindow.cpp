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
#include "walletdialog.h"
#include "rssdialog.h"
#include "main.h"

#define TAG_PER      ("@")
#define TAG_GRP      ("#")
#define TAG_SUB      ("$")
#define KEY_STRANGER ("?")

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

    btree = new QTreeWidget;
    btree->setIconSize(QSize(48,48));
    btree->setFixedWidth(280);
    btree->setColumnCount(1);
    btree->setFont(QFont("Arial", 8));
    QStringList columns;
    columns.append(tr("Contact"));
    btree->setHeaderLabels(columns);

    nodeFriends = new QTreeWidgetItem(btree, QStringList(tr("Friends")));
    nodeFriends->setIcon(0, QIcon(":/images/head.png"));
    nodeFriends->setData(0, Qt::UserRole, "Cat::Friends");
    populateFriendTree(nodeFriends);
    btree->addTopLevelItem(nodeFriends);

    nodeGroups = new QTreeWidgetItem(btree, QStringList(tr("Groups")));
    nodeGroups->setIcon(0, QIcon(":/images/group.png"));
    nodeFriends->setData(0, Qt::UserRole, "Cat::Groups");
    populateGroupTree(nodeGroups);
    btree->addTopLevelItem(nodeGroups);

    nodeSubscribes = new QTreeWidgetItem(btree, QStringList(tr("Subscribes")));
    nodeSubscribes->setIcon(0, QIcon(":/images/subscribe.png"));
    nodeFriends->setData(0, Qt::UserRole, "Cat::Subscribes");
    populateSubscribeTree(nodeSubscribes);
    btree->addTopLevelItem(nodeSubscribes);

    leftLayout->addWidget(btree);

    mainLayout->addLayout(leftLayout);
    msgView = new QListWidget;
    msgView->setIconSize(QSize(48,48));
    msgView->setSpacing(2);
    msgView->setFont(QFont("Arial", 8));

    textEdit = new QPlainTextEdit;
    textEdit->setMinimumWidth(480);
    textEdit->setFixedHeight(100);
    textEdit->setFocus();
    textEdit->setFont(QFont("Arial", 8));

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
    wrap->setMinimumHeight(600);
    wrap->setLayout(mainLayout);
    setCentralWidget(wrap);

    //register custom enum type <MsgType> for signal/slot;
    qRegisterMetaType<MsgType>();

    // Add signals
    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    connect(btree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*))
            , this, SLOT(onTreeCurrentBuddy(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(btree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int))
            , this, SLOT(onTreeBuddyDoubleClicked(QTreeWidgetItem*,int)));

    connect(msgView, SIGNAL(itemDoubleClicked(QListWidgetItem*))
            , this, SLOT(onMessageDoubleClicked(QListWidgetItem*)));

    connect(btnSend, SIGNAL(clicked())
            , this, SLOT(onSendBtnClicked()));

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

    connect(m_pollth, SIGNAL(inboxPollEvent())
            , m_rxth, SLOT(onInboxPollEvent()));

    connect(m_pollth, SIGNAL(done())
            , this, SLOT(onPollDone()));

    connect(m_pollth, SIGNAL(inboxPollProgress(QString))
            , this, SLOT(onPollProgress(QString)));

    connect(m_rxth, SIGNAL(gotMessage(QString, QStringList,QString, QString,QString))
            , this, SLOT(onNewMessage(QString, QStringList,QString, QString,QString)));

    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));
    connect(m_rxth, SIGNAL(rxProgress(QString)), this, SLOT(onRxProgress(QString)));

    connect(this, SIGNAL(readyToSend(QString,QStringList,QString))
            , m_txth, SLOT(onSendMessage(QString,QStringList,QString)));

    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));
    connect(m_txth, SIGNAL(txProgress(QString)), this, SLOT(onTxProgress(QString)));

    m_pollth->start();
    m_rxth->start();
    m_txth->start();
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
void MainWindow::onTreeBuddyDoubleClicked(QTreeWidgetItem *actItem, int col)
{
    // Process leaf nodes
    if (actItem == NULL){
        return ;
    }
    if (col != 0){
        return ;
    }
    QVariant qvData = actItem->data(0, Qt::UserRole);
    if (!qvData.isValid()|| qvData.isNull()){
        return ;
    }
    QString qvDataTypeName = qvData.typeName();
    if (qvDataTypeName != "QStringList"){
        return ;
    }
    QStringList qslData = qvData.toStringList();
    QString qsType = qslData.at(0);
    if (qsType == TAG_GRP){
        return ;
    }
    QString qsEmail = qslData.at(1);
    if (qsEmail.isEmpty() || qsEmail == KEY_STRANGER){
        return ;
    }
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(qsEmail.toStdString()));
    QString qsCertID = QString::fromStdString(m_bitmail->GetFriendID(qsEmail.toStdString()));
    CertDialog certDialog(m_bitmail, this);

    do {
        connect(&certDialog, SIGNAL(newSubscribe(QString)), this, SLOT(onNewSubscribe(QString)));
    }while(0);

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
        connect(walletAct, SIGNAL(triggered()), this, SLOT(onWalletBtnClicked()));
    }while(0);
    do{
        rssAct = new QAction(QIcon(":/images/subscribe.png"), tr("&rss"), this);
        rssAct->setStatusTip(tr("RSS"));
        connect(rssAct, SIGNAL(triggered()), this, SLOT(onRssBtnClicked()));
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
    rssToolbar = addToolBar(tr("RSS"));
    rssToolbar->addAction(rssAct);
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
    walletDialog walletdlg(this);
    if (walletdlg.exec() != QDialog::Accepted){
        return ;
    }
}
void MainWindow::onRssBtnClicked()
{
    RssDialog rssDialog(m_bitmail, this);
    rssDialog.exec();
}

/**
 * @Note: lesson about signal/slot paramters match
 * 1) emit signal(MsgType), slot(MsgType) will not called, if MsgType is a custom enumerate type.
 * 2) in the case1, Q_DECLARE_METATYPE(MsgType) & aRegisterMetaType<MsgType>(), will make case1 to work.
 */
void MainWindow::onNewMessage(const QString & from
                              , const QStringList & recip
                              , const QString & content
                              , const QString & certid
                              , const QString & cert)
{
    RTXMessage rtxMsg;
    rtxMsg.rtx(false);
    rtxMsg.from(from);
    rtxMsg.recip(recip);
    rtxMsg.content(content);
    rtxMsg.certid(certid);
    rtxMsg.cert(cert);
    populateMessage(rtxMsg);
}

void MainWindow::populateMessage(const RTXMessage & rtxMsg)
{
    QListWidgetItem * msgElt = new QListWidgetItem();
    msgElt->setIcon(QIcon(":/images/bubble.png"));
    msgElt->setText(rtxMsg.content());
    msgElt->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    msgElt->setBackgroundColor((rtxMsg.isTx())? (Qt::lightGray) : (QColor(Qt::green).lighter()));
    msgElt->setData(Qt::UserRole, QVariant(rtxMsg.Serialize()));
    msgView->addItem(msgElt);
    msgView->scrollToBottom();
    return;
}

void MainWindow::onMessageDoubleClicked(QListWidgetItem * actItem)
{
    if (!actItem){
        return ;
    }
    QVariant qvData = actItem->data(Qt::UserRole);
    QString qvDataType = qvData.typeName();
    if (qvDataType != "QString"){
        return ;
    }
    QString qsMsg = qvData.toString();

    QString qsFrom, qsContent, qsCertID, qsCert;
    do {
        RTXMessage rtxMsg;
        if (!rtxMsg.Load(qsMsg)){
            return ;
        }
        if (rtxMsg.isTx()){
            return ;
        }
        qsFrom = rtxMsg.from();
        // TODO: parse qsContent => {PeerMessage|GroupMessage|SubMessage}
        qsContent = rtxMsg.content();
        qsCertID = rtxMsg.certid();
        qsCert = rtxMsg.cert();
    }while(0);

    MessageDialog messageDialog(m_bitmail);
    messageDialog.SetFrom(qsFrom);
    messageDialog.SetMessage(qsContent);
    messageDialog.SetCertID(qsCertID);
    messageDialog.SetCert(qsCert);

    connect(&messageDialog, SIGNAL(signalAddFriend(QString))
            , this, SLOT(onAddFriend(QString)));
    if (messageDialog.exec() != QDialog::Accepted){
        return ;
    }
    return ;
}
//! [33]
void MainWindow::onSendBtnClicked()
{
    // If you have not setup a QTextCodec for QString & C-String(ANSI-MB)
    // toLatin1() ignore any codec;
    // toLocal8Bit use QTextCodec::codecForLocale(),
    // toAscii() use QTextCodec::setCodecForCStrings()
    // toUtf8() use UTF8 codec
    // QTextCodec::codecForLocale() guess the MOST suitable codec for current locale,
    // if application has not set codec for locale by setCodecForLocale;
    QString qsMsg;
    qsMsg = textEdit->toPlainText();
    textEdit->clear();
    if (qsMsg.isEmpty()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());

    MsgType mt;
    QString qsKey;
    if (!getCurrentRecipKey(mt, qsKey)){
        return ;
    }
    if (qsKey == KEY_STRANGER){
        return ;
    }
    QStringList qslRecip;
    getRecipList(mt, qsKey, qslRecip);

    RTXMessage rtxMsg;
    BMMessage bmMsg;
    bmMsg.msgType(mt);
    if (mt == mt_peer){
        PeerMessage peerMsg;
        peerMsg.content(qsMsg);
        bmMsg.content(peerMsg.Serialize());
    }else if (mt == mt_group){
        GroupMessage groupMsg;
        groupMsg.groupId(qsKey);
        std::string sGroupName;
        m_bitmail->GetGroupName(qsKey.toStdString(), sGroupName);
        groupMsg.groupName(QString::fromStdString(sGroupName));
        groupMsg.content(qsMsg);
        bmMsg.content(groupMsg.Serialize());
    }else if (mt == mt_subscribe){
        SubMessage subMsg;
        subMsg.refer("");
        subMsg.content(qsMsg);
        bmMsg.content(subMsg.Serialize());
    }else{
        return ;
    }
    rtxMsg.rtx(true);
    rtxMsg.from(qsFrom);
    rtxMsg.certid(qsCertId);
    rtxMsg.cert(qsCert);
    rtxMsg.recip(qslRecip);
    rtxMsg.content(bmMsg.Serialize());

    emit readyToSend(qsFrom
                     , qslRecip
                     , rtxMsg.content());

    populateMessage(rtxMsg);

    // There are bugs in Qt-Creator's memory view utlity;
    // open <address> in memory window,
    // select the value of the <address>,
    // jump to <value>(little-endien) in a NEW memory window, note: NEW
    return ;
}

void MainWindow::onInviteBtnClicked()
{
    InviteDialog inviteDialog(this);
    if (QDialog::Accepted != inviteDialog.exec()){
        return ;
    }
    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsRecip = inviteDialog.GetEmail();
    QString qsWhisper = inviteDialog.GetWhisper();
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());
    QStringList qslRecip;
    qslRecip.append(qsRecip);

    RTXMessage rtxMsg;
    BMMessage bmMsg;
    bmMsg.msgType(mt_peer);
    PeerMessage peerMsg;
    peerMsg.content(qsWhisper);
    bmMsg.content(peerMsg.Serialize());

    rtxMsg.rtx(true);
    rtxMsg.from(qsFrom);
    rtxMsg.certid(qsCertId);
    rtxMsg.cert(qsCert);
    rtxMsg.recip(qslRecip);
    rtxMsg.content(bmMsg.Serialize());

    emit readyToSend(qsFrom
                     , qslRecip
                     , rtxMsg.content());
    return ;
}

void MainWindow::setNotify(QTreeWidgetItem *item, int no)
{
    (void)no;
    item->setBackground(0, QBrush(QColor(Qt::red)));
}

bool MainWindow::getCurrentRecipKey(MsgType & mt, QString & qsKey)
{
    if (btree->currentItem() == NULL){
        return false;
    }

    QVariant qvData = btree->currentItem()->data(0, Qt::UserRole);
    if (!qvData.isValid() || qvData.isNull()){
        return false;
    }

    QString qsVDataTyep = qvData.typeName();
    if (qsVDataTyep != "QStringList"){
        return false;
    }

    QStringList qslData = qvData.toStringList();
    QString qsType = qslData.at(0);
    if (qsType == TAG_PER){
        mt = mt_peer;
    }else if( qsType == TAG_GRP){
        mt = mt_group;
    }else if ( qsType == TAG_SUB){
        mt = mt_subscribe;
    }else{
        mt = mt_undef;
        return false;
    }

    qsKey = qslData.at(1);

    return true;
}

bool MainWindow::getRecipList(MsgType mt, const QString &qsKey, QStringList &reciplist)
{
    if (mt == mt_undef){
        return false;
    }
    if (mt == mt_peer || mt == mt_subscribe){
        reciplist.append(qsKey);
    }else{
        std::vector<std::string> vecMembers;
        m_bitmail->GetGroupMembers(qsKey.toStdString(), vecMembers);
        for (std::vector<std::string>::const_iterator it = vecMembers.begin()
             ; it != vecMembers.end()
             ; ++it){
            reciplist.append(QString::fromStdString(*it));
        }
    }
    return true;
}

void MainWindow::populateFriendTree(QTreeWidgetItem * node)
{
    // Add buddies
    std::vector<std::string> vecEmails;
    m_bitmail->GetFriends(vecEmails);
    for (std::vector<std::string>::const_iterator it = vecEmails.begin()
         ; it != vecEmails.end(); ++it)
    {
        std::string sBuddyNick = m_bitmail->GetFriendNick(*it);
        QString qsNick = QString::fromStdString(sBuddyNick);
        populateFriendLeaf(node, QString::fromStdString(*it), qsNick);
    }
    // add stranger node
    populateFriendLeaf(node, KEY_STRANGER, tr("Strangers"));
    return ;
}
void MainWindow::populateGroupTree(QTreeWidgetItem * node)
{
    // Add Groups
    std::vector<std::string> vecGroups;
    m_bitmail->GetGroups(vecGroups);
    for (std::vector<std::string>::const_iterator it = vecGroups.begin()
         ; it != vecGroups.end(); ++it)
    {
        QString qsGroupId = QString::fromStdString(*it);
        std::vector<std::string> vecMembers;
        m_bitmail->GetGroupMembers(qsGroupId.toStdString(), vecMembers);
        std::string sGroupName;
        m_bitmail->GetGroupName(qsGroupId.toStdString(), sGroupName);
        QString qsGroupName = QString::fromStdString(sGroupName);
        QStringList qslMembers;
        for(std::vector<std::string>::const_iterator it_member = vecMembers.begin()
            ; it_member != vecMembers.end(); it_member ++){
            qslMembers.append(QString::fromStdString(*it_member));
        }
        populateGroupLeaf(node, qsGroupId, qsGroupName);
    }
    populateGroupLeaf(node, KEY_STRANGER, tr("Strangers"));
    return ;
}
void MainWindow::populateSubscribeTree(QTreeWidgetItem * node)
{
    // Add buddies
    std::vector<std::string> vecSubscribes;
    m_bitmail->GetSubscribes(vecSubscribes);
    for (std::vector<std::string>::const_iterator it = vecSubscribes.begin()
         ; it != vecSubscribes.end(); ++it)
    {
        QString qsSub = QString::fromStdString(*it);
        (void)qsSub;
        QString qsNick = QString::fromStdString( m_bitmail->GetFriendNick(qsSub.toStdString()));
        populateSubscribeLeaf(node, qsSub, qsNick);
    }
    populateSubscribeLeaf(node, KEY_STRANGER, tr("Strangers"));
    return ;
}

void MainWindow::populateFriendLeaf(QTreeWidgetItem * node, const QString &email, const QString &nick)
{
    QStringList qslBuddy;
    qslBuddy.append(nick);
    QTreeWidgetItem *buddy = new QTreeWidgetItem(qslBuddy);
    if (email == KEY_STRANGER){
        buddy->setIcon(0, QIcon(":/images/stranger.png"));
    }else{
        buddy->setIcon(0, QIcon(":/images/head.png"));
    }
    QStringList qslData;
    qslData.append(TAG_PER);
    qslData.append(email);
    buddy->setData(0, Qt::UserRole, QVariant(qslData));
    node->addChild(buddy);
    return ;
}

void MainWindow::populateGroupLeaf(QTreeWidgetItem * node, const QString & groupid, const QString & groupname)
{
    QStringList qslBuddy;
    qslBuddy.append(groupname);
    QTreeWidgetItem * group = new QTreeWidgetItem(qslBuddy);
    if (groupid == KEY_STRANGER){
        group->setIcon(0, QIcon(":/images/stranger.png"));
    }else{
        group->setIcon(0, QIcon(":/images/group.png"));
    }
    QStringList qslData;
    qslData.append(TAG_GRP);
    qslData += groupid;
    group->setData(0, Qt::UserRole, QVariant(qslData));
    node->addChild(group);
    return ;
}

void MainWindow::populateSubscribeLeaf(QTreeWidgetItem * node, const QString & email, const QString &nick)
{
    QStringList qslBuddy;
    qslBuddy.append(nick);
    QTreeWidgetItem * sub = new QTreeWidgetItem(qslBuddy);
    if (email == KEY_STRANGER){
        sub->setIcon(0, QIcon(":/images/stranger.png"));
    }else{
        sub->setIcon(0, QIcon(":/images/subscribe.png"));
    }
    QStringList qslData;
    qslData.append(TAG_SUB);
    qslData.append(email);
    sub->setData(0, Qt::UserRole, QVariant(qslData));
    node->addChild(sub);
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

void MainWindow::onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    if (current == NULL){
        (void)previous;
        return ;
    }

    QVariant qvData = current->data(0, Qt::UserRole);
    if (qvData.isNull()){
        return ;
    }
    if (!qvData.isValid()){
        return ;
    }

    QString qsVDataType = qvData.typeName();
    if (qsVDataType != "QStringList" ){
        btnSend->setEnabled(false);
        return ;
    }

    //reset to default backgroud color, to remove notify
    current->setBackgroundColor(0, QColor(Qt::white));

    //clear current message view
    msgView->clear();

    QStringList qslData = qvData.toStringList();
    QString qsType = qslData.at(0);
    if (qsType == TAG_PER || qsType == TAG_GRP || qsType == TAG_SUB){
        btnSend->setEnabled(true);
    }

    /*Key: email or groupName*/
    QString qsKey = qslData.at(1);
    if (qsKey == KEY_STRANGER){
        btnSend->setEnabled(false);
    }
    return ;
}

void MainWindow::onAddFriend(const QString &email)
{
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateFriendLeaf(nodeFriends, email, qsNick);
}

void MainWindow::onNewSubscribe(const QString & email)
{
    if (btree->findItems(email, Qt::MatchContains).size()){
        // Already exist;
        return ;
    }
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateSubscribeLeaf(nodeSubscribes, email, qsNick);
}
