#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QStatusBar>
#include <QProgressBar>

#include <bitmailcore/bitmail.h>
#include <json/json.h>

#include "threadgetmessage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_textEdit_textChanged();

    void onMessageGot(QString  sJosnMsg);

    void on_actionNetwork_triggered();

    void on_actionAdd_triggered();

    void onStrangerRequest(QString sJsonParam);

    void on_actionRemove_triggered();

    void on_actionView_triggered();

    void on_actionProfile_triggered();

    void on_actionRequest_triggered();

    void on_actionQrcode_triggered();

    void on_actionQrcode_2_triggered();

    void on_actionHelp_triggered();

    void on_actionVersion_triggered();

    void on_actionThanks_triggered();

    void on_actionDonation_triggered();

private:
    std::string ReadPlainTextFile(const std::string & path);

    bool LoadJsonConfig(const std::string & configPath, Json::Value & jsonConfig);

    void ResetTitle();

    void LoadBuddy(const std::string & sCertPem);

    void UnloadSelectedBuddy();

    void Config_AddBody(const std::string & sCertPem);

    void Config_RemoveBody(const std::string & email);

    bool CreateQrImage(const std::string & info, const std::string & path);

public:
    bool Initialize( const std::string & email, const std::string & keyPass);

    void UpdateProgress(int prog);
private:
    ThreadGetMessage * m_pThGetMessage;

    Ui::MainWindow *ui;

    QListWidget * m_buddy;

    QListWidget * m_msg;

    QTextEdit * m_input;

    QStatusBar * m_stbar;

    QProgressBar * m_progbar;

    BitMail * m_mail;

    std::string m_currentBuddy;
};

#endif // MAINWINDOW_H
