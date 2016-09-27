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
#include "netoptdialog.h"
#include "paydialog.h"
#include "shutdowndialog.h"
#include "certdialog.h"
#include "invitedialog.h"
#include "messagedialog.h"
#include "walletdialog.h"
#include "rssdialog.h"
#include "newgroupdialog.h"
#include "main.h"

#define TAG_PER      ("@")
#define TAG_GRP      ("#")
#define TAG_SUB      ("$")
#define KEY_STRANGER ("?")

//! [1]
MainWindow::MainWindow(BitMail * bitmail)
    : m_bitmail(bitmail)
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
    btree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btree, SIGNAL(customContextMenuRequested(QPoint))
            , this, SLOT(onCtxMenu(QPoint)));

    if (1){
        nodeFriends = new QTreeWidgetItem(btree, QStringList(tr("Friends")), NT_Friends);
        nodeFriends->setIcon(0, QIcon(":/images/head.png"));
        nodeFriends->setData(0, Qt::UserRole, "Cat::Friends");
        populateFriendTree(nodeFriends);
        btree->addTopLevelItem(nodeFriends);
    }

    if (1){
        nodeGroups = new QTreeWidgetItem(btree, QStringList(tr("Groups")), NT_Groups);
        nodeGroups->setIcon(0, QIcon(":/images/group.png"));
        nodeFriends->setData(0, Qt::UserRole, "Cat::Groups");
        populateGroupTree(nodeGroups);
        btree->addTopLevelItem(nodeGroups);
    }

    if (1){
        nodeSubscribes = new QTreeWidgetItem(btree, QStringList(tr("Subscribes")), NT_Subscribes);
        nodeSubscribes->setIcon(0, QIcon(":/images/subscribe.png"));
        nodeFriends->setData(0, Qt::UserRole, "Cat::Subscribes");
        populateSubscribeTree(nodeSubscribes);
        btree->addTopLevelItem(nodeSubscribes);
    }

    leftLayout->addWidget(btree);

    mainLayout->addLayout(leftLayout);

    sessLabel = new QLabel;
    sessLabel->setMaximumHeight(20);
    sessLabel->setAlignment(Qt::AlignCenter);
    sessLabel->setFont(QFont("FixedSys", 8));
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    sessLabel->setPalette(pa);
    sessLabel->setText("");

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
    rightLayout->addWidget(sessLabel);
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
}
//! [2]
MainWindow::~MainWindow()
{
}
void MainWindow::startupNetwork()
{
    m_rxth = (new RxThread(m_bitmail));
    connect(m_rxth, SIGNAL(gotMessage(QString, QString, QString,QString))
            , this, SLOT(onNewMessage(QString, QString, QString,QString)));
    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));
    connect(m_rxth, SIGNAL(rxProgress(QString)), this, SLOT(onRxProgress(QString)));
    m_rxth->start();

    m_txth = (new TxThread(m_bitmail));
    connect(this, SIGNAL(readyToSend(QString,QStringList,QString))
            , m_txth, SLOT(onSendMessage(QString,QStringList,QString)));
    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));
    connect(m_txth, SIGNAL(txProgress(QString)), this, SLOT(onTxProgress(QString)));
    m_txth->start();

    m_bitmail->StartBrad();
}
void MainWindow::shutdownNetwork()
{
    m_bitmail->ShutdownBrad();

    if (m_rxth ){
        m_rxth->stop();
    }
    if (m_txth) {
        m_txth->stop();
    }

}
void MainWindow::onRxDone()
{
    m_rxth->wait(1000); delete m_rxth; m_rxth = NULL;
    if (!m_rxth && !m_txth && m_shutdownDialog){
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

void MainWindow::onTxDone()
{
    m_txth->wait(1000); delete m_txth; m_txth = NULL;
    if (!m_rxth && !m_txth && m_shutdownDialog){
        m_shutdownDialog->done(0);
    }
}
//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    shutdownNetwork();
    /**
    * Model here3
    */
    if (m_rxth || m_txth){
        m_shutdownDialog = new ShutdownDialog(this);
        m_shutdownDialog->exec();
    }
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
    do {
        newGroup = new QAction(QIcon(":/images/newgroup.png"), tr("&NewGroup"), this);
        newGroup->setStatusTip(tr("Create a new Group"));
        connect(newGroup, SIGNAL(triggered()), this, SLOT(onNewGroupBtnClicked()));
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

    do{
        actInvite = new QAction(QIcon(":/images/invite.png"), tr("&Invite"), this);
        actInvite->setStatusTip(tr("Invite"));
        connect(actInvite, SIGNAL(triggered()), this, SLOT(onActInvite()));
    }while(0);

    do{
        actImport = new QAction(QIcon(":/images/add.png"), tr("&Import"), this);
        actImport->setStatusTip(tr("Import"));
        connect(actImport, SIGNAL(triggered()), this, SLOT(onActImport()));
    }while(0);

    do{
        actRemove = new QAction(QIcon(":/images/remove.png"), tr("&Remove"), this);
        actRemove->setStatusTip(tr("Remove"));
        connect(actRemove, SIGNAL(triggered()), this, SLOT(onActRemove()));
    }while(0);

    do{
        actRemoveGroup = new QAction(QIcon(":/images/remove.png"), tr("&Remove"), this);
        actRemoveGroup->setStatusTip(tr("RemoveGroup"));
        connect(actRemoveGroup, SIGNAL(triggered()), this, SLOT(onActRemoveGroup()));
    }while(0);

    do{
        actNewGroup = new QAction(QIcon(":/images/add.png"), tr("&Add"), this);
        actNewGroup->setStatusTip(tr("AddGroup"));
        connect(actNewGroup, SIGNAL(triggered()), this, SLOT(onActNewGroup()));
    }while(0);

    do{
        actNetwork = new QAction(QIcon(":/images/shutdown.png"), tr("&Startup"), this);
        actNetwork->setStatusTip(tr("Startup"));
        actNetwork->setCheckable(true);
        connect(actNetwork, SIGNAL(triggered(bool)), this, SLOT(onActNetwork(bool)));
    }while(0);

    do{
        actNetConfig = new QAction(QIcon(":/images/network.png"), tr("&Network"), this);
        actNetConfig->setStatusTip(tr("Network"));
        connect(actNetConfig, SIGNAL(triggered()), this, SLOT(onNetConfig()));
    }while(0);
}
//! [24]
//! [29] //! [30]
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(actNetwork);
    fileToolBar->addAction(configAct);
    fileToolBar->addAction(actNetConfig);

    //fileToolBar->addAction(actAssistant);

    editToolBar = addToolBar(tr("Buddies"));
    editToolBar->addAction(inviteAct);

    grpToolBar = addToolBar(tr("Groups"));
    grpToolBar->addAction(newGroup);

    rssToolbar = addToolBar(tr("RSS"));
    rssToolbar->addAction(rssAct);

    walletToolbar = addToolBar(tr("Wallet"));
    walletToolbar->addAction(payAct);
    walletToolbar->addAction(walletAct);


    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->setIconSize(QSize(24,24));
    //chatToolbar->addAction(emojAct);
    //chatToolbar->addAction(snapAct);
    //chatToolbar->addAction(fileAct);
    //chatToolbar->addAction(soundAct);
    //chatToolbar->addAction(videoAct);
    //chatToolbar->addAction(liveAct);
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
    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }
    QString qsPassphrase = optDialog.GetPassphrase();
    m_bitmail->SetPassphrase(qsPassphrase.toStdString());
}

