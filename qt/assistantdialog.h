#ifndef ASSISTANTDIALOG_H
#define ASSISTANTDIALOG_H

#include <QDialog>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;

class BitMail;

namespace Ui {
class AssistantDialog;
}

class AssistantDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AssistantDialog(BitMail * bm, QWidget *parent = 0);
    ~AssistantDialog();

    QString nick() const;
    void nick(const QString & n);

    QString email() const;
    void email(const QString & e);

    QString certid() const;
    void certid(const QString & c);

    QString cert() const;
    void cert(const QString & c);

    QString input() const;
    void input(const QString & i);

    QString output() const;
    void output(const QString & o);

    bool signOnly() const;

    bool skipStrangers() const;

private slots:
    void on_btnEncrypt_clicked();

    void on_btnDecrypt_clicked();

    void on_btnAddFriend_clicked();

    void on_btnClearInput_clicked();

    void on_txtInput_textChanged();

    void on_cbSignOnly_clicked(bool checked);

    void on_cbSkipStrangers_clicked(bool checked);

    void on_cbHideMain_clicked(bool checked);

    void on_AssistantDialog_rejected();

    void on_btnCopy_clicked();

private:
    Ui::AssistantDialog *ui;
    BitMail * m_bitmail;

    QLineEdit *m_leNick;
    QLineEdit *m_leEmail;
    QLineEdit *m_leCertId;
    QTextEdit *m_txtInput;
    QTextEdit *m_txtOutput;
    QTextEdit *m_txtCert;
    QComboBox *m_cbbFriends;

    QPushButton *m_btnAddFriend;
    QCheckBox   *m_cbSignOnly;
    QCheckBox   *m_cbSkipStrangers;
    QPushButton *m_btnEncrypt;
    QPushButton *m_btnDecrypt;
signals:
    void addFriend(const QString & email);
};

#endif // ASSISTANTDIALOG_H
