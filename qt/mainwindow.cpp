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
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QUrl>
#include <QtMultimedia/QAudioRecorder>

#include <bitmailcore/bitmail.h>
#include "mainwindow.h"
//! [0]
#include "rxthread.h"
#include "txthread.h"
#include "optiondialog.h"
#include "logindialog.h"
#include "netoptdialog.h"
#include "shutdowndialog.h"
#include "certdialog.h"
#include "invitedialog.h"
#include "messagedialog.h"
#include "ardialog.h"
#include "main.h"

#define TAG_PEER ("#")
#define KEY_STRANGER ("?")

MainWindow::MainWindow(BitMail * bitmail)
    : m_bitmail(bitmail)
    , m_rxth(NULL)
    , m_txth(NULL)
    , m_shutdownDialog(NULL)
    , m_arDlg(NULL)
{
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

    /** audio recorder **/
    audioRecorder = new QAudioRecorder();
    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));

    /** Form */
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *rightLayout = new QVBoxLayout;
    QHBoxLayout *btnLayout = new QHBoxLayout;

    btree = new QTreeWidget;
    btree->setIconSize(QSize(48,48));
    btree->setFixedWidth(280);
    btree->setColumnCount(1);
    btree->setFont(QFont("Arial", 8));
    QStringList columns;
    columns.append(tr("Contact"));
    btree->setHeaderLabels(columns);
    btree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCtxMenu(QPoint)));

    if (1){
        nodeFriends = new QTreeWidgetItem(btree, QStringList(tr("Friends")), NT_Friends);
        nodeFriends->setIcon(0, QIcon(":/images/head.png"));
        nodeFriends->setData(0, Qt::UserRole, "Cat::Friends");
        populateFriendTree(nodeFriends);
        btree->addTopLevelItem(nodeFriends);
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
    QFont font = textEdit->font();
    font.setBold(true);
    textEdit->setFont(font);
    textEdit->setFocus();

    btnLayout->setAlignment(Qt::AlignLeft);

    btnSend = new QPushButton(tr("Send"));
    btnSend->setToolTip(tr("Ctrl+Enter"));
    //btnSend->setFixedWidth(64);
    btnSend->setFixedHeight(32);
    btnSend->setEnabled(false);
    btnSend->setShortcut(QKeySequence("Ctrl+Return"));
    btnLayout->addWidget(btnSend);

    btnSendQr = new QPushButton(tr("QrImage"));
    btnSendQr->setToolTip(tr("Ctrl+Shift+Enter"));
    //btnSendQr->setFixedWidth(64);
    btnSendQr->setFixedHeight(32);
    btnSendQr->setEnabled(false);
    btnSendQr->setShortcut(QKeySequence("Ctrl+Shift+Return"));
    btnLayout->addWidget(btnSendQr);

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

    // Bind signals
    connect(btree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onTreeCurrentBuddy(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(btree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onTreeBuddyDoubleClicked(QTreeWidgetItem*,int)));
    connect(msgView, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onMessageDoubleClicked(QListWidgetItem*)));
    connect(btnSend, SIGNAL(clicked()), this, SLOT(onBtnSendClicked()));
    connect(btnSendQr, SIGNAL(clicked()), this, SLOT(onBtnSendQrClicked()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::configNetwork()
{
    onNetAct();
}

void MainWindow::startupNetwork()
{
    m_rxth = new RxThread(m_bitmail);
    connect(m_rxth, SIGNAL(gotMessage(QString, QString, QString,QString)), this, SLOT(onNewMessage(QString, QString, QString,QString)));
    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));
    connect(m_rxth, SIGNAL(rxProgress(QString)), this, SLOT(onRxProgress(QString)));
    m_rxth->start();

    m_txth = new TxThread(m_bitmail);
    connect(this, SIGNAL(readyToSend(QString,QString,QString)), m_txth, SLOT(onSendMessage(QString,QString,QString)));
    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));
    connect(m_txth, SIGNAL(txProgress(QString)), this, SLOT(onTxProgress(QString)));
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    shutdownNetwork();
    /**
    * Model here
    */
    if (m_rxth || m_txth){
        m_shutdownDialog = new ShutdownDialog(this);
        m_shutdownDialog->exec();
    }
    event->accept();
}

/** Double click a friend node*/
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
    QString qsEmail = qslData.at(1);

    if (qsEmail.isEmpty() || qsEmail == KEY_STRANGER){
        return ;
    }

    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(qsEmail.toStdString()));
    QString qsCertID = QString::fromStdString(m_bitmail->GetFriendID(qsEmail.toStdString()));
    CertDialog certDialog(m_bitmail, this);    
    certDialog.SetEmail(qsEmail);
    certDialog.SetNick(qsNick);
    certDialog.SetCertID(qsCertID);

    QPixmap qrImage = BMQTApplication::toQrImage(QString("bitmail:%1#%2").arg(qsEmail).arg(qsCertID));
    certDialog.qrImage(qrImage);

    if (QDialog::Rejected == certDialog.exec()){
        return ;
    }
}