void MainWindow::onNetConfig()
{
    NetOptionDialog optDialog(this);
    optDialog.SetSmtpUrl(QString::fromStdString(m_bitmail->GetTxUrl()));
    optDialog.SetSmtpLogin(QString::fromStdString(m_bitmail->GetTxLogin()));
    optDialog.SetSmtpPassword(QString::fromStdString(m_bitmail->GetTxPassword()));

    optDialog.SetImapUrl(QString::fromStdString(m_bitmail->GetRxUrl()));
    optDialog.SetImapLogin(QString::fromStdString(m_bitmail->GetRxLogin()));
    optDialog.SetImapPassword(QString::fromStdString(m_bitmail->GetRxPassword()));

    optDialog.BradPort(m_bitmail->GetBradPort());
    optDialog.BradExtUrl(QString::fromStdString(m_bitmail->GetBradExtUrl()));

    optDialog.SetProxyIP(QString::fromStdString(m_bitmail->GetProxyIp()));
    optDialog.SetProxyPort(m_bitmail->GetProxyPort());
    optDialog.SetProxyLogin(QString::fromStdString(m_bitmail->GetProxyUser()));
    optDialog.SetProxyPassword(QString::fromStdString(m_bitmail->GetProxyPassword()));

    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }

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

    do {
        m_bitmail->SetProxyIp(optDialog.GetProxyIP().toStdString());
        m_bitmail->SetProxyPort(optDialog.GetProxyPort());
        m_bitmail->SetProxyUser(optDialog.GetProxyLogin().toStdString());
        m_bitmail->SetProxyPassword(optDialog.GetProxyPassword().toStdString());
    }while(0);

    do {
        m_bitmail->SetBradPort(optDialog.BradPort());
        m_bitmail->SetBradExtUrl(optDialog.BradExtUrl().toStdString());
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
                              , const QString & content
                              , const QString & certid
                              , const QString & cert)
{
    BMMessage bmMsg;
    if (!bmMsg.Load(content)){
        qDebug() << "Invalid BMMessage";
        return ;
    }
    QString qsFriendBradExtUrl = bmMsg.bradExtUrl();
    if (!from.isEmpty() && !qsFriendBradExtUrl.isEmpty()){
        m_bitmail->SetFriendBrad(from.toStdString(), qsFriendBradExtUrl.toStdString());
        qDebug() << QString("Setup Brad Mapping [%1]=>[%2]").arg(from).arg(qsFriendBradExtUrl);
    }
    QString qsKey = from;
    if (bmMsg.msgType() == mt_peer){
        if (!m_bitmail->IsFriend(from.toStdString(), cert.toStdString())){
            qsKey = KEY_STRANGER;
        }
    }else
    if (bmMsg.msgType() == mt_group){
        GroupMessage groupMsg;
        if (!groupMsg.Load(bmMsg.content())){
            qDebug() << "Invalid GroupMessage";
            return ;
        }
        QString qsGroupId = groupMsg.groupId();
        qsKey = qsGroupId;
        if (!m_bitmail->HasGroup(qsGroupId.toStdString())){
            qsKey = KEY_STRANGER;
        }
    }else
    if (bmMsg.msgType() == mt_subscribe) {
        if (!m_bitmail->IsFriend(from.toStdString(), cert.toStdString())){
            qsKey = KEY_STRANGER;
        }
    }else{
        qDebug() << "Invalid BMMessage::MsgType";
        return;
    }

    RTXMessage rtxMsg;
    rtxMsg.rtx(false);
    rtxMsg.from(from);
    rtxMsg.recip(QStringList(QString::fromStdString(m_bitmail->GetEmail())));
    rtxMsg.content(bmMsg);
    rtxMsg.certid(certid);
    rtxMsg.cert(cert);

    enqueueMsg(bmMsg.msgType(), qsKey, rtxMsg);

    MsgType mt0;
    QString key0;
    getCurrentRecipKey(mt0, key0);
    if (mt0 != mt_undef
            && mt0 == bmMsg.msgType()
            && !key0.isEmpty()
            && key0 == qsKey){
        populateMessage(rtxMsg);
    }else{
        setNotify(bmMsg.msgType(), qsKey);
    }
}

