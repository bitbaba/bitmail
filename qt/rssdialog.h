#ifndef RSSDIALOG_H
#define RSSDIALOG_H

#include <QDialog>

namespace Ui {
class RssDialog;
}

class RssDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RssDialog(QWidget *parent = 0);
    ~RssDialog();

private:
    Ui::RssDialog *ui;
};

#endif // RSSDIALOG_H
