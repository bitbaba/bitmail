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
#include <QFileIconProvider>
#include <QSystemTrayIcon>
#include <QProgressBar>

#include <iostream>
#include <bitmailcore/bitmail.h>
#include "mainwindow.h"
//! [0]
#include "rxthread.h"
#include "txthread.h"
#include "procth.h"
#include "optiondialog.h"
#include "logindialog.h"
#include "netoptdialog.h"
#include "shutdowndialog.h"
#include "certdialog.h"
#include "invitedialog.h"
#include "messagedialog.h"
#include "newgroupdialog.h"
#include "main.h"

MainWindow::MainWindow(BitMail * bitmail)
    : m_bitmail(bitmail)
    , m_rxth(NULL)
    , m_txth(NULL)
    , m_shutdownDialog(NULL)
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
    setupTextActions();

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

    if (1){
        nodeFriends = new QTreeWidgetItem(btree, QStringList(tr("Friends")));
        nodeFriends->setIcon(0, QIcon(":/images/head.png"));
        btree->addTopLevelItem(nodeFriends);
        populateFriendTree();
        nodeFriends->setExpanded(true);
    }

    if (2){
        nodeGroups = new QTreeWidgetItem(btree, QStringList(tr("Groups")));
        nodeGroups->setIcon(0, QIcon(":/images/group.png"));
        btree->addTopLevelItem(nodeGroups);
        populateGroupTree();
    }

    leftLayout->addWidget(btree);

    mainLayout->addLayout(leftLayout);

    sessLabel = new QLabel;
    sessLabel->setMaximumHeight(32);
    sessLabel->setMinimumHeight(32);
    sessLabel->setAlignment(Qt::AlignCenter);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    sessLabel->setPalette(pa);
    sessLabel->setText("");

    msgView = new QListWidget;
    msgView->setIconSize(QSize(48,48));
    msgView->setSpacing(2);
    msgView->setFont(QFont("Arial", 8));
    msgView->setSpacing(10);

    textEdit = new QTextEdit;
    textEdit->setMinimumWidth(560);
    textEdit->setFixedHeight(100);
    textEdit->setAcceptRichText(false);
    textEdit->setFocus();
    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));

    btnLayout->setAlignment(Qt::AlignLeft);

    btnSend = new QPushButton(tr("Send"));
    btnSend->setToolTip(tr("Ctrl+Enter"));
    btnSend->setFixedHeight(32);
    btnSend->setEnabled(false);
    btnSend->setShortcut(QKeySequence("Ctrl+Return"));
    btnLayout->addWidget(btnSend);

    btnSendQr = new QPushButton(tr("QrImage"));
    btnSendQr->setToolTip(tr("Ctrl+Shift+Enter"));
    btnSendQr->setFixedHeight(32);
    btnSendQr->setEnabled(false);
    btnSendQr->setShortcut(QKeySequence("Ctrl+Shift+Return"));
    btnLayout->addWidget(btnSendQr);

    rightLayout->addWidget(sessLabel);
    rightLayout->addWidget(msgView);
    rightLayout->addWidget(chatToolbar);
    rightLayout->addWidget(textToolbar);
    rightLayout->addWidget(textEdit);
    rightLayout->addLayout(btnLayout);
    mainLayout->addLayout(rightLayout);
    QWidget * wrap = new QWidget(this);
    wrap->setMinimumWidth(720);
    wrap->setMinimumHeight(600);
    wrap->setLayout(mainLayout);
    setCentralWidget(wrap);

    do {
        rxProg = new QProgressBar(this);
        rxProg->setTextVisible(false);
        rxProg->setMaximumHeight(statusBar()->height()/2);
        rxProg->setMinimumWidth(rxProg->maximumHeight()/2);
        rxProg->setMaximumWidth(rxProg->maximumHeight()/2);
        rxProg->setInvertedAppearance(true);
        rxProg->setOrientation(Qt::Vertical);
        //rxProg->setRange(0, 0);
        statusBar()->addPermanentWidget(rxProg);

        rxTip = new QLabel(this);
        rxTip->setMaximumWidth(64);
        rxTip->setMinimumWidth(64);
        statusBar()->addPermanentWidget(rxTip);
    }while(0);


    do {
        txProg = new QProgressBar(this);
        txProg->setTextVisible(false);
        txProg->setMaximumHeight(statusBar()->height()/2);
        txProg->setMinimumWidth(txProg->maximumHeight()/2);
        txProg->setMaximumWidth(txProg->maximumHeight()/2);
        txProg->setInvertedAppearance(false);
        txProg->setOrientation(Qt::Vertical);
        statusBar()->addPermanentWidget(txProg);

        txTip = new QLabel(this);
        txTip->setMaximumWidth(64);
        txTip->setMinimumWidth(64);
        statusBar()->addPermanentWidget(txTip);
    }while(0);

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
    connect(m_rxth, SIGNAL(gotMessage(QString, QString, QString, QString, QString, QString)), this, SLOT(onNewMessage(QString, QString, QString, QString, QString, QString)));
    connect(m_rxth, SIGNAL(done()), this, SLOT(onRxDone()));
    connect(m_rxth, SIGNAL(rxProgress(int, QString)), this, SLOT(onRxProgress(int, QString)));
    m_rxth->start();

    m_txth = new TxThread(m_bitmail);
    connect(this, SIGNAL(readyToSend(QString,QStringList,QString)), m_txth, SLOT(onSendMessage(QString,QStringList,QString)));
    connect(m_txth, SIGNAL(done()), this, SLOT(onTxDone()));
    connect(m_txth, SIGNAL(txProgress(int, QString)), this, SLOT(onTxProgress(int, QString)));
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
void MainWindow::onRxProgress(int st, const QString &info)
{
    qDebug() << info;
    if (st == Rx_download){
        rxProg->setValue(info.toFloat());
    }
    if (st == Rx_doneload){
        rxProg->setValue(0);
    }
    rxTip->setText(info);
    return ;
}
void MainWindow::onTxProgress(int st, const QString &info)
{
    qDebug() << info;
    if (st == Tx_upload){
        txProg->setValue(info.toFloat());
    }
    if (st == Tx_updone){
        txProg->setValue(0);
    }
    txTip->setText(info);
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
    if (actItem == NULL || col != 0){
        return ;
    }

    QVariant qvData = actItem->data(0, Qt::UserRole);
    if (!qvData.isValid()
        || qvData.isNull()
        || QString::fromStdString(qvData.typeName()) != "QString")
    {
        return ;
    }

    QString sessKey = qvData.toString();

    QStringList receips = BMQTApplication::toQStringList(BitMail::fromSessionKey(sessKey.toStdString()));

    if (receips.size() == 1){
        QString qsEmail = receips.at(0);
        QString qsNick = QString::fromStdString(m_bitmail->GetFriendNick(qsEmail.toStdString()));
        QString qsCertID = QString::fromStdString(m_bitmail->GetFriendID(qsEmail.toStdString()));
        CertDialog certDialog(m_bitmail, this);
        certDialog.SetEmail(qsEmail);
        certDialog.SetNick(qsNick);
        certDialog.SetCertID(qsCertID);
        std::string sessKey = BitMail::toSessionKey(qsEmail.toStdString());
        certDialog.comment(QString::fromStdString(m_bitmail->sessionName(sessKey)));

        QPixmap qrImage = BMQTApplication::toQrImage(QString("bitmail:%1#%2").arg(qsEmail).arg(qsCertID));
        certDialog.qrImage(qrImage);

        connect(&certDialog, SIGNAL(friendChanged()), this, SLOT(populateFriendTree()));

        if (QDialog::Rejected == certDialog.exec()){
            return ;
        }
    }else if (receips.size() >= 2){
        NewGroupDialog dlg(m_bitmail);
        dlg.groupMembers(receips);
        connect(&dlg, SIGNAL(groupChanged()), this, SLOT(populateGroupTree()));
        if (dlg.exec() != QDialog::Accepted){
            return ;
        }
        std::vector<std::string> _receips = BMQTApplication::toStdStringList(dlg.groupMembers());
        std::string _sessKey = BitMail::toSessionKey(_receips);
        if (_sessKey != sessKey.toStdString()){
            m_bitmail->AddGroup(BitMail::serializeReceips(_receips));
            populateGroupTree();
        }
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

    do{
        photoAct = new QAction(QIcon(":/images/camera.png"), tr("&Photo"), this);
        photoAct->setStatusTip(tr("Send photo"));
        connect(photoAct, SIGNAL(triggered()), this, SLOT(onPhotoAct()));
    }while(0);

    do{
        videoAct = new QAction(QIcon(":/images/video.png"), tr("&Video"), this);
        videoAct->setStatusTip(tr("Send video"));
        connect(videoAct, SIGNAL(triggered()), this, SLOT(onVideoAct()));
    }while(0);

    do{
        htmlAct = new QAction(QIcon(":/images/html.png"), tr("&Html"), this);
        htmlAct->setStatusTip(tr("HTML mode"));
        htmlAct->setCheckable(true);
        connect(htmlAct, SIGNAL(triggered(bool)), this, SLOT(onHtmlAct(bool)));
    }while(0);
}


void MainWindow::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    textToolbar = tb;
    textToolbar->setVisible(false);

    QString rsrcPath = BMQTApplication::GetResHome();
    rsrcPath += "/textedit/win";

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = tb->addAction(boldIcon, tr("&Bold"), this, &MainWindow::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = tb->addAction(italicIcon, tr("&Italic"), this, &MainWindow::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = tb->addAction(underlineIcon, tr("&Underline"), this, &MainWindow::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = tb->addAction(pix, tr("&Color..."), this, &MainWindow::textColor);
    tb->addAction(actionTextColor);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");

    typedef void (QComboBox::*QComboIntSignal)(int);
    connect(comboStyle, static_cast<QComboIntSignal>(&QComboBox::activated), this, &MainWindow::textStyle);

    typedef void (QComboBox::*QComboStringSignal)(const QString &);
    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, static_cast<QComboStringSignal>(&QComboBox::activated), this, &MainWindow::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes) comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, static_cast<QComboStringSignal>(&QComboBox::activated), this, &MainWindow::textSize);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void MainWindow::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void MainWindow::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void MainWindow::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void MainWindow::createToolBars()
{
    editToolBar = addToolBar(tr("Buddies"));
    editToolBar->addAction(inviteAct);

    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(configAct);
    fileToolBar->addAction(netAct);

    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->setIconSize(QSize(24,24));
    chatToolbar->addAction(emojiAct);
    chatToolbar->addAction(snapAct);
    chatToolbar->addAction(fileAct);
    chatToolbar->addAction(audioAct);
    chatToolbar->addAction(photoAct);
    chatToolbar->addAction(videoAct);
    chatToolbar->addAction(htmlAct);
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
    optDialog.SetSmtpUrl(QString::fromStdString(m_bitmail->txUrl()));
    optDialog.SetSmtpLogin(QString::fromStdString(m_bitmail->txLogin()));
    optDialog.SetSmtpPassword(QString::fromStdString(m_bitmail->txPassword()));

    optDialog.SetImapUrl(QString::fromStdString(m_bitmail->rxUrl()));
    optDialog.SetImapLogin(QString::fromStdString(m_bitmail->rxLogin()));
    optDialog.SetImapPassword(QString::fromStdString(m_bitmail->rxPassword()));

    optDialog.socks5(QString::fromStdString(m_bitmail->proxy()));

    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }

    QString qsTxUrl      = optDialog.GetSmtpUrl();
    QString qsTxLogin    = optDialog.GetSmtpLogin();
    QString qsTxPassword = optDialog.GetSmtpPassword();

    QString qsRxUrl      = optDialog.GetImapUrl();
    QString qsRxLogin    = optDialog.GetImapLogin();
    QString qsRxPassword = optDialog.GetImapPassword();

    QString qsProxy      = optDialog.socks5();

    m_bitmail->InitNetwork(qsTxUrl.toStdString()
                        , qsTxLogin.toStdString()
                        , qsTxPassword.toStdString()
                        , qsRxUrl.toStdString()
                        , qsRxLogin.toStdString()
                        , qsRxPassword.toStdString()
                        , qsProxy.toStdString());

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
        txTip->setText(tr("No active network"));
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

    Send(qsMsg);
}

void MainWindow::onEmojiAct()
{
    QString emojiFile = QFileDialog::getOpenFileName(this, tr("select a emoji to send"), BMQTApplication::GetEmojiHome(), "Emoji Files (*.png *.jpg *.bmp)");
    QString qsMsg = BMQTApplication::toMimeAttachment(emojiFile);
    Send(qsMsg);
}

void MainWindow::onAudioAct()
{
    QString sessKey = currentSessionKey();

    QString app = BMQTApplication::GetAppHome() + "/audiorecorder" + BMQTApplication::GetExeSuffix();

    QString temp = BMQTApplication::GetTempFile();
    temp += ".wav";

    ProcTh * proc = new ProcTh(sessKey, app, QStringList(), temp);

    connect(proc, SIGNAL(procDone(QString, QString)), this, SLOT(onProcThDone(QString, QString)));

    proc->start();
}

void MainWindow::onSnapAct()
{
    QString sessKey = currentSessionKey();

    QString app = BMQTApplication::GetAppHome() + "/screenshot" + BMQTApplication::GetExeSuffix();

    QString temp = BMQTApplication::GetTempFile();
    temp += ".png";

    ProcTh * proc = new ProcTh(sessKey, app, QStringList(), temp);

    connect(proc, SIGNAL(procDone(QString, QString)), this, SLOT(onProcThDone(QString, QString)));

    proc->start();
}

void MainWindow::onPhotoAct()
{
    QString sessKey = currentSessionKey();

    QString app = BMQTApplication::GetAppHome() + "/camera" + BMQTApplication::GetExeSuffix();

    QString temp = BMQTApplication::GetTempFile();
    temp += ".jpg";

    ProcTh * proc = new ProcTh(sessKey, app, QStringList()<<"-j", temp);

    connect(proc, SIGNAL(procDone(QString, QString)), this, SLOT(onProcThDone(QString, QString)));

    proc->start();
}

void MainWindow::onVideoAct()
{
    QString sessKey = currentSessionKey();

    QString app = BMQTApplication::GetAppHome() + "/camera" + BMQTApplication::GetExeSuffix();

    QString temp = BMQTApplication::GetTempFile();
    temp += ".mpg";

    ProcTh * proc = new ProcTh(sessKey, app, QStringList()<<"-m", temp);

    connect(proc, SIGNAL(procDone(QString, QString)), this, SLOT(onProcThDone(QString, QString)));

    proc->start();
}

void MainWindow::onHtmlAct(bool fChecked)
{
    textToolbar->setVisible(fChecked);
    if (fChecked){
        if (!textEdit->acceptRichText()) textEdit->setAcceptRichText(true);
        if (0){
            /**
             * 1) emoji could be implemented by <img src=":/emoji/smile.png>, without setting `BASEURL', as this this emoji can be compiled into resources.
             * 2) rich-text images edit/display with 'setBaseUrl()' is tech. possilbe as following test. But is it nesserary?
             *    tech. details: all distribution shared a uiniq "$APPHOME/res/<hash>" path to resouces, such as image, stylesheet, etc.
             */
            QTextCursor cursor = textEdit->textCursor();
            textEdit->document()->setBaseUrl(QUrl::fromLocalFile("D:/workspace/github/bitmail/build-bitmail-Desktop_Qt_5_7_0_MinGW_32bit-Debug/debug/emoji"));
            cursor.insertHtml("<img src=\"./emoji/like.png\" />");
            cursor.insertHtml("<img src=\"D:/workspace/github/bitmail/qt/images/add.png\" />");
        }
    }else{
        // to get a `reset' palin text format
        QTextEdit * plain = new QTextEdit();
        // setup plain mode
        textEdit->setCurrentCharFormat(plain->currentCharFormat());
        delete plain;
        textEdit->setPlainText(textEdit->document()->toPlainText());
        if (textEdit->acceptRichText()) textEdit->setAcceptRichText(false);
    }
}

void MainWindow::onBtnInviteClicked()
{
    InviteDialog inviteDialog(this);
    if (QDialog::Accepted != inviteDialog.exec()){
        return ;
    }
    std::vector<std::string> receips = BMQTApplication::toStdStringList(inviteDialog.GetEmail().split(",;:| \t\r\n"));
    QString sessKey = QString::fromStdString( BitMail::toSessionKey( BitMail::serializeReceips(receips)) );
    QString qsWhisper = inviteDialog.GetWhisper();
    QStringList attachments = inviteDialog.attachments();
    QStringList parts;
    parts.append( BMQTApplication::toMimeTextPlain( qsWhisper ));
    for(QStringList::const_iterator it = attachments.constBegin(); it != attachments.constEnd(); ++it){
        QString filePath = *it;
        if (!filePath.isEmpty() && QFileInfo(filePath).exists()){
            parts.append( BMQTApplication::toMimeAttachment(*it) );
        }
    }
    QString qsMsg = BMQTApplication::toMixed(parts);

    SendTo(sessKey, qsMsg);

    if (BitMail::isGroupSession(sessKey.toStdString())){
        if (!hasLeaf(nodeGroups, sessKey)){
            populateLeaf(nodeGroups, sessKey);
        }
    }else {
        if (!hasLeaf(nodeFriends, sessKey)){
            populateLeaf(nodeFriends, sessKey);
        }
    }

    return ;
}

void MainWindow::onBtnSendClicked()
{
    if (m_txth == NULL){
        txTip->setText(tr("No active network"));
        return ;
    }
    QString qsMsgPlain = !htmlAct->isChecked() ? textEdit->toPlainText() : textEdit->document()->toHtml();
    textEdit->clear();

    if (qsMsgPlain.isEmpty()){
        return ;
    }

    QString qsMsg = !htmlAct->isChecked() ? BMQTApplication::toMimeTextPlain(qsMsgPlain) : BMQTApplication::toMimeTextHtml(qsMsgPlain);

    Send(qsMsg);
}

void MainWindow::onBtnSendQrClicked()
{
    if (m_txth == NULL){
        txTip->setText(tr("No active network"));
        return ;
    }
    QString qsMsgPlain = !htmlAct->isChecked() ? textEdit->toPlainText() : textEdit->document()->toHtml();
    textEdit->clear();

    if (qsMsgPlain.isEmpty()){
        return ;
    }

    QString qsMsg = BMQTApplication::toMimeImage(BMQTApplication::toQrImage(qsMsgPlain).toImage());

    Send(qsMsg);
}

/**
 * @Note: lesson about signal/slot paramters match
 * 1) emit signal(MsgType), slot(MsgType) will not called, if MsgType is a custom enumerate type.
 * 2) in the case1, Q_DECLARE_METATYPE(MsgType) & aRegisterMetaType<MsgType>(), will make case1 to work.
 */
void MainWindow::onNewMessage(const QString & from
                              , const QString & qsTo /*receips*/
                              , const QString & content
                              , const QString & certid
                              , const QString & cert
                              , const QString & sigtime)
{   
    qDebug() << "receips: " << qsTo;

    std::vector<std::string> vec_receips = BitMail::decodeReceips(qsTo.toStdString());

    QString sessionKey = QString::fromStdString(BitMail::toSessionKey(from.toStdString()));

    if (vec_receips.size() > 1){
        sessionKey = QString::fromStdString(BitMail::toSessionKey(vec_receips));
    }

    if (BitMail::isGroupSession(sessionKey.toStdString())){
        if (!hasLeaf(nodeGroups, sessionKey)){
            populateLeaf(nodeGroups, sessionKey);
        }
    }else{
        if (!hasLeaf(nodeFriends, sessionKey)){
            populateLeaf(nodeFriends, sessionKey);
        }
    }

    QString receips = QString::fromStdString(BitMail::serializeReceips(vec_receips));    
    enqueueMsg(sessionKey, false, BMQTApplication::GetSigTime(sigtime), from, receips, content, certid, cert);

    if (!sessionKey.isEmpty() && sessionKey == this->currentSessionKey()){
        populateMessages(sessionKey);
    }

    this->activateWindow();
}

void MainWindow::enqueueMsg(const QString & k
                            , bool tx
                            , const QString & sigtime
                            , const QString & from
                            , const QString & to
                            , const QString & msg
                            , const QString & certid
                            , const QString & cert)
{
    QJsonObject obj;
    obj["tx"] = tx;
    obj["sigtime"] = sigtime;
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

void MainWindow::Send(const QString & qsMsg)
{
    QString sessionKey = currentSessionKey();
    SendTo(sessionKey, qsMsg);
}


void MainWindow::SendTo(const QString & sessionKey, const QString & qsMsg)
{
    std::vector<std::string> vec_receips = BitMail::fromSessionKey(sessionKey.toStdString());
    if (vec_receips.empty()){
        return ;
    }

    QString qsFrom = QString::fromStdString(m_bitmail->GetEmail());
    QString qsCertId = QString::fromStdString(m_bitmail->GetID());
    QString qsCert = QString::fromStdString(m_bitmail->GetCert());

    emit readyToSend(qsFrom, BMQTApplication::toQStringList(vec_receips), qsMsg);

    enqueueMsg(sessionKey, true, QDateTime::currentDateTime().toString(), qsFrom, QString::fromStdString( BitMail::serializeReceips(vec_receips) ), qsMsg, qsCertId, qsCert);

    populateMessages(sessionKey);
}

void MainWindow::onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    btnSend->setEnabled(false);
    btnSendQr->setEnabled(false);
    chatToolbar->setEnabled(false);
    sessLabel->setText("");

    if (current == NULL){
        return ;
    }

    QVariant qvData = current->data(0, Qt::UserRole);
    if (qvData.isNull()
        ||!qvData.isValid()
        ||QString::fromStdString(qvData.typeName()) != "QString")
    {
        return ;
    }

    QString sessKey = qvData.toString();

    /*Setup session label header*/
    sessLabel->setText( QString::fromStdString(m_bitmail->sessionName(sessKey.toStdString()))) ;
    btnSend->setEnabled(true);
    chatToolbar->setEnabled(true);
    btnSendQr->setEnabled(true);

    populateMessages(sessKey);

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
        if (msgObj["tx"].toBool()){
            msgElt->setIcon(BMQTApplication::Iconfy(QByteArray::fromStdString(m_bitmail->sessionLogo(BitMail::toSessionKey(m_bitmail->GetEmail())))));
        }else{
            if (BitMail::isGroupSession(k.toStdString())){
                //TODO: display a merged head
                msgElt->setIcon(QIcon(":/images/bubble.png"));
            }else{
                msgElt->setIcon(BMQTApplication::Iconfy(QByteArray::fromStdString(m_bitmail->sessionLogo(k.toStdString()))));
            }
        }

        QString qsMsg = msgObj["msg"].toString();

        QVariantList varlist = BMQTApplication::fromMime(qsMsg);
        varlist.insert(0, msgObj["sigtime"].toString());

        msgElt->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        msgElt->setBackgroundColor(msgObj["tx"].toBool() ? QColor(158,234,106) : Qt::lightGray);
        msgElt->setData(Qt::UserRole, msgObj);
        QWidget * widget = createMessageWidget(msgView->width()>>1, varlist);
        msgElt->setSizeHint(widget->sizeHint());
        msgView->addItem(msgElt);
        msgView->setItemWidget(msgElt, widget);
        msgView->scrollToBottom();
    }
}

