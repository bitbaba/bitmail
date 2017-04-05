#ifndef ARDIALOG_H
#define ARDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class arDialog;
}

class arDialog : public QDialog
{
    Q_OBJECT

public:
    explicit arDialog(QWidget *parent = 0);
    ~arDialog();
public:
    void ShowStatus(const QString & st);

private:
    Ui::arDialog *ui;
    QLabel * lblStatus;
};

#endif // ARDIALOG_H
