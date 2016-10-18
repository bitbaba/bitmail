#include "assistantdialog.h"
#include "ui_assistantdialog.h"
#include "bitmailcore/bitmail.h"
#include <QDebug>
#include <QMessageBox>
#include <QClipboard>

AssistantDialog::AssistantDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssistantDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);

#if defined(MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowIcon(QIcon(":/images/bitmail.png"));

    m_txtInput = findChild<QTextEdit*>("txtInput");
    m_btnEncrypt = findChild<QPushButton*>("btnEncrypt");
    m_btnDecrypt = findChild<QPushButton*>("btnDecrypt");
    m_btnSign = findChild<QPushButton*>("btnSign");
    m_btnAddFriend = findChild<QPushButton*>("btnAddFriend");
    m_cbbFriends = findChild<QComboBox*>("cbbFriends");

    m_leNick = findChild<QLineEdit*>("leNick");
    m_leEmail = findChild<QLineEdit*>("leEmail");
    m_leCertId = findChild<QLineEdit*>("leCertId");
    m_txtCert = findChild<QTextEdit*>("txtCert");
    m_txtOutput = findChild<QTextEdit*>("txtOutput");

    // Populate friend list
    do {
        m_cbbFriends->addItem(QIcon(":/images/group.png")
                              , tr("All")
                              , QVariant(QString("*")));

        std::vector<std::string> vecGroupIds;
        m_bitmail->GetGroups(vecGroupIds);
        for (unsigned int i = 0; i < vecGroupIds.size(); ++i){
            std::string sGroupName;
            m_bitmail->GetGroupName(vecGroupIds[i], sGroupName);
            m_cbbFriends->addItem(QIcon(":/images/group.png")
                                  , QString::fromStdString(sGroupName)
                                  , QVariant(QString("#") + QString::fromStdString(vecGroupIds[i])));
        }
        std::vector<std::string> vecFriends;
        m_bitmail->GetFriends(vecFriends);
        for (std::vector<std::string>::const_iterator it = vecFriends.begin();
              it != vecFriends.end(); ++it){
            std::string sNick=m_bitmail->GetFriendNick(*it);
            m_cbbFriends->addItem(QIcon(":/images/head.png")
                                  , QString::fromStdString(sNick)
                                  , QVariant(QString::fromStdString(*it)));
        }
    }while(0);
    // Listen clipboards
    m_clip = QApplication::clipboard();
    connect(m_clip, SIGNAL(dataChanged()), this, SLOT(onClipDataChanged()));
}

AssistantDialog::~AssistantDialog()
{
    delete ui;
}

QString AssistantDialog::nick() const
{
    return m_leNick->text();
}
void AssistantDialog::nick(const QString & n)
{
    m_leNick->setText(n);
}

QString AssistantDialog::email() const
{
    return m_leEmail->text();
}
void AssistantDialog::email(const QString & e)
{
    m_leEmail->setText(e);
}

QString AssistantDialog::certid() const
{
    return m_leCertId->text();
}
void AssistantDialog::certid(const QString & c)
{
    m_leCertId->setText(c);
}

QString AssistantDialog::cert() const
{
    return m_txtCert->toPlainText();
}
void AssistantDialog::cert(const QString & c)
{
    m_txtCert->setPlainText(c);
}

QString AssistantDialog::input() const
{
    return m_txtInput->toPlainText();
}
void AssistantDialog::input(const QString & i)
{
    m_txtInput->setPlainText(i);
}

QString AssistantDialog::output() const
{
    return m_txtOutput->toPlainText();
}
void AssistantDialog::output(const QString & o)
{
    m_txtOutput->setPlainText(o);
}

