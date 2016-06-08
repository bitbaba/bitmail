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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QListWidget;
class QPushButton;
class QTextEdit;
class QLabel;
class QLineEdit;
class QJsonDocument;
class QJsonArray;
class QJsonObject;
class QJsonValue;
class QListWidgetItem;
QT_END_NAMESPACE

class BitMail;
class PollThread;
class RxThread;
class TxThread;
class ShutdownDialog;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(BitMail * bitmail);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void documentWasModified();
    void onSendBtnClicked();
    void onConfigBtnClicked();
    void onPayBtnClicked();
    void onWalletBtnClicked();
    void onNewMessage(const QString & from, const QString & msg, const QString & cert);
    void onCurrentBuddy(QListWidgetItem * current, QListWidgetItem * previous);
    void onRxDone();
    void onTxDone();
    void onPollDone();
    void onBuddyDoubleClicked(QListWidgetItem * actItem);
    void onInviteBtnClicked();
    void onMessageDoubleClicked(QListWidgetItem * actItem);
    void onUPnPBtnClicked();
    void onProxyBtnClicked();
    void onMakeFriend(const QString & email, const QString & cert);
private:
    void createActions();
    void createToolBars();
    void createStatusBar();
    void loadProfile(const QString &fileName, const QString & passphrase);
    bool saveProfile(const QString &fileName);
    void populateMessage(bool fTx, const QString & from, const QString & msg, const QString & cert);
    void populateBuddy(const QString & email, const QString & nick);
    void populateBuddies();
    void clearMsgView();
    void populateMsgView(const QString & email);
private:
    QTextEdit *textEdit;
    QListWidget * blist;
    QListWidget *msgView;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *chatToolbar;
    QToolBar *walletToolbar;
    QToolBar *netToolbar;

    QPushButton * btnSend;

    QAction *configAct;

    QAction *inviteAct;

    QAction *upnpAct;

    QAction *proxyAct;

    QAction *snapAct;
    QAction *fileAct;
    QAction *emojAct;
    QAction *soundAct;
    QAction *videoAct;
    QAction *liveAct;
    QAction *payAct;
    QAction *walletAct;
private:
    BitMail *m_bitmail;
    PollThread *m_pollth;
    RxThread *m_rxth;
    TxThread *m_txth;
    ShutdownDialog *m_shutdownDialog;
signals:
    void readyToSend(const QString & to, const QString & msg);
};
//! [0]

#endif
