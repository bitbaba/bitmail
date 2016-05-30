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

#include "optiondialog.h"
#include "logindialog.h"

//! [1]
MainWindow::MainWindow()
//! [1] //! [2]
{
    bm  = new BitMail();

    LoginDialog loginDialog;
    loginDialog.exec();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    blist = new QListWidget;
    leftLayout->addWidget(blist);
    mainLayout->addLayout(leftLayout);

    QVBoxLayout * rightLayout = new QVBoxLayout;

    mlist = new QListWidget;
    textEdit = new QTextEdit;

    QHBoxLayout * btnLayout = new QHBoxLayout;
    btnSend = new QPushButton(tr("Send"));
    btnSend->setFixedWidth(64);
    btnSend->setFixedHeight(32);
    connect(btnSend, SIGNAL(clicked()), this, SLOT(onSendBtnClicked()));

    btnLayout->addWidget(btnSend);
    btnLayout->setAlignment(btnSend, Qt::AlignLeft);

    mainLayout->addLayout(rightLayout);

    QWidget * wrap = new QWidget(this);
    wrap->setLayout(mainLayout);
    setCentralWidget(wrap);

    createActions();
    createToolBars();
    createStatusBar();

    rightLayout->addWidget(mlist);
    rightLayout->addWidget(chatToolbar);
    rightLayout->addWidget(textEdit);
    rightLayout->addLayout(btnLayout);

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

#if defined(MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    setWindowIcon(QIcon(":/images/bitmail.png"));
    setWindowTitle(tr("BitMail"));
}
//! [2]

MainWindow::~MainWindow()
{
    if (bm != NULL){
        delete bm;
        bm = NULL;
    }
}

//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    event->accept();
}
//! [4]

//! [17]
void MainWindow::createActions()
//! [17] //! [18]
{
    do {
        newAct = new QAction(QIcon(":/images/profile.png"), tr("&NewAccount"), this);
        newAct->setStatusTip(tr("Create a new bitmail account profile"));
        connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
    }while(0);

    do {
        configAct = new QAction(QIcon(":/images/config.png"), tr("&Configure"), this);
        configAct->setStatusTip(tr("Configure current account"));
        connect(configAct, SIGNAL(triggered()), this, SLOT(onConfigBtnClicked()));
    }while(0);

//! [19]
    do {
        openAct = new QAction(QIcon(":/images/open.png"), tr("&Load..."), this);
        openAct->setStatusTip(tr("Loading an existing account profile"));
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    }while(0);
//! [18] //! [19]

    do {
        saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
        saveAct->setStatusTip(tr("Save the document to disk"));
        connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    }while(0);

    do {
        addAct = new QAction(QIcon(":/images/add.png"), tr("&Add"), this);
        addAct->setStatusTip(tr("Add a new buddy by certificate or certificate ID."));
    }while(0);

    do {
        inviteAct = new QAction(QIcon(":/images/invite.png"), tr("&Invite"), this);
        inviteAct->setStatusTip(tr("Invite a new friend by send a request message."));
    }while(0);

    do{
        snapAct = new QAction(QIcon(":/images/snap.png"), tr("&Snapshot"), this);
        snapAct->setStatusTip(tr("Snapshot"));
        snapAct->setEnabled(false);
    }while(0);

    do{
        fileAct = new QAction(QIcon(":/images/file.png"), tr("&File"), this);
        fileAct->setStatusTip(tr("File"));
        fileAct->setEnabled(false);
    }while(0);

    do{
        emojAct = new QAction(QIcon(":/images/emoj.png"), tr("&Emoji"), this);
        emojAct->setStatusTip(tr("Emoji"));
        emojAct->setEnabled(false);
    }while(0);

    do{
        soundAct = new QAction(QIcon(":/images/sound.png"), tr("&Sound"), this);
        soundAct->setStatusTip(tr("Sound"));
        soundAct->setEnabled(false);
    }while(0);

    do{
        videoAct = new QAction(QIcon(":/images/video.png"), tr("&Video"), this);
        videoAct->setStatusTip(tr("Video"));
        videoAct->setEnabled(false);
    }while(0);

    do{
        styleAct = new QAction(QIcon(":/images/style.png"), tr("&Font"), this);
        connect(styleAct, SIGNAL(triggered()), this, SLOT(onStyleBtnClicked()));
        styleAct->setStatusTip(tr("Font"));
    }while(0);

    do{
        colorAct = new QAction(QIcon(":/images/color.png"), tr("&Color"), this);
        connect(colorAct, SIGNAL(triggered()), this, SLOT(onColorBtnClicked()));
        colorAct->setStatusTip(tr("Color"));
    }while(0);

    do{
        liveAct = new QAction(QIcon(":/images/live.png"), tr("&Live"), this);
        liveAct->setStatusTip(tr("Live"));
        liveAct->setEnabled(false);
    }while(0);

    do{
        textAct = new QAction(QIcon(":/images/text.png"), tr("&Text"), this);
        textAct->setStatusTip(tr("Text"));
        textAct->setCheckable(true);
        connect(textAct, SIGNAL(triggered(bool)), this, SLOT(onTextBtnClicked(bool)));
    }while(0);

    do{
        payAct = new QAction(QIcon(":/images/bitcoin.png"), tr("&Pay"), this);
        payAct->setStatusTip(tr("Pay by Bitcoin"));
        connect(payAct, SIGNAL(triggered(bool)), this, SLOT(onPayBtnClicked(bool)));
    }while(0);
}
//! [24]

