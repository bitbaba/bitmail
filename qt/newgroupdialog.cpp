#include "newgroupdialog.h"
#include "ui_newgroupdialog.h"

NewGroupDialog::NewGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGroupDialog)
{
    ui->setupUi(this);
}

NewGroupDialog::~NewGroupDialog()
{
    delete ui;
}
