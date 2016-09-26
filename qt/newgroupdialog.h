#ifndef NEWGROUPDIALOG_H
#define NEWGROUPDIALOG_H

#include <QDialog>

namespace Ui {
class NewGroupDialog;
}

class NewGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGroupDialog(QWidget *parent = 0);
    ~NewGroupDialog();

private:
    Ui::NewGroupDialog *ui;
};

#endif // NEWGROUPDIALOG_H
