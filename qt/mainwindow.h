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
#include "msgqueue.h"

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
class QTreeWidget;
class QTreeWidgetItem;

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
public slots:
    void documentWasModified();
    void onSendBtnClicked();
    void onConfigBtnClicked();
    void onPayBtnClicked();
    void onWalletBtnClicked();
    void onRssBtnClicked();
    void onNewMessage(const QString & from
                      , const QStringList & recip
                      , const QString & msg
                      , const QString & certid
                      , const QString & cert);

    void onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void onRxDone();
    void onRxProgress(const QString & info);
    void onTxDone();
    void onTxProgress(const QString & info);
    void onPollDone();
    void onPollProgress(const QString & info);

    void onTreeBuddyDoubleClicked(QTreeWidgetItem * actItem,int col);
    void onInviteBtnClicked();
    void onMessageDoubleClicked(QListWidgetItem * actItem);
    void onAddFriend(const QString & email);
    void onNewSubscribe(const QString & sub);

private:
    bool getCurrentRecipKey(MsgType & mt, QString & qsKey);
    bool getRecipList(MsgType, const QString & qsKey, QStringList & reciplist);
    void createActions();
    void createToolBars();
    void createStatusBar();
    void loadProfile(const QString &fileName, const QString & passphrase);
    bool saveProfile(const QString &fileName);
    void populateMessage(const RTXMessage & rtxMsg);
    void populateFriendLeaf(QTreeWidgetItem * node, const QString &email, const QString &nick);
    void populateGroupLeaf(QTreeWidgetItem * node, const QString & groupid, const QString & groupname);
    void populateSubscribeLeaf(QTreeWidgetItem * node, const QString & email, const QString &nick);

    void populateFriendTree(QTreeWidgetItem * node);
    void populateGroupTree(QTreeWidgetItem * node);
    void populateSubscribeTree(QTreeWidgetItem * node);
    void clearMsgView();
    void populateMsgView(const QString & email);
    void startupNetwork();
    void shutdownNetwork();
    void setNotify(QTreeWidgetItem * item, int no);
    void enqueueMsg(MsgType mt, const QString & key, const RTXMessage & rtxMsg);
    QList<RTXMessage> dequeueMsg(MsgType mt, const QString & key);
    QString formatRTXMessage(const RTXMessage & rtxMsg);
private:
    QPlainTextEdit *textEdit;
    QTreeWidget * btree;
    QTreeWidgetItem * nodeFriends;
    QTreeWidgetItem * nodeGroups;
    QTreeWidgetItem * nodeSubscribes;
    //QTreeWidgetItem * nodeStrangers;
    QListWidget *msgView;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *chatToolbar;
    QToolBar *walletToolbar;
    QToolBar *rssToolbar;
    QToolBar *netToolbar;
    QPushButton * btnSend;
    QAction *configAct;
    QAction *inviteAct;
    QAction *snapAct;
    QAction *fileAct;
    QAction *emojAct;
    QAction *soundAct;
    QAction *videoAct;
    QAction *liveAct;
    QAction *payAct;
    QAction *walletAct;
    QAction *rssAct;
private:
    BitMail *m_bitmail;
    PollThread *m_pollth;
    RxThread *m_rxth;
    TxThread *m_txth;
    ShutdownDialog *m_shutdownDialog;
    // histroy messages queue
    std::map<QString, QStringList> m_peermsgQ;
    std::map<QString, QStringList> m_groupmsgQ;
    std::map<QString, QStringList> m_submsgQ;
signals:
    void readyToSend(  const QString & from
                     , const QStringList & recip
                     , const QString & msg);

};


//! [0]
#endif