void MainWindow::enqueueMsg(MsgType mt, const QString &key, const RTXMessage &rtxMsg)
{
    if (mt == mt_peer){
        m_peermsgQ[key].append(rtxMsg.Serialize());
    }else if (mt == mt_group){
        m_groupmsgQ[key].append(rtxMsg.Serialize());
    }else if (mt == mt_subscribe){
        m_submsgQ[key].append(rtxMsg.Serialize());
    }else{
        return ;
    }
}

QList<RTXMessage> MainWindow::dequeueMsg(MsgType mt, const QString &key)
{
    QList<RTXMessage> rtxList;
    std::map<QString, QStringList> * ptr;
    if (mt == mt_peer){
        ptr = &m_peermsgQ;
    }else if (mt == mt_group){
        ptr = &m_groupmsgQ;
    }else if (mt == mt_subscribe){
        ptr = &m_submsgQ;
    }else{
        return rtxList;
    }
    for (std::map<QString, QStringList>::const_iterator it = ptr->begin()
         ; it != ptr->end()
         ; ++it){
        if (it->first == key){
            QStringList slist = it->second;
            for (int i = 0; i < slist.length(); ++i){
                QString smsg = slist.at(i);
                RTXMessage rtxMsg;
                if (rtxMsg.Load(smsg)){
                    rtxList.append(rtxMsg);
                }
            }
            break;
        }
    }
    return rtxList;
}

