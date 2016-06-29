#include "rssdialog.h"
#include "ui_rssdialog.h"

#include <bitmailcore/bitmail.h>

RssDialog::RssDialog(BitMail * bm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RssDialog),
    m_bitmail(bm)
{
    ui->setupUi(this);
    QButtonGroup * btnGrp = new QButtonGroup();

    m_raPublic = findChild<QRadioButton*>("raPublic");
    btnGrp->addButton(m_raPublic, 1);
    m_raPrivate = findChild<QRadioButton*>("raPrivate");
    btnGrp->addButton(m_raPrivate,2);
    m_raGroup = findChild<QRadioButton*>("raGroup");
    btnGrp->addButton(m_raGroup,  3);

    m_raPublic->setChecked(true);
    m_raPrivate->setChecked(false);
    m_raGroup->setChecked(false);

    m_txtContent = findChild<QTextEdit*>("txtContent");
    m_cbbGroup   = findChild<QComboBox*>("cbbGroup");
    m_cbbGroup->setEnabled(false);

    std::vector<std::string> vecGroups;
    if (m_bitmail){
        m_bitmail->GetGroups(vecGroups);
    }
    for (std::vector<std::string>::const_iterator it = vecGroups.begin()
         ; it != vecGroups.end()
         ; ++it){
        std::vector<std::string> vecMembers;
        m_bitmail->GetGroupMembers(*it, vecMembers);
        QStringList qslMembers;
        for (std::vector<std::string>::const_iterator it_member = vecMembers.begin()
             ; it_member != vecMembers.end()
             ; ++it_member){
            qslMembers.append(QString::fromStdString(*it_member));
        }
        m_cbbGroup->addItem(QIcon(":/images/group.png"), QString::fromStdString(*it), QVariant(qslMembers));
    }
}

RssDialog::~RssDialog()
{
    delete ui;
}

void RssDialog::on_raGroup_toggled(bool checked)
{
    m_cbbGroup->setEnabled(checked);
}

void RssDialog::on_raPrivate_toggled(bool checked)
{
    (void)checked;
}

void RssDialog::on_raPublic_toggled(bool checked)
{
    (void)checked;
}
