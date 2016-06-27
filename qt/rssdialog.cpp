#include "rssdialog.h"
#include "ui_rssdialog.h"

RssDialog::RssDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RssDialog)
{
    ui->setupUi(this);
}

RssDialog::~RssDialog()
{
    delete ui;
}