void AssistantDialog::on_btnEncrypt_clicked()
{
    clearOutput();
    std::vector<std::string> vecFriends;
    do {
        QString qsData = m_cbbFriends->currentData().toString();
        QChar cPrefix = qsData.at(0);
        if (cPrefix == '*'){       // ---- All
            m_bitmail->GetFriends(vecFriends);
        }else if (cPrefix == '#'){ // ---- Group
            QString sGroupId = qsData.mid(1);
            m_bitmail->GetGroupMembers(sGroupId.toStdString(), vecFriends);
        }else{                     // ---- Peer
            vecFriends.push_back(qsData.toStdString());
        }
    }while(0);
    std::string msg = input().toStdString();
    output(QString::fromStdString(m_bitmail->EncMsg(vecFriends, msg, false)));
}

void AssistantDialog::on_btnDecrypt_clicked()
{
    std::string smime = input().toStdString();
    std::string sEmail, sNick, sMsg, sCertId, sCert;
    m_bitmail->DecMsg(smime, sEmail, sNick, sMsg, sCertId, sCert);
    nick(QString::fromStdString(sNick));
    email(QString::fromStdString(sEmail));
    output(QString::fromStdString(sMsg));
    certid(QString::fromStdString(sCertId));
    cert(QString::fromStdString(sCert));
}

void AssistantDialog::on_btnAddFriend_clicked()
{
    QString qsFrom = email();
    QString qsCert = cert();
    QString qsCertID = certid();

    if (m_bitmail->HasFriend(qsFrom.toStdString())){
        if (m_bitmail->IsFriend(qsFrom.toStdString(), qsCert.toStdString())){
            QMessageBox::information(this
                                     , tr("Friend")
                                     , tr("You are already friends.")
                                     , QMessageBox::Ok);
            return ;
        }
        QString qsPrevCertID = QString::fromStdString(m_bitmail->GetFriendID(qsFrom.toStdString()));
        int ret = QMessageBox::question(this
                                        , tr("Add Friend")
                                        , tr("Replace with this certificate?\r\n")
                                        + tr("Old: [") + qsPrevCertID + QString("]\r\n")
                                        + tr("New: [") + qsCertID + QString("]\r\n")
                                        , QMessageBox::Yes | QMessageBox::No);
        if (ret != QMessageBox::Yes){
            return ;
        }
    }

    if (bmOk != m_bitmail->AddFriend(qsFrom.toStdString(), qsCert.toStdString())){
        QMessageBox::warning(this, tr("Add Friend"), tr("Failed to add friend"), QMessageBox::Ok);
        return ;
    }

    m_cbbFriends->addItem(QIcon(":/images/head.png")
                          , nick()
                          , QVariant(qsFrom));

    emit addFriend(email());
}

void AssistantDialog::on_btnClearInput_clicked()
{
    m_txtInput->clear();
}

void AssistantDialog::on_txtInput_textChanged()
{
    clearOutput();
    const QString MIME_TAG = "MIME-Version: 1.0";
    QString qsInput = input();
    if (qsInput.indexOf(MIME_TAG) == 0){
        m_btnSign->setEnabled(false);
        m_cbbFriends->setEnabled(false);
        m_btnEncrypt->setEnabled(false);
        m_btnDecrypt->setEnabled(true);
        on_btnDecrypt_clicked();
    }else{
        m_btnSign->setEnabled(true);
        m_cbbFriends->setEnabled(true);
        m_btnEncrypt->setEnabled(true);
        m_btnDecrypt->setEnabled(false);
        // Default is Encrypting.
        // Not Signing
        on_btnEncrypt_clicked();
    }
}

void AssistantDialog::on_btnCopy_clicked()
{
    m_clip->setText(m_txtOutput->toPlainText());
}

void AssistantDialog::onClipDataChanged()
{
   QString text = m_clip->text();
   if (!text.isEmpty()){
      input(text);
   }
   return ;
}

void AssistantDialog::on_btnSign_clicked()
{
    clearOutput();
    std::vector<std::string> vecFriends;
    std::string msg = input().toStdString();
    output(QString::fromStdString(m_bitmail->EncMsg(vecFriends, msg, true)));
}

void AssistantDialog::clearOutput()
{
    m_txtCert->clear();
    m_leNick->clear();
    m_leEmail->clear();
    m_leCertId->clear();
    m_txtOutput->clear();
}
