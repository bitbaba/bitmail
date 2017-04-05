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
#include <QProcess>
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
class QProcess;
class QAudioRecorder;

QT_END_NAMESPACE
class BitMail;
class RxThread;
class TxThread;
class BradThread;
class ShutdownDialog;
class arDialog;

enum nodeType{
    NT_Undef = 0,
    NT_Friends,
    NT_Friend,
    NT_Stranger,
    NT_Groups,
    NT_Group,
    NT_StrangerGroup,
    NT_Subscribes,
    NT_Subscribe,
    NT_StrangerSub,
};

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(BitMail * bitmail);
    ~MainWindow();
    void configNetwork();
    void startupNetwork();
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
public slots:
    void onBtnSendClicked();
    void onBtnConfigClicked();
    void onBtnInviteClicked();

    void onNewMessage(const QString & from, const QString & msg, const QString & certid, const QString & cert);
    void onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void onTreeBuddyDoubleClicked(QTreeWidgetItem * actItem,int col);
    void onMessageDoubleClicked(QListWidgetItem * actItem);

    void onRxDone();
    void onRxProgress(const QString & info);
    void onTxDone();
    void onTxProgress(const QString & info);

    void onRemoveAct();
    void onNetAct();
    void onFileAct();
    void onAudioAct();

    void onAddFriend(const QString & email);
    void onDurationChanged(qint64);
private:    
    QString getCurrentReceipt();

    void createActions();
    void createToolBars();
    void createStatusBar();
    void loadProfile(const QString &fileName, const QString & passphrase);
    bool saveProfile(const QString &fileName);

    void ViewCert(const QString & e);

    void populateFriendLeaf(QTreeWidgetItem * node, const QString &email, const QString &nick,  nodeType nt);
    void populateFriendTree(QTreeWidgetItem * node);
    void populateMessages(const QString & k);

    void shutdownNetwork();

    void enqueueMsg(const QString & k, bool tx, const QString & from, const QString & to, const QString & msg, const QString & certid, const QString & cert);
    QStringList dequeueMsg(const QString &key);
private:
    QAudioRecorder *audioRecorder;
    QPlainTextEdit *textEdit;
    QTreeWidget * btree;
    QTreeWidgetItem * nodeFriends;

    QLabel * sessLabel;
    QListWidget *msgView;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *chatToolbar;
    QToolBar *netToolbar;
    QPushButton * btnSend;

    QAction *configAct;
    QAction *inviteAct;
    QAction *removeAct;
    QAction *netAct;
    QAction *fileAct;
    QAction *audioAct;

private:
    BitMail *m_bitmail;

    RxThread *m_rxth;
    TxThread *m_txth;

    ShutdownDialog *m_shutdownDialog;
    arDialog * m_arDlg;

    // histroy messages queue
    std::map<QString, QStringList> m_peermsgQ;

signals:
    void readyToSend(  const QString & from
                     , const QString & to
                     , const QString & msg);
};


enum MPType {
    MPT_undef = 0,
    MPT_textplain = 1,
    MPT_image = 2,
};

extern std::string MP_Compose(MPType t, const std::string & data);

//! [0]
#endif
