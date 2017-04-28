#include "groupdialog.h"
#include "main.h"
#include "ui_groupdialog.h"

#include <QListView>
#include <QComboBox>
#include <QLineEdit>
#include <QFileDialog>

#include <bitmailcore/bitmail.h>

groupDialog::groupDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::groupDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(BMQTApplication::GetImageResHome() + "/bitmail.png"));

    m_leGroupId        = findChild<QLineEdit*>  ("leGroupId"       );
    m_leGroupName      = findChild<QLineEdit*>  ("leGroupName"     );
    m_listAddedMembers = findChild<QListWidget*>("listAddedMembers");
    m_cbbMembers       = findChild<QComboBox*>  ("cbbMembers"      );
    m_btnLogo          = findChild<QPushButton*>("btnLogo"         );

    m_leGroupId->setReadOnly(true);

    m_btnLogo->setText("");

    m_leGroupName->setFocus();

    std::vector<std::string> vecFriends = m_bitmail->contacts();
    for (std::vector<std::string>::const_iterator it = vecFriends.begin()
         ; it != vecFriends.end()
         ; ++it)
    {
        m_cbbMembers->addItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"), QString::fromStdString(*it));
    }
}

groupDialog::~groupDialog()
{
    delete ui;
}

void groupDialog::on_btnAdd_clicked()
{
    QString qsSelected = m_cbbMembers->currentText();
    if (qsSelected.isEmpty()){
        return ;
    }
    if (m_listAddedMembers->findItems(qsSelected, Qt::MatchExactly).size()){
        return ;
    }
    QListWidgetItem * elt = new QListWidgetItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"), qsSelected);
    m_listAddedMembers->addItem(elt);
}


void groupDialog::on_listAddedMembers_itemDoubleClicked(QListWidgetItem *item)
{
    m_listAddedMembers->takeItem(m_listAddedMembers->row(item));
}

QString groupDialog::groupId() const
{
    return m_leGroupId->text();
}

QString groupDialog::groupName()const
{
    return m_leGroupName->text();
}

void groupDialog::groupName(const QString & gn)
{
    m_leGroupName->setText(gn);
}

QStringList groupDialog::groupMembers() const
{
    QStringList qsl;
    for (int i = 0; i < m_listAddedMembers->count(); i++){
        QListWidgetItem * elt = m_listAddedMembers->item(i);
        qsl.append(elt->text());
    }
    return qsl;
}

void groupDialog::groupMembers(const QStringList & members)
{
    std::string sessKey = BitMail::serializeReceips(BMQTApplication::toStdStringList(members));

    m_leGroupId->setText( QString::fromStdString( sessKey ));

    groupName(QString::fromStdString(m_bitmail->contattrib(sessKey, "comment")));

    for (QStringList::const_iterator it = members.begin(); it != members.end(); it++)
    {
        if (it->isEmpty()) continue;
        m_listAddedMembers->addItem(new QListWidgetItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"), *it));
    }
}

void groupDialog::head(const QIcon &hd)
{
    m_btnLogo->setIcon(hd);
}

void groupDialog::on_btnSetGroupName_clicked()
{
    if (!groupName().isEmpty() && !groupId().isEmpty()){
        m_bitmail->contattrib(groupId().toStdString(), "comment", groupName().toStdString());
    }
    emit groupChanged();
}

void groupDialog::on_btnLogo_clicked()
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

    if (groupId().isEmpty()){
        return ;
    }

    m_bitmail->contattrib(groupId().toStdString(), "head", b64logo.toStdString());

    m_btnLogo->setIcon(head);

    emit groupChanged();
}

void groupDialog::on_btnRemove_clicked()
{
    if (!BitMail::getInst()->removeContact(groupId().toStdString())){
        return ;
    }
    emit groupChanged();
}
