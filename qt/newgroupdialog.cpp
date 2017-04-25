#include "newgroupdialog.h"
#include "main.h"
#include "ui_newgroupdialog.h"

#include <QListView>
#include <QComboBox>
#include <QLineEdit>

#include <bitmailcore/bitmail.h>

NewGroupDialog::NewGroupDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGroupDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(BMQTApplication::GetImageResHome() + "/bitmail.png"));

    m_leGroupId = findChild<QLineEdit*>("leGroupId");
    m_leGroupId->setReadOnly(true);

    m_leGroupName = findChild<QLineEdit*>("leGroupName");
    m_leGroupName->setFocus();

    m_listAddedMembers = findChild<QListWidget*>("listAddedMembers");

    m_cbbMembers = findChild<QComboBox*>("cbbMembers");

    std::vector<std::string> vecFriends;
    m_bitmail->GetFriends(vecFriends);
    for (std::vector<std::string>::const_iterator it = vecFriends.begin()
         ; it != vecFriends.end()
         ; ++it)
    {
        m_cbbMembers->addItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"), QString::fromStdString(*it));
    }
}

NewGroupDialog::~NewGroupDialog()
{
    delete ui;
}

void NewGroupDialog::on_btnAdd_clicked()
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


void NewGroupDialog::on_listAddedMembers_itemDoubleClicked(QListWidgetItem *item)
{
    m_listAddedMembers->takeItem(m_listAddedMembers->row(item));
}

QString NewGroupDialog::groupId() const
{
    return m_leGroupId->text();
}

QString NewGroupDialog::groupName()const
{
    return m_leGroupName->text();
}

void NewGroupDialog::groupName(const QString & gn)
{
    m_leGroupName->setText(gn);
}

QStringList NewGroupDialog::groupMembers() const
{
    QStringList qsl;
    for (int i = 0; i < m_listAddedMembers->count(); i++){
        QListWidgetItem * elt = m_listAddedMembers->item(i);
        qsl.append(elt->text());
    }
    return qsl;
}

void NewGroupDialog::groupMembers(const QStringList & members)
{
    std::string sessKey = BitMail::toSessionKey(BMQTApplication::toStdStringList(members));

    m_leGroupId->setText( QString::fromStdString( sessKey ));

    groupName(QString::fromStdString(m_bitmail->sessionName(sessKey)));

    for (QStringList::const_iterator it = members.begin(); it != members.end(); it++)
    {
        if (it->isEmpty()) continue;
        m_listAddedMembers->addItem(new QListWidgetItem(QIcon(BMQTApplication::GetImageResHome() + "/head.png"), *it));
    }
}

void NewGroupDialog::on_btnSetGroupName_clicked()
{
    if (!groupName().isEmpty() && !groupId().isEmpty()){
        m_bitmail->sessionName(groupId().toStdString(), groupName().toStdString());
    }
    emit groupChanged();
}