//! [29] //! [30]
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(configAct);
    fileToolBar->addAction(newAct);

    editToolBar = addToolBar(tr("Buddies"));
    editToolBar->addAction(addAct);
    editToolBar->addAction(inviteAct);

    chatToolbar = addToolBar(tr("Chat"));
    chatToolbar->addAction(textAct);
    chatToolbar->addAction(styleAct);
    chatToolbar->addAction(colorAct);
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

//! [42]
void MainWindow::loadProfile(const QString &fileName, const QString & passphrase)
//! [42] //! [43]
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("BitMail"),
                             tr("Cannot read file: ")
                             + (fileName)
                             + "\n"
                             + (file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QJsonDocument jdoc;
    jdoc = QJsonDocument::fromJson(in.readAll().toLatin1());

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    QJsonObject joRoot = jdoc.object();
    QJsonObject joProfile;
    QJsonObject joTx;
    QJsonObject joRx;
    QJsonArray  jaBuddies;

    if (joRoot.contains("Profile")){
        joProfile = joRoot["Profile"].toObject();

        QString qsEmail;
        if (joProfile.contains("email")){
            qsEmail = joProfile["email"].toString();
        }
        QString qsNick;
        if (joProfile.contains("nick")){
            qsNick = joProfile["nick"].toString();
        }
        QString qsCert;
        if (joProfile.contains("cert")){
            qsCert = joProfile["cert"].toString();
        }
        QString qsKey;
        if (joProfile.contains("key")){
            qsKey = joProfile["key"].toString();
        }

        (void)passphrase;
        bm->LoadProfile(passphrase.toStdString()
                        , qsKey.toStdString()
                        , qsCert.toStdString());

    }

    QString qsTxUrl, qsTxLogin, qsTxPassword;
    if (joRoot.contains("tx")){
        joTx = joRoot["tx"].toObject();
        if (joTx.contains("url")){
            qsTxUrl = joTx["url"].toString();
        }
        if (joTx.contains("login")){
            qsTxLogin = joTx["login"].toString();
        }
        if (joTx.contains("password")){
            qsTxPassword = QString::fromStdString( bm->Decrypt(joTx["password"].toString().toStdString()));
        }
    }

    QString qsRxUrl, qsRxLogin, qsRxPassword;
    if (joRoot.contains("rx")){
        joRx = joRoot["rx"].toObject();
        if (joRx.contains("url")){
            qsRxUrl = joRx["url"].toString();
        }
        if (joRx.contains("login")){
            qsRxLogin = joRx["login"].toString();
        }
        if (joRx.contains("password")){
            qsRxPassword = QString::fromStdString( bm->Decrypt(joRx["password"].toString().toStdString()));
        }
    }

    bm->InitNetwork(qsTxUrl.toStdString(), qsTxLogin.toStdString(), qsTxPassword.toStdString()
                    , qsRxUrl.toStdString(), qsRxLogin.toStdString(), qsRxPassword.toStdString());

    if (joRoot.contains("buddies")){
        jaBuddies = joRoot["buddies"].toArray();
        for(QJsonArray::const_iterator it = jaBuddies.constBegin()
            ; it != jaBuddies.constEnd()
            ; it++){
            const QJsonObject & joBuddy = (*it).toObject();
            QString qsEmail = joBuddy["email"].toString();
            QString qsCert =  joBuddy["cert"].toString();
            (void )qsEmail;
            bm->AddBuddy(qsCert.toStdString());
        }
    }

    statusBar()->showMessage(tr("File loaded"), 2000);
}
//! [43]

