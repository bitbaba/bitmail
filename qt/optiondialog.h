#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QSpinBox;
class QPushButton;

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(bool fNew = false, QWidget *parent = 0);
    ~OptionDialog();

private:
    Ui::OptionDialog *ui;
public:
    void email(const QString & e);
    QString email() const;

    void nick(const QString & n);
    QString nick() const;

    void passphrase(const QString & p);
    QString passphrase() const;

    void bits(const int & n);
    int bits() const;

    void txUrl(const QString & txurl);
    QString txUrl(void) const;

    void rxUrl(const QString & rxurl);
    QString rxUrl(void) const;

    void login(const QString & l);
    QString login(void) const;

    void password(const QString & p);
    QString password(void) const;

    void socks5(const QString & s);
    QString socks5(void) const;

    bool noSig() const;

    bool noEnvelop() const;

    bool noFriend() const;

private slots:
    void on_leEmail_textChanged(const QString &arg1);

    void on_btnExportCert_clicked();

    void on_btnExportKey_clicked();

private:
    QLineEdit *m_leEmail;
    QLineEdit *m_leNick;
    QLineEdit *m_lePassphrase;
    QSpinBox  *m_sbBits;
    QLineEdit *m_leTxUrl;
    QLineEdit *m_leRxUrl;
    QLineEdit *m_leLogin;
    QLineEdit *m_lePassword;
    QLineEdit *m_leSocks5;
    QPushButton *m_btnExCert;
    QPushButton *m_btnExKey;

    QCheckBox *m_cbNoSig;
    QCheckBox *m_cbNoEnvelop;
    QCheckBox *m_cbNoFriend;
private:
    bool       newProfile;
};

#endif // OPTIONDIALOG_H