void MainWindow::populateMessage(const RTXMessage & rtxMsg)
{
    QListWidgetItem * msgElt = new QListWidgetItem();
    msgElt->setIcon(QIcon(":/images/bubble.png"));
    msgElt->setText(formatRTXMessage(rtxMsg));
    msgElt->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    msgElt->setBackgroundColor((rtxMsg.isTx())? (Qt::lightGray) : (QColor(Qt::green).lighter()));
    msgElt->setData(Qt::UserRole, QVariant(rtxMsg.Serialize()));
    msgView->addItem(msgElt);
    msgView->scrollToBottom();
    return;
}

QString MainWindow::formatRTXMessage(const RTXMessage &rtxMsg)
{
    QString qsShow = rtxMsg.Serialize();
    BMMessage bmMsg;
    if (!bmMsg.Load(rtxMsg.content())){
        return qsShow;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetFriendNick(rtxMsg.from().toStdString()));

    QString qsContent;
    MsgType mt = bmMsg.msgType();
    if (mt == mt_group){
        GroupMessage groupMsg;
        if (!groupMsg.Load(bmMsg.content())){
            return qsShow;
        }
        qsContent = groupMsg.content();
    }
    else if (mt == mt_peer){
        PeerMessage peerMsg;
        if (!peerMsg.Load(bmMsg.content())){
            return qsShow;
        }
        qsContent = peerMsg.content();
    }else if (mt == mt_subscribe){
        SubMessage subMsg;
        if (!subMsg.Load(bmMsg.content())){
            return  qsShow;
        }
        qsContent = subMsg.content();
    }

    QString qsTime = QDateTime::currentDateTime().toLocalTime().toString();

    qsShow = QString("[%1] - (%2)\r\n\r\n%3\r\n\r\n")
                            .arg(qsTime)
                            .arg(qsFrom)
                            .arg(qsContent.length() > 32 ? (qsContent.mid(0, 32) + tr("...")) : (qsContent));
    return qsShow;
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

    BMMessage bmMsg;
    QString qsFrom, qsCertID, qsCert;
    do {
        RTXMessage rtxMsg;
        if (!rtxMsg.Load(qsMsg)){
            return ;
        }
        if (rtxMsg.isTx()){
            return ;
        }
        if (!bmMsg.Load(rtxMsg.content())){
            return ;
        }
        qsFrom = rtxMsg.from();
        qsCertID = rtxMsg.certid();
        qsCert = rtxMsg.cert();
    }while(0);

    QString qsContent;
    if (bmMsg.msgType() == mt_peer){
        PeerMessage peerMsg;
        if (!peerMsg.Load(bmMsg.content())){
            return ;
        }
        qsContent = peerMsg.content();
    }else if (bmMsg.msgType() == mt_group){
        GroupMessage groupMsg;
        if (!groupMsg.Load(bmMsg.content())){
            return ;
        }
        qsContent = groupMsg.content();
    }else if (bmMsg.msgType() == mt_subscribe){
        SubMessage subMsg;
        if (!subMsg.Load(bmMsg.content())){
            return ;
        }
        qsContent = subMsg.content();
    }else{
        return ;
    }

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
    if (m_txth == NULL){
        statusBar()->showMessage(tr("No active network"));
        return ;
    }
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
    bmMsg.bradExtUrl(QString::fromStdString(m_bitmail->GetBradExtUrl()));
    if (mt == mt_peer){
        PeerMessage peerMsg;
        peerMsg.content(qsMsg);
        bmMsg.content(peerMsg);
    }else if (mt == mt_group){
        GroupMessage groupMsg;
        groupMsg.groupId(qsKey);
        std::string sGroupName;
        m_bitmail->GetGroupName(qsKey.toStdString(), sGroupName);
        groupMsg.groupName(QString::fromStdString(sGroupName));
        groupMsg.content(qsMsg);
        bmMsg.content(groupMsg);
    }else if (mt == mt_subscribe){
        SubMessage subMsg;
        subMsg.refer("");
        subMsg.content(qsMsg);
        bmMsg.content(subMsg);
    }else{
        return ;
    }
    rtxMsg.rtx(true);
    rtxMsg.from(qsFrom);
    rtxMsg.certid(qsCertId);
    rtxMsg.cert(qsCert);
    rtxMsg.recip(qslRecip);
    rtxMsg.content(bmMsg);
    qDebug() << rtxMsg.content();

    emit readyToSend(qsFrom
                     , qslRecip
                     , bmMsg.Serialize());

    enqueueMsg(mt, qsKey, rtxMsg);

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
    bmMsg.bradExtUrl(QString::fromStdString(m_bitmail->GetBradExtUrl()));
    PeerMessage peerMsg;
    peerMsg.content(qsWhisper);
    bmMsg.content(peerMsg);

    rtxMsg.rtx(true);
    rtxMsg.from(qsFrom);
    rtxMsg.certid(qsCertId);
    rtxMsg.cert(qsCert);
    rtxMsg.recip(qslRecip);
    rtxMsg.content(bmMsg);

    emit readyToSend(qsFrom
                     , qslRecip
                     , bmMsg.Serialize());
    return ;
}

