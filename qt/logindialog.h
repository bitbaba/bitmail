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
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_cmdCreate_clicked();

    void on_btnEnter_clicked();

public:
    QString GetEmail() const;

    QString GetPassphrase() const;

private:
    void reloadProfiles();

    bool createProfile(int nBits, const QString & qsEmail, const QString & qsNick, const QString & qsPassphrase
                       , const QString & txUrl, const QString & rxUrl, const QString & login, const QString & pass, const QString & proxy);

private:
    Ui::LoginDialog *ui;
private:
    QComboBox * m_cbEmail;
    QLineEdit * m_lePassphrase;
};

#endif // LOGINDIALOG_H
