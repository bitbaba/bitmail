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
    m_leNick = findChild<QLineEdit*>("leNick");
    m_leEmail = findChild<QLineEdit*>("leEmail");
    m_leCertId = findChild<QLineEdit*>("leCertId");
    m_txtCert = findChild<QTextEdit*>("txtCert");
    m_txtInput = findChild<QTextEdit*>("txtInput");
    m_txtOutput = findChild<QTextEdit*>("txtOutput");
    m_btnEncrypt = findChild<QPushButton*>("btnEncrypt");
    m_btnDecrypt = findChild<QPushButton*>("btnDecrypt");
    m_cbSignOnly = findChild<QCheckBox*>("cbSignOnly");
    m_cbSkipStrangers = findChild<QCheckBox*>("cbSkipStrangers");
    m_btnAddFriend = findChild<QPushButton*>("btnAddFriend");
    m_cbbFriends = findChild<QComboBox*>("cbbFriends");

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

bool AssistantDialog::signOnly() const
{
    return m_cbSignOnly->isChecked();
}

bool AssistantDialog::skipStrangers() const
{
    return m_cbSkipStrangers->isChecked();
}

void AssistantDialog::on_btnEncrypt_clicked()
{
    std::vector<std::string> vecFriends;
    bool fSignOnly = signOnly();
    bool fSkipStranger = skipStrangers();
    if (!fSignOnly){
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
    }
    std::string smime;
    std::string msg = input().toStdString();
    m_bitmail->EncMsg(vecFriends, msg, smime, fSignOnly, fSkipStranger);
    output(QString::fromStdString(smime));
    nick("");
    email("");
    certid("");
    cert("");
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

    emit addFriend(email());
}

void AssistantDialog::on_btnClearInput_clicked()
{
    m_txtInput->clear();
}

void AssistantDialog::on_txtInput_textChanged()
{
    nick(""); email(""); certid(""); cert(""); output("");
}

void AssistantDialog::on_cbSignOnly_clicked(bool checked)
{
    m_cbbFriends->setEnabled(!checked);
    m_cbSkipStrangers->setEnabled(!checked);
}

void AssistantDialog::on_cbSkipStrangers_clicked(bool checked)
{
    (void)checked;
}

void AssistantDialog::on_cbHideMain_clicked(bool checked)
{
    if (checked){
        parentWidget()->hide();
    }else{
        parentWidget()->show();
    }
}

void AssistantDialog::on_AssistantDialog_rejected()
{
    parentWidget()->show();
}

void AssistantDialog::on_btnCopy_clicked()
{
    QClipboard * board = QApplication::clipboard();
    board->setText(m_txtOutput->toPlainText());
}
