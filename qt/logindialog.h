#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class QComboBox;
class QLineEdit;
class QCheckBox;

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
    void on_cbAssistant_clicked(bool checked);

    void on_btnEnter_clicked();

public:
    QString GetEmail() const;
    void SetEmail(const QString & email);

    QString GetPassphrase() const;
    void SetPassphrase(const QString & passphrase);

    bool imAssistant() const;
private:
    Ui::LoginDialog *ui;
private:
    QComboBox * m_cbEmail;
    QLineEdit * m_lePassphrase;
    QCheckBox * m_cbAssistant;
};

#endif // LOGINDIALOG_H
