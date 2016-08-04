#ifndef ASSISTANTDIALOG_H
#define ASSISTANTDIALOG_H

#include <QDialog>

class QTextEdit;
class QLineEdit;
class QPushButton;
class QCheckBox;

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

private slots:
    void on_btnEncrypt_clicked();

    void on_btnDecrypt_clicked();

    void on_btnAddFriend_clicked();

    void on_btnClearInput_clicked();

    void on_txtInput_textChanged();

private:
    Ui::AssistantDialog *ui;
    BitMail * m_bitmail;

    QLineEdit *m_leNick;
    QLineEdit *m_leEmail;
    QLineEdit *m_leCertId;
    QTextEdit *m_txtInput;
    QTextEdit *m_txtOutput;
    QTextEdit *m_txtCert;

    QPushButton *m_btnAddFriend;
    QCheckBox   *m_cbSingOnly;
    QCheckBox   *m_cbSkipStrangers;
    QPushButton *m_btnEncrypt;
    QPushButton *m_btnDecrypt;
signals:
    void addFriend(const QString & email);
};

#endif // ASSISTANTDIALOG_H