void MainWindow::onNewGroupBtnClicked()
{
    NewGroupDialog newGroupDialog;
    if (newGroupDialog.exec() != QDialog::Accepted){
        return ;
    }
    return ;
}

void MainWindow::setNotify(MsgType mt, const QString & qsKey)
{
    QTreeWidgetItem * node = NULL;
    if (mt == mt_peer){
        node = nodeFriends;
    }else if (mt == mt_group){
        node = nodeGroups;
    }else if (mt == mt_subscribe){
        node = nodeSubscribes;
    }else{
        return ;
    }
    for (int i = 0; i < node->childCount(); i++){
        QTreeWidgetItem * elt = node->child(i);
        QVariant qvData = elt->data(0, Qt::UserRole);
        QStringList qslData = qvData.toStringList();
        QString key = qslData.at(1);
        if (qsKey == key){
            elt->setBackground(0, QBrush(QColor(Qt::red)));
            this->activateWindow();
        }
    }
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
        populateFriendLeaf(node, QString::fromStdString(*it), qsNick, NT_Friend);
    }
    // add stranger node
    populateFriendLeaf(node, KEY_STRANGER, tr("Strangers"), NT_Stranger);
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
        populateGroupLeaf(node, qsGroupId, qsGroupName, NT_Group);
    }
    populateGroupLeaf(node, KEY_STRANGER, tr("Strangers"), NT_StrangerGroup);
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
        populateSubscribeLeaf(node, qsSub, qsNick, NT_Subscribe);
    }
    populateSubscribeLeaf(node, KEY_STRANGER, tr("Strangers"), NT_StrangerSub);
    return ;
}