void MainWindow::createActions()
{
    do {
        configAct = new QAction(QIcon(":/images/config.png"), tr("&Configure"), this);
        configAct->setStatusTip(tr("Configure current account"));
        connect(configAct, SIGNAL(triggered()), this, SLOT(onBtnConfigClicked()));
    }while(0);

    do{
        netAct = new QAction(QIcon(":/images/network.png"), tr("&Network"), this);
        netAct->setStatusTip(tr("Network"));
        connect(netAct, SIGNAL(triggered()), this, SLOT(onNetAct()));
    }while(0);

    do {
        inviteAct = new QAction(QIcon(":/images/invite.png"), tr("&Invite"), this);
        inviteAct->setStatusTip(tr("Invite by sending your signature"));
        connect(inviteAct, SIGNAL(triggered()), this, SLOT(onBtnInviteClicked()));
    }while(0);

    do{        
        removeAct = new QAction(QIcon(":/images/remove.png"), tr("&Remove"), this);
        removeAct->setStatusTip(tr("Remove"));
        connect(removeAct, SIGNAL(triggered()), this, SLOT(onRemoveAct()));
    }while(0);

    do{
        fileAct = new QAction(QIcon(":/images/file.png"), tr("&File"), this);
        fileAct->setStatusTip(tr("Send File"));
        connect(fileAct, SIGNAL(triggered()), this, SLOT(onFileAct()));
    }while(0);

    do{
        audioAct = new QAction(QIcon(":/images/sound.png"), tr("&Audio"), this);
        audioAct->setStatusTip(tr("Send audio"));
        connect(audioAct, SIGNAL(triggered()), this, SLOT(onAudioAct()));
    }while(0);

    do{
        emojiAct = new QAction(QIcon(":/images/emoj.png"), tr("&Emoji"), this);
        emojiAct->setStatusTip(tr("Send emoji"));
        connect(emojiAct, SIGNAL(triggered()), this, SLOT(onEmojiAct()));
    }while(0);

    do{
        snapAct = new QAction(QIcon(":/images/snap.png"), tr("&Snapshot"), this);
        snapAct->setStatusTip(tr("Send snapshot"));
        connect(snapAct, SIGNAL(triggered()), this, SLOT(onSnapAct()));
    }while(0);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(configAct);
    fileToolBar->addAction(netAct);

    editToolBar = addToolBar(tr("Buddies"));
    editToolBar->addAction(inviteAct);

    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->setIconSize(QSize(24,24));
    chatToolbar->addAction(emojiAct);
    chatToolbar->addAction(audioAct);
    chatToolbar->addAction(snapAct);
    chatToolbar->addAction(fileAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


void MainWindow::onBtnConfigClicked()
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

void MainWindow::onNetAct()
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
        m_bitmail->SetBradPort(BMQTApplication::GetLanIp().toStdString(), optDialog.BradPort());
    }while(0);

    return ;
}

void MainWindow::onRemoveAct()
{
    QTreeWidgetItem * elt = btree->currentItem();
    elt->parent()->removeChild(elt);
}

void MainWindow::onFileAct()
{
    if (m_txth == NULL){
        statusBar()->showMessage(tr("No active network"));
        return ;
    }

    QStringList paths = QFileDialog::getOpenFileNames(this);
    if (!paths.size()){
        return ;
    }

    QVariantList vparts;
    for (QStringList::const_iterator it = paths.constBegin(); it != paths.constEnd(); ++it){
        vparts.append(QVariant::fromValue(QFileInfo(*it)));
    }

    QString qsMsg = BMQTApplication::toMixed(vparts);

    QString qsTo = getCurrentReceipt();
    if (qsTo.isEmpty()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());

    emit readyToSend(qsFrom, qsTo, qsMsg);

    enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);

    populateMessages(qsTo);
}

void MainWindow::onAudioAct()
{
    if (m_txth == NULL){
        statusBar()->showMessage(tr("No active network"));
        return ;
    }
    QString tempAudio = BMQTApplication::GetTempHome() + "/" +QUuid::createUuid().toString().remove("{").remove("}");
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/amr");
    audioSettings.setQuality(QMultimedia::VeryLowQuality);
    audioRecorder->setEncodingSettings(audioSettings);
    audioRecorder->setOutputLocation(QUrl::fromLocalFile(tempAudio));
    audioRecorder->record();
    if (!m_arDlg){
        m_arDlg = new arDialog(this);
    }
    if (m_arDlg){
        m_arDlg->exec();
    }
}

