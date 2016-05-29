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


//! [1]
MainWindow::MainWindow()
//! [1] //! [2]
{
    bm  = new BitMail();

    bm->CreateProfile("", "", "", 1024);

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

//! [3]
void MainWindow::closeEvent(QCloseEvent *event)
//! [3] //! [4]
{
    if (true) {
        event->accept();
    }
}
//! [4]

//! [5]
void MainWindow::newFile()
//! [5] //! [6]
{
    OptionDialog optDialog(this);
    optDialog.exec();
}
//! [6]

//! [7]
void MainWindow::open()
//! [7] //! [8]
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Profile"));
    if (fileName.isEmpty()){
        return ;
    }
    loadProfile(fileName);
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
}
//! [24]

//! [29] //! [30]
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Profile"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(configAct);

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
void MainWindow::loadProfile(const QString &fileName)
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

    textEdit->setPlainText(in.readAll());

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    //setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}
//! [43]

//! [44]
bool MainWindow::saveProfile(const QString &fileName)
//! [44] //! [45]
{
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

    out << textEdit->toPlainText();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    statusBar()->showMessage(tr("File saved"), 2000);
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

void MainWindow::onConfigBtnClicked()
{
    OptionDialog optDialog;
    optDialog.SetEmail("me@mine.net");
    optDialog.exec();
    statusBar()->showMessage(optDialog.GetEmail());
    return ;
}
