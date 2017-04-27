#include "certdialog.h"
#include "ui_certdialog.h"
#include "main.h"

#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QDir>
#include <QFileDialog>
#include <bitmailcore/bitmail.h>


CertDialog::CertDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CertDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);

    m_leEmail  = findChild<QLineEdit*>  ("leEmail"  );
    m_leNick   = findChild<QLineEdit*>  ("leNick"   );
    m_leCertID = findChild<QLineEdit*>  ("leCertID" );
    m_lbQrCode = findChild<QLabel*>     ("lbQrCode" );
    m_leComment= findChild<QLineEdit*>  ("leComment");
    m_btnLogo  = findChild<QPushButton*>("btnLogo"  );

    m_leEmail->setReadOnly(true);
    m_leNick->setReadOnly(true);
    m_leCertID->setReadOnly(true);
    m_btnLogo->setToolTip(tr("click to choose a logo"));
    m_btnLogo->setText("");
    m_btnLogo->setIcon(QIcon(BMQTApplication::GetImageResHome() + "/head.png"));

}

CertDialog::~CertDialog()
{
    delete ui;
}

void CertDialog::on_buttonBox_accepted()
{

}

void CertDialog::on_buttonBox_rejected()
{

}

void CertDialog::SetEmail(const QString & e)
{
    m_leEmail->setText(e);
}

QString CertDialog::GetEmail() const
{
    return m_leEmail->text();
}

void CertDialog::SetNick(const QString & n)
{
    m_leNick->setText(n);
}

QString CertDialog::GetNick() const
{
    return m_leNick->text();
}

void CertDialog::CertDialog::SetCertID(const QString & cid)
{
    m_leCertID->setText(cid);
}

QString CertDialog::CertDialog::GetCertID() const
{
    return m_leCertID->text();
}

void CertDialog::comment(const QString &cmt)
{
    m_leComment->setText(cmt);
}

QString CertDialog::comment() const
{
    return m_leComment->text();
}

void CertDialog::qrImage(const QPixmap &p)
{
    m_lbQrCode->setScaledContents(true);
    m_lbQrCode->setPixmap(p);
}

void CertDialog::on_btnSetComment_clicked()
{
    if (!GetEmail().isEmpty() && !comment().isEmpty()){
        std::string sessKey = BitMail::serializeReceips(GetEmail().toStdString());
        m_bitmail->attrib(sessKey, "comment", comment().toStdString());
    }
    emit friendChanged();
}

void CertDialog::on_btnLogo_clicked()
{
    QString logoFile = QFileDialog::getOpenFileName(this, tr("choose a logo"), QDir::homePath(), "Logo (*.png *.jpg *.bmp)");
    if (logoFile.isEmpty()){
        return ;
    }
    QIcon head(logoFile);

    QByteArray b64logo = BMQTApplication::StringifyIcon(head);

    if (b64logo.isEmpty()){
        return ;
    }

    if (GetEmail().isEmpty()){
        return ;
    }

    std::string sessKey = BitMail::serializeReceips(GetEmail().toStdString());

    m_bitmail->attrib(sessKey, "head", b64logo.toStdString());

    m_btnLogo->setIcon(head);

    emit friendChanged();
}