//! [44]
bool MainWindow::saveProfile(const QString &fileName)
//! [44] //! [45]
{
    // Format Profile to QJson
    QJsonObject joRoot;
    QJsonObject joProfile;
    QJsonObject joTx;
    QJsonObject joRx;
    QJsonArray  jaBuddies;

    joProfile["email"] = QString::fromStdString(bm->GetEmail());
    joProfile["nick"] = QString::fromStdString(bm->GetCommonName());
    joProfile["key"] = QString::fromStdString(bm->GetKey());
    joProfile["cert"] = QString::fromStdString(bm->GetCert());

    joTx["url"] = QString::fromStdString(bm->GetTxUrl());
    joTx["login"] = QString::fromStdString(bm->GetTxLogin());
    joTx["password"] = QString::fromStdString(bm->Encrypt(bm->GetTxPassword()));

    joRx["url"] = QString::fromStdString(bm->GetRxUrl());
    joRx["login"] = QString::fromStdString(bm->GetRxLogin());
    joRx["password"] = QString::fromStdString(bm->Encrypt(bm->GetRxPassword()));
    joRx["stranger"] = bm->AllowStranger();

    std::vector<std::string > vecBuddies;
    bm->GetBuddies(vecBuddies);
    for (std::vector<std::string>::const_iterator it = vecBuddies.begin(); it != vecBuddies.end(); ++it){
        std::string sBuddyCertPem = bm->GetBuddyCert(*it);
        QJsonObject joBuddy;
        joBuddy["email"] = QString::fromStdString(*it);
        joBuddy["cert"]  = QString::fromStdString(sBuddyCertPem);
        jaBuddies.append(joBuddy);
    }

    // for more readable, instead of `profile'
    joRoot["Profile"] = joProfile;
    joRoot["tx"] = joTx;
    joRoot["rx"] = joRx;
    joRoot["buddies"] = jaBuddies;

    QJsonDocument jdoc(joRoot);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("BitMail"),
                             tr("Cannot write file:")
                             + (fileName)
                             + "\n"
                             + (file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    out << jdoc.toJson();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("Profile saved"), 2000);
    return true;
}
//! [45]

//! [48]
QString MainWindow::strippedName(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}
//! [49]

QString MainWindow::GetProfileHome()
{
    QString qsProfileHome = QDir::homePath();
    qsProfileHome += "/";
    qsProfileHome += "bitmail";
    qsProfileHome += "/";
    qsProfileHome += "profile";
    return qsProfileHome;
}

QString MainWindow::GetDataHome()
{
    QString qsProfileHome = QDir::homePath();
    qsProfileHome += "/";
    qsProfileHome += "bitmail";
    qsProfileHome += "/";
    qsProfileHome += "data";
    return "";
}

void MainWindow::onStyleBtnClicked()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, textEdit->font());
    if (ok){
        textEdit->setFont(font);
    }
    return ;
}

