#ifndef WALLETDIALOG_H
#define WALLETDIALOG_H

#include <QDialog>

class QPushButton;
class QLineEdit;
class QComboBox;
class QListWidget;

namespace Ui {
class walletDialog;
}

class walletDialog : public QDialog
{
    Q_OBJECT

public:
    explicit walletDialog(QWidget *parent = 0);
    ~walletDialog();

private:
    Ui::walletDialog *ui;
    QComboBox * m_cbBank;
    QLineEdit * m_leAccount;
    QLineEdit * m_leToken;
    QPushButton * m_btnQuery;
    QLineEdit * m_leBalance;
    QPushButton * m_btnCharge;

    QComboBox * m_cbBlockChain;
    QLineEdit * m_lePassphrase;
    QPushButton * m_btnBlkChainQuery;
    QLineEdit * m_leBlkChainBalance;
    QPushButton * m_btnImport;

    QListWidget * m_lstDetails;
};

#endif // WALLETDIALOG_H