void MainWindow::onDurationChanged(qint64 duration)
{
    if (duration >= 3000){
        if (m_arDlg){
            m_arDlg->done(0);
        }
        audioRecorder->stop();
        QString audioFilePath = audioRecorder->outputLocation().toLocalFile();

        QString qsMsg = BMQTApplication::toMimeAttachment(audioFilePath);
        QString qsTo = getCurrentReceipt();
        if (qsTo.isEmpty()){
            return ;
        }
        QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
        QString qsCertId = QString::fromStdString(m_bitmail->GetID());
        QString qsCert = QString::fromStdString(m_bitmail->GetCert());
        emit readyToSend(qsFrom, qsTo, qsMsg);
        enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);
        populateMessages(qsTo);
    }else{
        m_arDlg->ShowStatus(tr("Recorded") + QString("%1.%2").arg(duration / 1000).arg(duration % 1000) + tr("seconds"));
    }
}

void MainWindow::onEmojiAct()
{
    QString emojiFile = QFileDialog::getOpenFileName(this, tr("select a emoji to send"), BMQTApplication::GetEmojiHome(), "Emoji Files (*.png *.jpg *.bmp)");
    QString qsMsg = BMQTApplication::toMimeAttachment(emojiFile);
    QString qsTo = getCurrentReceipt();
    if (qsTo.isEmpty()){
        return ;
    }
    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());
    emit readyToSend(qsFrom, qsTo, qsMsg);
    enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);
    populateMessages(qsTo);
    return ;
}

void MainWindow::onSnapAct()
{
    // minimize parent
    this->hide();

    QTimer::singleShot(1000, this, &MainWindow::shootScreen);
}

void MainWindow::shootScreen()
{
    QApplication::beep();

    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

    // grab
    QPixmap originalPixmap = screen->grabWindow(0);
    // TODO: model a edit window to select in a pixmap

    // show window
    this->showNormal();

    QString qsMsg = BMQTApplication::toMimeImage(originalPixmap.toImage());
    QString qsTo = getCurrentReceipt();
    if (qsTo.isEmpty()){
        return ;
    }
    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());
    emit readyToSend(qsFrom, qsTo, qsMsg);
    enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);
    populateMessages(qsTo);
    return ;
}

void MainWindow::onBtnInviteClicked()
{
    InviteDialog inviteDialog(this);
    if (QDialog::Accepted != inviteDialog.exec()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsTo = inviteDialog.GetEmail();
    QString qsWhisper = BMQTApplication::toMimeTextPlain( inviteDialog.GetWhisper() );

    emit readyToSend(qsFrom , qsTo, qsWhisper);

    return ;
}

void MainWindow::onBtnSendClicked()
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
    QString qsMsgPlain = textEdit->toPlainText();
    textEdit->clear();
    if (qsMsgPlain.isEmpty()){
        return ;
    }

#if 1
    QString qsMsg = BMQTApplication::toMimeTextPlain(qsMsgPlain);
#endif

#if 0
    QString qsMsg = BMQTApplication::toMimeTextHtml(qsMsgPlain);
#endif

#if 0
    QStringList parts;
    parts.append( BMQTApplication::toMimeTextPlain(qsMsgPlain) );
    parts.append( BMQTApplication::toMimeImage(QImage(":/images/head.png")) );
    QString qsMsg = BMQTApplication::toMixed(parts);
#endif

#if 0
    QVariantList vparts;
    vparts.append(QImage(":/images/head.png"));
    vparts.append("Hello! BitMail.");
    vparts.append(QVariant::fromValue(QFileInfo("D:/Users/harry/bitmail/data/c12f0b637784d65e424463c29d0cc7b50d7860f7ada6968db0f405bee663ba0a.PNG")));
    vparts.append(QVariant::fromValue(QFileInfo("D:/Users/harry/bitmail/data/1.amr")));
    QString qsMsg = BMQTApplication::toMixed(vparts);
#endif

#if 0
    QStringList parts;
    parts.append( BMQTApplication::toMimeTextHtml(qsMsgPlain) );
    parts.append( BMQTApplication::toMimeImage(QImage(":/images/head.png")) );
    QString qsMsg = BMQTApplication::toMixed(parts);
#endif


    QString qsTo = getCurrentReceipt();
    if (qsTo.isEmpty()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());

    emit readyToSend(qsFrom, qsTo, qsMsg);

    enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);

    populateMessages(qsTo);

    // There are bugs in Qt-Creator's memory view utlity;
    // open <address> in memory window,
    // select the value of the <address>,
    // jump to <value>(little-endien) in a NEW memory window, note: NEW
}

