#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class QComboBox;
class QLineEdit;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    enum {
        CreateNew = 2,
    };
public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_cmdCreate_clicked();

    void on_cbEmail_currentIndexChanged(const QString &arg1);
public:
    QString GetEmail() const;
    void SetEmail(const QString & email);

    QString GetPassphrase() const;
    void SetPassphrase(const QString & passphrase);
private:
    Ui::LoginDialog *ui;
private:
    QComboBox * m_cbEmail;
    QLineEdit * m_lePassphrase;
};

#endif // LOGINDIALOG_H
