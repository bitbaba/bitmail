#include "rssdialog.h"
#include "ui_rssdialog.h"
#include "main.h"
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

    std::vector<std::string> contacts = m_bitmail->contacts();

    for (std::vector<std::string>::const_iterator it = contacts.begin() ; it != contacts.end() ; ++it){
        std::vector<std::string> receips = BitMail::decodeReceips(*it);
        std::string nick = m_bitmail->contattrib(*it, "comment");
        if (nick.empty()){
            std::string certpem = m_bitmail->contattrib(*it, "cert");
            nick = BitMail::certCN(certpem);
        }
        m_cbbGroup->addItem((receips.size() > 1) ? QIcon(BMQTApplication::GetImageResHome() + "/group.png") : QIcon(BMQTApplication::GetImageResHome() + "/head.png")
                            , QString::fromStdString(nick)
                            , QVariant(QString::fromStdString(*it)));
    }
}

RssDialog::~RssDialog()
{
    delete ui;
}

bool RssDialog::isPublic() const
{
    return m_raPublic->isChecked();
}

bool RssDialog::isPrivate() const
{
    return m_raPrivate->isChecked();
}

bool RssDialog::isGroup()const
{
    return m_raGroup->isChecked();
}

QString RssDialog::groupId() const
{
    return m_cbbGroup->currentData().toString();
}

QString RssDialog::content() const
{
    return m_txtContent->toPlainText();
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
