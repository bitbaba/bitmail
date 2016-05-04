#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::LoginDialog *ui;
private:
    std::string m_sEmail;
    std::string m_sKeyPass;
public:
    std::string GetEmail() const;
    std::string GetKeyPass() const;
private:
    void ChangeProfileHome(const std::string & email);

private:
    QComboBox * m_comboEmail;
    QLineEdit * m_ctrlKeyPass;
};

#endif // LOGINDIALOG_H
