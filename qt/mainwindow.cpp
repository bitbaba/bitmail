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

#include "main.h"

//! [1]
MainWindow::MainWindow(const QString & email, const QString & passphrase)
    : m_bitmail(new BitMail())
//! [1] //! [2]
{
    if (m_bitmail != NULL){
        BMQTApplication::LoadProfile(m_bitmail, email, passphrase);
    }

    m_pollth = new PollThread(m_bitmail);

    m_rxth = (new RxThread(m_bitmail));

    m_txth = (new TxThread(m_bitmail));

    connect(this, SIGNAL(readyToSend(QString,QString))
            , m_txth, SLOT(onSendMessage(QString,QString)));

    connect(m_pollth, SIGNAL(inboxPollEvent())
            , m_rxth, SLOT(onInboxPollEvent()));

    connect(m_rxth, SIGNAL(gotMessage(QString,QString,QString))
            , this, SLOT(onNewMessage(QString,QString,QString)));

    connect(m_pollth, SIGNAL(done()), this, SLOT(onPollDone()));

    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));

    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));

    m_pollth->start();

    m_rxth->start();

    m_txth->start();

    createActions();
    createToolBars();
    createStatusBar();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout * rightLayout = new QVBoxLayout;
    QHBoxLayout * btnLayout = new QHBoxLayout;
    blist = new QListWidget;
    blist->setIconSize(QSize(48,48));
    blist->setFixedWidth(192);
    leftLayout->addWidget(blist);
    mainLayout->addLayout(leftLayout);
    msgView = new QListWidget;
    msgView->setIconSize(QSize(48,48));
    msgView->setSpacing(2);
    textEdit = new QTextEdit;
    textEdit->setMinimumWidth(400);
    textEdit->setFixedHeight(96);
    btnSend = new QPushButton(tr("Send"));
    btnSend->setFixedWidth(64);
    btnSend->setFixedHeight(32);
    btnSend->setEnabled(false);
    btnLayout->addWidget(btnSend);
    btnLayout->setAlignment(btnSend, Qt::AlignLeft);
    rightLayout->addWidget(msgView);
    rightLayout->addWidget(chatToolbar);
    rightLayout->addWidget(textEdit);
    rightLayout->addLayout(btnLayout);
    mainLayout->addLayout(rightLayout);
    QWidget * wrap = new QWidget(this);
    wrap->setLayout(mainLayout);
    setCentralWidget(wrap);

    // Populate buddies list
    populateBuddies();

    // Add signals
    connect(btnSend, SIGNAL(clicked())
            , this, SLOT(onSendBtnClicked()));


    btnSend->setShortcut(QKeySequence("Ctrl+Return"));

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    connect(blist, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*))
            , this, SLOT(onCurrentBuddy(QListWidgetItem*,QListWidgetItem*)));

    connect(blist, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onBuddyDoubleClicked(QListWidgetItem*)));

#if defined(MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    setWindowIcon(QIcon(":/images/bitmail.png"));

    setWindowTitle(tr("BitMail"));

    textEdit->setFocus();
}
//! [2]

MainWindow::~MainWindow()
{

}

void MainWindow::onRxDone()
{
    if (!m_shutdownDialog)
        return ;

    m_shutdownDialog->SetMessage(tr("Rx thread done."));

    m_rxth->wait(1000);

    delete m_rxth; m_rxth = NULL;

    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}

void MainWindow::onTxDone()
{
    if (!m_shutdownDialog)
        return ;

    m_shutdownDialog->SetMessage(tr("Tx thread done."));

    m_txth->wait(1000);

    delete m_txth; m_txth = NULL;

    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}

void MainWindow::onPollDone()
{
    if (!m_shutdownDialog)
        return ;

    m_shutdownDialog->SetMessage(tr("Poll thread done."));

    m_pollth->wait(1000);

    delete m_pollth; m_pollth = NULL;

    if (!m_rxth && !m_txth && !m_pollth){
        m_shutdownDialog->done(0);
    }
}

void MainWindow::onBuddyDoubleClicked(QListWidgetItem *actItem)
{
    if (actItem == NULL){
        return ;
    }

    QString qsEmail = actItem->data(Qt::UserRole).toString();
    QString qsNick = QString::fromStdString(m_bitmail->GetCommonName(qsEmail.toStdString()));
    QString qsCertID = QString::fromStdString(m_bitmail->GetCertID(qsEmail.toStdString()));

    CertDialog certDialog(true, this);
    certDialog.setModal(false);
    certDialog.SetEmail(qsEmail);
    certDialog.SetNick(qsNick);
    certDialog.SetCertID(qsCertID);

    if (QDialog::Rejected == certDialog.exec()){
        return ;
    }

    (void)qsEmail;

    return ;
}