QWidget * MainWindow::createMessageWidget(int width, const QVariantList &varlist)
{
    // ThanskTo:
    //http://stackoverflow.com/questions/948444/qlistview-qlistwidget-with-custom-items-and-custom-item-widgets
    //http://www.qtcentre.org/threads/27777-Customize-QListWidgetItem-how-to
    // http://stackoverflow.com/questions/9506586/qtextedit-resize-to-fit
    QWidget* widget = new QWidget;
    QVBoxLayout* vbox = new QVBoxLayout( widget );
    vbox->setSizeConstraint( QLayout::SetFixedSize );

    for (QVariantList::const_iterator it = varlist.constBegin(); it != varlist.constEnd(); ++it){
        QWidget * vElt = NULL;
        QVariant var = *it;
        if (QString::fromStdString(var.typeName()) == "QImage"){
            QImage qImage = qvariant_cast<QImage>(var);
            QLabel * tmp = new QLabel(widget);
            if (qImage.width() > width){
                tmp->setFixedSize(width, width*1.0f/qImage.width() * qImage.height());
                tmp->setScaledContents(true);
            }
            tmp->setPixmap(QPixmap::fromImage(qImage));
            vElt = tmp;
        }else if (QString::fromStdString(var.typeName()) == "QString"){
            QLabel * tmp = new QLabel(widget);
            tmp->setFixedWidth(width);
            tmp->setWordWrap(true);
            tmp->setText(var.toString());
            vElt = tmp;
        }else if (QString::fromStdString(var.typeName()) == "QByteArray"){
            QLabel * tmp = new QLabel(widget);
            tmp->setTextFormat(Qt::RichText);
            tmp->setText(QString::fromUtf8(var.toByteArray()));
            vElt = tmp;
        }else if (QString::fromStdString(var.typeName()) == "QFileInfo"){
            QFileInfo fileInfo = qvariant_cast<QFileInfo>(var);
            QLabel * tmp = new QLabel(widget);
            tmp->setPixmap((QFileIconProvider().icon(fileInfo).pixmap(QSize(64<width?64:width,64<width?64:width))));
            vElt = tmp;
        }else{

        }
        vbox->setSpacing(5);
        vbox->addWidget( vElt );
    }
    return widget;
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
    messageDialog.group(msgObj["to"].toString());

    connect(&messageDialog, SIGNAL(friendsChanged()), this, SLOT(populateFriendTree()));
    connect(&messageDialog, SIGNAL(groupsChanged()), this, SLOT(populateGroupTree()));

    messageDialog.exec();

    return ;
}

