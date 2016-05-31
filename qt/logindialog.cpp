#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPainter>
#include "main.h"

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
    done(LoginDialog::CreateNew);
}

void LoginDialog::on_cbEmail_currentIndexChanged(const QString &arg1)
{
    (void )arg1;
}

void LoginDialog::SetEmail(const QString &email)
{
    m_cbEmail->insertItem(QComboBox::InsertAtTop, email);
    m_cbEmail->setCurrentIndex(0);
    return ;
}

QString LoginDialog::GetEmail() const
{
    return m_cbEmail->currentText();
}

void LoginDialog::SetPassphrase(const QString & passphrase)
{
    m_lePassphrase->setText(passphrase);
}

QString LoginDialog::GetPassphrase() const
{
    return m_lePassphrase->text();
}