void MainWindow::populateFriendLeaf(QTreeWidgetItem * node, const QString &email, const QString &nick, nodeType nt)
{
    QStringList qslBuddy;
    qslBuddy.append(nick);
    QTreeWidgetItem *buddy = new QTreeWidgetItem(node, qslBuddy, nt);
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

void MainWindow::populateGroupLeaf(QTreeWidgetItem * node, const QString & groupid, const QString & groupname, nodeType nt)
{
    QStringList qslBuddy;
    qslBuddy.append(groupname);
    QTreeWidgetItem * group = new QTreeWidgetItem(node, qslBuddy, nt);
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

void MainWindow::populateSubscribeLeaf(QTreeWidgetItem * node, const QString & email, const QString &nick, nodeType nt)
{
    QStringList qslBuddy;
    qslBuddy.append(nick);
    QTreeWidgetItem * sub = new QTreeWidgetItem(node, qslBuddy, nt);
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
    btnSend->setEnabled(false);
    sessLabel->setText("");
    msgView->clear();
    (void)previous;

    if (current == NULL){
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
        return ;
    }

    //reset to default backgroud color, to remove notify
    current->setBackgroundColor(0, QColor(Qt::white));

    MsgType mt = mt_undef;
    QStringList qslData = qvData.toStringList();
    QString qsType = qslData.at(0);
    if (qsType == TAG_PER || qsType == TAG_GRP || qsType == TAG_SUB){
        mt = qsType == TAG_PER ? mt_peer : ( qsType == TAG_GRP ? mt_group : mt_subscribe);
        btnSend->setEnabled(true);
    }

    /*Key: email or groupName*/
    QString qsKey = qslData.at(1);
    if (qsKey == KEY_STRANGER){
        btnSend->setEnabled(false);
    }

    /*Setup session label header*/
    if (qsKey == KEY_STRANGER){
        sessLabel->setText(tr("Strangers"));
    }else{
        if (mt == mt_group){
            std::string sGroupName;
            m_bitmail->GetGroupName(qsKey.toStdString(), sGroupName);
            sessLabel->setText(QString::fromStdString(sGroupName));
        }else if (mt == mt_peer || mt == mt_subscribe){
            QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(qsKey.toStdString()));
            sessLabel->setText(qsNick);
        }else{
            return ;
        }
    }

    QList<RTXMessage> rtxList = dequeueMsg(mt, qsKey);

    for (int i = 0; i < rtxList.length(); ++i){
        RTXMessage rtxMsg = rtxList.at(i);
        populateMessage(rtxMsg);
    }

    return ;
}

void MainWindow::onAddFriend(const QString &email)
{
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateFriendLeaf(nodeFriends, email, qsNick, NT_Friend);
}

void MainWindow::onNewSubscribe(const QString & email)
{
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateSubscribeLeaf(nodeSubscribes, email, qsNick, NT_Subscribe);
}

void MainWindow::onCtxMenu(const QPoint & pos)
{
    QTreeWidgetItem * elt = btree->itemAt(pos);
    if (elt != NULL){
        qDebug() << elt->text(0);
    }
    nodeType nt = NT_Undef;
    nt = (nodeType) elt->type();

    QMenu ctxMenu(this);
    if (nt == NT_Friends){
        ctxMenu.addAction(actInvite);
        ctxMenu.addAction(actImport);
    }else if (nt == NT_Friend){
        ctxMenu.addAction(actRemove);
    }
    else if (nt == NT_Groups){
        ctxMenu.addAction(actNewGroup);
    }else if (nt == NT_Group){
        ctxMenu.addAction(actRemoveGroup);
    }else{
        return ;
    }
    ctxMenu.exec(QCursor::pos());
}

void MainWindow::onActInvite()
{
    onInviteBtnClicked();
}
void MainWindow::onActImport()
{

}
void MainWindow::onActRemove()
{
    QTreeWidgetItem * elt = btree->currentItem();
    elt->parent()->removeChild(elt);
}
void MainWindow::onActNewGroup()
{

}
void MainWindow::onActRemoveGroup()
{
   QTreeWidgetItem * elt = btree->currentItem();
   elt->parent()->removeChild(elt);
}

void MainWindow::onActNetwork(bool fChecked)
{
    if (fChecked){
        startupNetwork();
    }else{
        shutdownNetwork();
    }
}
