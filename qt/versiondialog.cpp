#include "versiondialog.h"
#include "ui_versiondialog.h"
#include "version.h"
#include <bitmailcore/bitmail.h>

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);

    m_lblCoreVersion = findChild<QLabel*>("lblCoreVersion");
    m_lblQtVersion   = findChild<QLabel*>("lblQtVersion");

    m_lblCoreVersion->setText(QString::fromStdString(BitMail::GetVersion()));
    m_lblQtVersion->setText(QString::fromLatin1(BITMAIL_QT_GIT_VERSION));
}

VersionDialog::~VersionDialog()
{
    delete ui;
}
