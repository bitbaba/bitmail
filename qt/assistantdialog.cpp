#include "assistantdialog.h"
#include "ui_assistantdialog.h"
#include "bitmailcore/bitmail.h"
#include <QDebug>
#include <QMessageBox>
#include <QClipboard>
#include "main.h"

AssistantDialog::AssistantDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AssistantDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);

#if defined(MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
#endif
    setWindowIcon(QIcon(BMQTApplication::GetImageResHome() + "/bitmail.png"));

    m_txtInput     = findChild<QTextEdit*>  ("txtInput"    );
    m_btnEncrypt   = findChild<QPushButton*>("btnEncrypt"  );
    m_btnDecrypt   = findChild<QPushButton*>("btnDecrypt"  );
    m_btnSign      = findChild<QPushButton*>("btnSign"     );
    m_btnAddFriend = findChild<QPushButton*>("btnAddFriend");
    m_cbbFriends   = findChild<QComboBox*>  ("cbbFriends"  );
    m_leNick       = findChild<QLineEdit*>  ("leNick"      );
    m_leEmail      = findChild<QLineEdit*>  ("leEmail"     );
    m_leCertId     = findChild<QLineEdit*>  ("leCertId"    );
    m_txtCert      = findChild<QTextEdit*>  ("txtCert"     );
    m_txtOutput    = findChild<QTextEdit*>  ("txtOutput"   );

    std::vector<std::string> friends = m_bitmail->contacts();
    for (std::vector<std::string>::const_iterator it = friends.begin(); it != friends.end(); ++it)
    {
        std::string email = (*it);
        std::string sessKey = BitMail::serializeReceips(email);
        m_cbbFriends->addItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png")
                              , QString::fromStdString(m_bitmail->contattrib(sessKey, "comment"))
                              , QVariant(QString::fromStdString(sessKey)));
    }

    std::vector<std::string> groups = m_bitmail->contacts();
    for (unsigned int i = 0; i < groups.size(); ++i)
    {
        std::string group = groups[i];
        std::vector<std::string> vec_receips = BitMail::decodeReceips(group);
        std::string sessKey = BitMail::serializeReceips(vec_receips);
        m_cbbFriends->addItem(QIcon(BMQTApplication::GetImageResHome() + "/group.png")
                              , QString::fromStdString(m_bitmail->contattrib(sessKey, "comment"))
                              , QVariant(QString::fromStdString(sessKey)));
    }

    if (0){// Listen clipboard
        m_clip = QApplication::clipboard();
        connect(m_clip, SIGNAL(dataChanged()), this, SLOT(onClipDataChanged()));
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

void AssistantDialog::on_btnEncrypt_clicked()
{
    clearOutput();
    std::vector<std::string> friends;
    QString sessKey = m_cbbFriends->currentData().toString();
    friends = BitMail::decodeReceips(sessKey.toStdString());
    std::string msg = input().toStdString();
    output(QString::fromStdString(m_bitmail->Encrypt(friends, msg, false)));
}

void AssistantDialog::on_btnDecrypt_clicked()
{
    std::string smime = input().toStdString();
    std::string sEmail, sNick, sMsg, sCertId, sCert, sSigTime;
    bool encrypted = false;
    m_bitmail->Decrypt(smime, sEmail, sNick, sMsg, sCertId, sCert, sSigTime, &encrypted);
    nick(QString::fromStdString(sNick));
    email(QString::fromStdString(sEmail));
    output(QString::fromStdString(sMsg));
    certid(QString::fromStdString(sCertId));
    cert(QString::fromStdString(sCert));
    (void)sSigTime;
}

void AssistantDialog::on_btnAddFriend_clicked()
{
    QString qsFrom = email();
    QString qsCert = cert();

    if (!m_bitmail->contattrib(qsFrom.toStdString(), "cert", qsCert.toStdString())){
        QMessageBox::warning(this, tr("Add Friend"), tr("Failed to add friend"), QMessageBox::Ok);
        return ;
    }

    std::string sessKey = BitMail::serializeReceips(qsFrom.toStdString());

    m_cbbFriends->addItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png")
                          , QString::fromStdString(m_bitmail->contattrib(sessKey, "comment"))
                          , QVariant(QString::fromStdString(sessKey)));
}

void AssistantDialog::on_btnClearInput_clicked()
{
    m_txtInput->clear();
}

void AssistantDialog::on_txtInput_textChanged()
{
    clearOutput();

    QString qsInput = input();

    if (qsInput.contains("Mime-Version:", Qt::CaseInsensitive)
        || qsInput.contains("Content-Type:", Qt::CaseInsensitive))
    {
        on_btnDecrypt_clicked();
    }else{
        m_btnDecrypt->setEnabled(false);
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
    output(QString::fromStdString(m_bitmail->Encrypt(vecFriends, msg, true)));
}

void AssistantDialog::clearOutput()
{
    m_txtCert->clear();
    m_leNick->clear();
    m_leEmail->clear();
    m_leCertId->clear();
    m_txtOutput->clear();
}
