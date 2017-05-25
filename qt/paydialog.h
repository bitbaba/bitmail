#ifndef PAYDIALOG_H
#define PAYDIALOG_H

#include <QDialog>

namespace Ui {
class PayDialog;
}

class PayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PayDialog(const QString & recip, QWidget *parent = 0);
    ~PayDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PayDialog *ui;

    QString m_recip;
};

#endif // PAYDIALOG_H
