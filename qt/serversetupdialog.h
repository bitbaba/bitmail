#ifndef SERVERSETUPDIALOG_H
#define SERVERSETUPDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class ServerSetupDialog;
}

class ServerSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerSetupDialog(QWidget *parent = 0);
    ~ServerSetupDialog();

private slots:
    void on_buttonBox_accepted();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::ServerSetupDialog *ui;
private:
    QLineEdit * m_cTxUrl;
    QLineEdit * m_cTxLogin;
    QLineEdit * m_cTxPass;

    QLineEdit * m_cRxUrl;
    QLineEdit * m_cRxLogin;
    QLineEdit * m_cRxPass;

    std::string m_txUrl;
    std::string m_txLogin;
    std::string m_txPass;

    std::string m_rxUrl;
    std::string m_rxLogin;
    std::string m_rxPass;
    unsigned int m_nPop3Interval; /*Pop3 check interval, in seconds*/
    bool m_fAllowStrangers; /* allow stranger(s) to requst */

    std::string m_sEmailGuess;

    std::string m_sConfig;
public:
    void SetEmailForGuess(const std::string & email);
    void SetDefaultValueFromConfigration(const std::string & jsonParam);
    std::string GetConfigStr(void) const;
};

#endif // SERVERSETUPDIALOG_H