void MainWindow::onBtnSendQrClicked()
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
    QString qsMsgPlain = textEdit->toPlainText();
    textEdit->clear();
    if (qsMsgPlain.isEmpty()){
        return ;
    }

    QString qsMsg = BMQTApplication::toMimeImage(BMQTApplication::toQrImage(qsMsgPlain).toImage());

    QString qsTo = getCurrentReceipt();
    if (qsTo.isEmpty()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());

    emit readyToSend(qsFrom, qsTo, qsMsg);

    enqueueMsg(qsTo, true, qsFrom, qsTo, qsMsg, qsCertId, qsCert);

    populateMessages(qsTo);

    // There are bugs in Qt-Creator's memory view utlity;
    // open <address> in memory window,
    // select the value of the <address>,
    // jump to <value>(little-endien) in a NEW memory window, note: NEW
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
    QString qsTo = QString::fromStdString(m_bitmail->GetEmail());

    enqueueMsg(from, false, from, qsTo, content, certid, cert);

    populateMessages(from);
}

void MainWindow::enqueueMsg(const QString & k
                            , bool tx
                            , const QString & from
                            , const QString & to
                            , const QString & msg
                            , const QString & certid
                            , const QString & cert)
{
    QJsonObject obj;
    obj["tx"] = tx;
    obj["from"] = from;
    obj["to"] = to;
    obj["msg"] = msg;
    obj["certid"] = certid;
    obj["cert"] = cert;
    QJsonDocument doc;
    doc.setObject(obj);
    m_peermsgQ[k].append(doc.toJson());
}

QStringList MainWindow::dequeueMsg(const QString &k)
{
    return m_peermsgQ[k];
}

void MainWindow::onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    btnSend->setEnabled(false);
    btnSendQr->setEnabled(false);
    sessLabel->setText("");

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
    if (QString::fromStdString(qvData.typeName()) != "QStringList" ){
        return ;
    }

    QStringList qslData = qvData.toStringList();

    QString qsKey = qslData.at(1);

    /*Setup session label header*/
    if (qsKey == KEY_STRANGER){
        sessLabel->setText(tr("Strangers"));
    }else{
        sessLabel->setText(QString::fromStdString(m_bitmail->GetFriendNick(qsKey.toStdString())));
        btnSend->setEnabled(true);
        btnSendQr->setEnabled(true);
    }

    populateMessages(qsKey);

    return ;
}

void MainWindow::populateMessages(const QString & k)
{
    msgView->clear();

    QStringList rtxList = dequeueMsg(k);
    for (int i = 0; i < rtxList.length(); ++i){
        QString tmp = rtxList.at(i);
        QJsonObject msgObj = (QJsonDocument::fromJson(tmp.toUtf8()).object());
        QListWidgetItem * msgElt = new QListWidgetItem();
        msgElt->setIcon(QIcon(":/images/bubble.png"));
        msgElt->setText(msgObj["msg"].toString().mid(0, 128));
        msgElt->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        msgElt->setBackgroundColor(msgObj["tx"].toBool() ? Qt::lightGray : Qt::green);
        msgElt->setData(Qt::UserRole, msgObj);
        msgView->addItem(msgElt);
        msgView->scrollToBottom();
    }
}

/** Double click message element in message view*/
void MainWindow::onMessageDoubleClicked(QListWidgetItem * actItem)
{
    if (!actItem){
        return ;
    }

    QVariant qvData = actItem->data(Qt::UserRole);
    QString qvDataType = qvData.typeName();
    if (qvDataType != "QJsonObject"){
        return ;
    }

    QJsonObject msgObj = qvData.toJsonObject();

    MessageDialog messageDialog(m_bitmail);
    messageDialog.SetFrom(msgObj["from"].toString());
    messageDialog.SetMessage(msgObj["msg"].toString());
    messageDialog.SetCertID(msgObj["certid"].toString());
    messageDialog.SetCert(msgObj["cert"].toString());

    connect(&messageDialog, SIGNAL(signalAddFriend(QString))
            , this, SLOT(onAddFriend(QString)));

    messageDialog.exec();

    return ;
}

QString MainWindow::getCurrentReceipt()
{
    if (btree->currentItem() == NULL){
        return "";
    }

    QVariant qvData = btree->currentItem()->data(0, Qt::UserRole);
    if (!qvData.isValid() || qvData.isNull()){
        return "";
    }

    QString qsVDataTyep = qvData.typeName();
    if (qsVDataTyep != "QStringList"){
        return "";
    }

    QStringList qslData = qvData.toStringList();

    return qslData.at(1);
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
    qslData.append(TAG_PEER);
    qslData.append(email);
    buddy->setData(0, Qt::UserRole, QVariant(qslData));
    node->addChild(buddy);
    return ;
}

void MainWindow::onAddFriend(const QString &email)
{
    QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(email.toStdString()));
    populateFriendLeaf(nodeFriends, email, qsNick, NT_Friend);
}