void MainWindow::onColorBtnClicked()
{
    QColor color = QColorDialog::getColor(Qt::green, this, tr("Select Color"), QColorDialog::DontUseNativeDialog);
    textEdit->setTextColor(color);
    textEdit->setAutoFillBackground(true);
    return ;
}

void MainWindow::onTextBtnClicked(bool fchecked)
{
    styleAct->setEnabled(!fchecked);
    colorAct->setEnabled(!fchecked);
    //emojAct->setEnabled(!fchecked);
    //snapAct->setEnabled(!fchecked);
    //soundAct->setEnabled(!fchecked);
    //videoAct->setEnabled(!fchecked);
    //fileAct->setEnabled(!fchecked);
    return ;
}

void MainWindow::onStrangerBtnClicked(bool fchecked)
{
    if (fchecked){

    }
    return ;
}

void MainWindow::onSendBtnClicked()
{
    return ;
}

void MainWindow::onPayBtnClicked()
{
    return ;
}

//! [5]
void MainWindow::newFile()
//! [5] //! [6]
{
    OptionDialog optDialog(true, this);
    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }

    QString qsEmail = optDialog.GetEmail();
    QString qsNick = optDialog.GetNick();
    QString qsPassphrase = optDialog.GetPassphrase();
    int nBits = optDialog.GetBits();

    QString qsSmtpUrl = optDialog.GetSmtpUrl();
    QString qsSmtpLogin = optDialog.GetSmtpLogin();
    QString qsSmtpPassword = optDialog.GetSmtpPassword();

    QString qsImapUrl = optDialog.GetImapUrl();
    QString qsImapLogin = optDialog.GetImapLogin();
    QString qsImapPassword = optDialog.GetImapPassword();
    bool fImapAllowStranger = optDialog.GetImapAllowStranger();

    bm->CreateProfile(qsNick.toStdString()
                      , qsEmail.toStdString()
                      , qsPassphrase.toStdString()
                      , nBits);

    bm->InitNetwork(qsSmtpUrl.toStdString()
                    , qsSmtpLogin.toStdString()
                    , qsSmtpPassword.toStdString()
                    , qsImapUrl.toStdString()
                    , qsImapLogin.toStdString()
                    , qsImapPassword.toStdString());

    bm->AllowStranger(fImapAllowStranger);
}
//! [6]

void MainWindow::onConfigBtnClicked()
{
    OptionDialog optDialog(false, this);
    optDialog.SetEmail(QString::fromStdString(bm->GetEmail()));
    optDialog.SetNick(QString::fromStdString(bm->GetCommonName()));
    optDialog.SetPassphrase(QString::fromStdString(bm->GetPassphrase()));
    optDialog.SetBits(bm->GetBits());

    optDialog.SetSmtpUrl(QString::fromStdString(bm->GetTxUrl()));
    optDialog.SetSmtpLogin(QString::fromStdString(bm->GetTxLogin()));
    optDialog.SetSmtpPassword(QString::fromStdString(bm->GetTxPassword()));

    optDialog.SetImapUrl(QString::fromStdString(bm->GetRxUrl()));
    optDialog.SetImapLogin(QString::fromStdString(bm->GetRxLogin()));
    optDialog.SetImapPassword(QString::fromStdString(bm->GetRxPassword()));
    optDialog.SetImapAllowStranger(bm->AllowStranger());

    if (QDialog::Accepted != optDialog.exec()){
        return ;
    }
    return ;
}

//! [7]
void MainWindow::open()
//! [7] //! [8]
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Profile"));
    if (fileName.isEmpty()){
        return ;
    }
    loadProfile(fileName, "");
}
//! [8]

//! [9]
bool MainWindow::save()
//! [9] //! [10]
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Profile"));
    if (fileName.isEmpty()){
        return false;
    }
    saveProfile(fileName);
    return true;
}
//! [10]

//! [15]
void MainWindow::documentWasModified()
//! [15] //! [16]
{

}
//! [16]
