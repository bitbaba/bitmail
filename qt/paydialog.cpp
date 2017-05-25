#include "paydialog.h"
#include "ui_paydialog.h"
#include <QGroupBox>

PayDialog::PayDialog(const QString & recip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PayDialog)
    , m_recip(recip)
{
    ui->setupUi(this);

    QGroupBox * grpTo = findChild<QGroupBox*>("grpTo");
    if (grpTo){
        grpTo->setTitle(QString(tr("To")) + QString("(%1)").arg(m_recip));
    }

    setWindowIcon(QIcon(":/images/bitmail.png"));
}

PayDialog::~PayDialog()
{
    delete ui;
}

void PayDialog::on_buttonBox_accepted()
{

}

void PayDialog::on_buttonBox_rejected()
{

}
