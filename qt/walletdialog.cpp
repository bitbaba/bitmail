#include "walletdialog.h"
#include "ui_walletdialog.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>

walletDialog::walletDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::walletDialog)
{
    ui->setupUi(this);

    m_cbBank = findChild<QComboBox*>("cbbBank");
    m_cbBank->addItem(QIcon(":images/bitcoin.png"), QString("bank.bitbaba.com"));
    m_cbBank->setIconSize(QSize(16,16));
    m_leAccount = findChild<QLineEdit*>("leAccount");
    m_leToken = findChild<QLineEdit*>("leToken");
    m_btnQuery = findChild<QPushButton*>("btnQuery");
    m_leBalance = findChild<QLineEdit*>("leBalance");
    m_btnCharge = findChild<QPushButton*>("btnCharge");

    m_cbBlockChain = findChild<QComboBox*>("cbbBlockChain");
    m_lePassphrase = findChild<QLineEdit*>("lePassphrase");
    m_btnBlkChainQuery = findChild<QPushButton*>("btnBlkChainQuery");
    m_leBlkChainBalance = findChild<QLineEdit*>("leBlkChainBalance");
    m_btnImport = findChild<QPushButton*>("btnImport");

    m_lstDetails = findChild<QListWidget*>("lstDetails");
}

walletDialog::~walletDialog()
{
    delete ui;
}
