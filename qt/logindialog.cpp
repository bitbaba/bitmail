#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    QLabel * lbLogo = findChild<QLabel *>("lbLogo");
    QPixmap imgLogo = QPixmap(":/images/bkgrd.png");
    QPainter pencil;
    pencil.begin(&imgLogo);
    pencil.setPen(Qt::red);
    pencil.setFont(QFont("Arial", 12));
    pencil.drawText(125, 20, 300, 100, Qt::AlignCenter, tr("BitMail Qt Client"));
    pencil.end();
    lbLogo->setPixmap(imgLogo);

    m_lePassphrase = findChild<QLineEdit*>("lePassphrase");
    m_cbEmail = findChild<QComboBox*>("cbEmail");

    m_cbEmail->addItem(QIcon(":/images/head.png"), "alice@example.net");
    m_cbEmail->addItem(QIcon(":/images/head.png"), "bob@example.net");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_cmdCreate_clicked()
{

}

void LoginDialog::on_cbEmail_currentIndexChanged(const QString &arg1)
{
    (void )arg1;
}