QString MainWindow::currentSessionKey()
{
    if (btree->currentItem() == NULL){
        return "";
    }

    QVariant qvData = btree->currentItem()->data(0, Qt::UserRole);
    if (!qvData.isValid()
        || qvData.isNull()
        || QString::fromStdString(qvData.typeName()) != "QString"){
        return "";
    }

    QString sessKey = qvData.toString();

    return sessKey;
}

void MainWindow::populateFriendTree()
{
    // clear all firstly
    while(nodeFriends->childCount()){
        QTreeWidgetItem * item = nodeFriends->child(0);
        nodeFriends->removeChild(item);
    }
    // Add buddies
    std::vector<std::string> vecEmails;
    m_bitmail->GetFriends(vecEmails);
    for (std::vector<std::string>::const_iterator it = vecEmails.begin()
         ; it != vecEmails.end(); ++it)
    {
        QString sessKey = QString::fromStdString( BitMail::toSessionKey(*it) );
        if (BitMail::isGroupSession(sessKey.toStdString())){
            continue;
        }
        populateLeaf(nodeFriends, sessKey);
    }
    return ;
}


void MainWindow::populateGroupTree()
{
    // clear all firstly
    while(nodeGroups->childCount()){
        QTreeWidgetItem * item = nodeGroups->child(0);
        nodeGroups->removeChild(item);
    }
    // Add groups
    std::vector<std::string> vecGroups;
    m_bitmail->GetGroups(vecGroups);
    for (std::vector<std::string>::const_iterator it = vecGroups.begin()
         ; it != vecGroups.end(); ++it)
    {
        QString sessKey = QString::fromStdString( BitMail::toSessionKey( BitMail::decodeReceips(*it) ) );
        if (!BitMail::isGroupSession(sessKey.toStdString())){
            continue;
        }
        populateLeaf(nodeGroups, sessKey);
    }
    return ;
}

