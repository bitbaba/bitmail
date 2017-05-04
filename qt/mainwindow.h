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
class QProgressBar;
class QComboBox;
class QFontComboBox;
class QTextCharFormat;

class ClickableLabel;
class TextEdit;

QT_END_NAMESPACE
class BitMail;
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
    void onBtnSendClicked();
    void onBtnSendQrClicked();
    void onBtnConfigClicked();
    void onBtnInviteClicked();
    void onHelpAct();
    void onLabelDoubleClicked(ClickableLabel * label);
    void onTextEditDropped(const QVariantList & varlist);

    void onNewMessage(const QString & from, const QString & receips, const QString & msg, const QString & certid, const QString & cert, const QString & sigtime);
    void onTreeCurrentBuddy(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void onTreeBuddyDoubleClicked(QTreeWidgetItem * actItem,int col);
    void onMessageDoubleClicked(QListWidgetItem * actItem);

    void onRxDone();
    void onRxProgress(int st, const QString & info);
    void onTxDone();
    void onTxProgress(int st, const QString & info);

    void onProcThDone(const QString & sessKey, const QString & output);

    void onRemoveAct();
    void onFileAct();
    void onAudioAct();
    void onEmojiAct();
    void onSnapAct();
    void onPhotoAct();
    void onVideoAct();
    void onHtmlAct(bool fChecked);

    void populateFriendTree();
    void populateGroupTree();

private slots:
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();

    void currentCharFormatChanged(const QTextCharFormat &format);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);

    QString currentSessionKey();

    void createActions();
    void setupTextActions();
    void createToolBars();
    void createStatusBar();
    QWidget * createMessageWidget(int width, const QVariantList & varlist);

    void loadProfile(const QString &fileName, const QString & passphrase);
    bool saveProfile(const QString &fileName);

    void ViewCert(const QString & e);

    bool hasLeaf(QTreeWidgetItem * node, const QString & sessKey);

    void populateLeaf(QTreeWidgetItem * node, const QString & sessKey);

    void populateMessages(const QString & k);

    void shutdownNetwork();

    void enqueueMsg(const QString & k, bool tx, const QString & sigtime, const QString & from, const QString & to, const QString & msg, const QString & certid, const QString & cert);
    QStringList dequeueMsg(const QString &key);

    void Send(const QString & msg);
    void SendTo(const QString & sessKey, const QString & msg);
private:
    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

    TextEdit *textEdit;
    QTreeWidget * btree;
    QTreeWidgetItem * nodeFriends;
    QTreeWidgetItem * nodeGroups;

    QLabel * sessLabel;
    QListWidget *msgView;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *chatToolbar;
    QToolBar *netToolbar;
    QToolBar *textToolbar;
    QPushButton * btnSend;
    QPushButton * btnSendQr;
    QProgressBar * rxProg;
    QLabel * rxTip;
    QProgressBar * txProg;
    QLabel * txTip;

    QAction *configAct;
    QAction *inviteAct;
    QAction *helpAct;
    QAction *fileAct;
    QAction *audioAct;
    QAction *emojiAct;
    QAction *snapAct;
    QAction *photoAct;
    QAction *videoAct;
    QAction *htmlAct;

    // Text edit options
    QAction *actionTextBold;
    QAction *actionTextUnderline;
    QAction *actionTextItalic;
    QAction *actionTextColor;

private:
    BitMail *m_bitmail;

    RxThread *m_rxth;
    TxThread *m_txth;

    ShutdownDialog *m_shutdownDialog;

    // histroy messages queue
    std::map<QString, QStringList> m_peermsgQ;

signals:
    void readyToSend(  const QString & from
                     , const QStringList & to
                     , const QString & msg);
};

//! [0]
#endif