//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    m_rxth->stop(); m_txth->stop(); m_pollth->stop();

    m_shutdownDialog = new ShutdownDialog(this);
    m_shutdownDialog->exec();

    if (m_bitmail != NULL){
        qDebug() << "Dump profile";
        BMQTApplication::SaveProfile(m_bitmail);

        qDebug() << "Release bitmail, clear deleted messages.";
        delete m_bitmail;
        m_bitmail = NULL;
    }

    event->accept();    
}
//! [4]

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
        addAct = new QAction(QIcon(":/images/add.png"), tr("&Add"), this);
        addAct->setStatusTip(tr("Add a new buddy by certificate or certificate ID."));
        connect(addAct, SIGNAL(triggered()), this, SLOT(onAddBuddyBtnClicked()));
    }while(0);

    do {
        inviteAct = new QAction(QIcon(":/images/invite.png"), tr("&Invite"), this);
        inviteAct->setStatusTip(tr("Invite a new friend by send a request message."));
    }while(0);

    do{
        snapAct = new QAction(QIcon(":/images/snap.png"), tr("&Snapshot"), this);
        snapAct->setStatusTip(tr("Snapshot"));
    }while(0);

    do{
        fileAct = new QAction(QIcon(":/images/file.png"), tr("&File"), this);
        fileAct->setStatusTip(tr("File"));
    }while(0);

    do{
        emojAct = new QAction(QIcon(":/images/emoj.png"), tr("&Emoji"), this);
        emojAct->setStatusTip(tr("Emoji"));
    }while(0);

    do{
        soundAct = new QAction(QIcon(":/images/sound.png"), tr("&Sound"), this);
        soundAct->setStatusTip(tr("Sound"));
    }while(0);

    do{
        videoAct = new QAction(QIcon(":/images/video.png"), tr("&Video"), this);
        videoAct->setStatusTip(tr("Video"));
    }while(0);

    do{
        liveAct = new QAction(QIcon(":/images/live.png"), tr("&Live"), this);
        liveAct->setStatusTip(tr("Live"));
    }while(0);

    do{
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
    editToolBar->addAction(addAct);
    editToolBar->addAction(inviteAct);

    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->addAction(emojAct);
    chatToolbar->addAction(snapAct);
    chatToolbar->addAction(fileAct);
    chatToolbar->addAction(soundAct);
    chatToolbar->addAction(videoAct);
    chatToolbar->addAction(liveAct);
    chatToolbar->addAction(payAct);
    chatToolbar->setIconSize(QSize(24,24));

    walletToolbar = addToolBar(tr("Wallet"));
    walletToolbar->addAction(walletAct);
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
        qDebug() << "no message to send";
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
    (void)sMsg;

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    (void)qsFrom;

    if (blist->currentItem() == NULL){
        qDebug() << "no current buddy selected";
        statusBar()->showMessage(tr("no current buddy selected"), 3000);
        return ;
    }

    QString qsTo = blist->currentItem()->data(Qt::UserRole).toString();

    emit readyToSend(qsTo, QString::fromStdString(sMsg));

    populateMessage(true
                    , QString::fromStdString(m_bitmail->GetEmail())
                    , QString::fromStdString(sMsg));


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
    optDialog.SetNick(QString::fromStdString(m_bitmail->GetCommonName(m_bitmail->GetEmail())));
    optDialog.SetPassphrase(QString::fromStdString(m_bitmail->GetPassphrase()));
    optDialog.SetBits(m_bitmail->GetBits());

    optDialog.SetSmtpUrl(QString::fromStdString(m_bitmail->GetTxUrl()));
    optDialog.SetSmtpLogin(QString::fromStdString(m_bitmail->GetTxLogin()));
    optDialog.SetSmtpPassword(QString::fromStdString(m_bitmail->GetTxPassword()));

    optDialog.SetImapUrl(QString::fromStdString(m_bitmail->GetRxUrl()));
    optDialog.SetImapLogin(QString::fromStdString(m_bitmail->GetRxLogin()));
    optDialog.SetImapPassword(QString::fromStdString(m_bitmail->GetRxPassword()));

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

void MainWindow::populateMessage(bool fTx, const QString &from, const QString &msg)
{
    QListWidgetItem * msgElt = new QListWidgetItem(QIcon(":/images/bubble.png")
                                                   , msg);

    msgElt->setFlags((Qt::ItemIsSelectable
                      | Qt::ItemIsEditable)
                      | Qt::ItemIsEnabled);

    msgElt->setBackgroundColor(fTx ? Qt::lightGray
                                   : QColor(Qt::green).lighter());

    msgElt->setData(Qt::UserRole, QVariant(from));

    msgView->addItem(msgElt);

    msgView->scrollToBottom();

    return;
}

void MainWindow::onNewMessage(const QString &from, const QString &msg, const QString &cert)
{
    //TODO: cert check and buddy management
    (void) cert;

    //show message
    populateMessage(false, from, msg);
}

void MainWindow::populateBuddies()
{
    // Add buddies
    std::vector<std::string> vecEmails;
    m_bitmail->GetBuddies(vecEmails);

    for (std::vector<std::string>::const_iterator it = vecEmails.begin()
         ; it != vecEmails.end(); ++it)
    {
        std::string sBuddyNick = m_bitmail->GetCommonName(*it);
        QString qsNick = QString::fromStdString(sBuddyNick);
        populateBuddy(QString::fromStdString(*it), qsNick);
    }
    return ;
}

void MainWindow::populateBuddy(const QString &email, const QString &nick)
{
    QListWidgetItem *buddy = new QListWidgetItem(QIcon(":/images/head.png"), nick);
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

void MainWindow::onAddBuddyBtnClicked()
{
    CertDialog certDialog(false, this);
    if (QDialog::Rejected == certDialog.exec()){
        return ;
    }
    
    QString qsEmail = certDialog.GetEmail();
    QString qsNick = certDialog.GetNick();
    QString qsCertID = certDialog.GetCertID();

    (void)qsEmail, (void)qsNick, (void)qsCertID;

    if (qsEmail.isEmpty()){
        statusBar()->showMessage(tr("Invalid email address"), 5000);
        return ;
    }

    if (qsCertID.isEmpty()){
        statusBar()->showMessage(tr("Invalid cert identifier"), 5000);
        return ;
    }

    m_bitmail->AddBuddy(qsCertID.toStdString());

    populateBuddy(qsEmail, qsNick);
}