bool MainWindow::hasLeaf(QTreeWidgetItem * node, const QString &sessKey)
{
    for(int i = 0; i < node->childCount(); i++){
        QTreeWidgetItem * elt = node->child(i);
        if (elt->data(0, Qt::UserRole).toString() == sessKey){
            return true;
        }
    }
    return false;
}

void MainWindow::removeLeaf(QTreeWidgetItem * node, const QString & sessKey)
{
    for (int j = 0; j < node->childCount(); j++){
        QTreeWidgetItem * leaf = node->child(j);
        if (leaf->data(0, Qt::UserRole).toString() == sessKey){
            node->removeChild(leaf);
            return ;
        }
    }
}

void MainWindow::populateLeaf(QTreeWidgetItem * node, const QString & sessKey)
{
    QStringList qslText;
    qslText.append(QString::fromStdString(m_bitmail->sessionName(sessKey.toStdString())));

    QTreeWidgetItem *buddy = new QTreeWidgetItem(node, qslText, 0);

    //TODO: logo synthesize
    if (BitMail::isGroupSession(sessKey.toStdString())){
        buddy->setIcon(0, QIcon(":/images/group.png"));
    }else{
        std::string b64logo = m_bitmail->sessionLogo(sessKey.toStdString());
        if (!b64logo.empty()){
            buddy->setIcon(0, BMQTApplication::Iconfy(QByteArray::fromStdString(b64logo)));
        }else{
            buddy->setIcon(0, QIcon(":/images/head.png"));
        }
    }
    buddy->setData(0, Qt::UserRole, sessKey);

    node->addChild(buddy);
    return ;
}

void MainWindow::onProcThDone(const QString & sessKey, const QString &output)
{
    if (QFileInfo(output).exists() && QFileInfo(output).size()){
        QString qsMsg = BMQTApplication::toMimeAttachment(output);
        SendTo(sessKey, qsMsg);
    }
    return ;
}
