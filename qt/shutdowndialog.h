#ifndef SHUTDOWNDIALOG_H
#define SHUTDOWNDIALOG_H

#include <QDialog>

class QLabel;

namespace Ui {
class ShutdownDialog;
}

class ShutdownDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShutdownDialog(QWidget *parent = 0);
    ~ShutdownDialog();
    void SetMessage(const QString & msg);
private:
    Ui::ShutdownDialog *ui;

    QLabel *m_lbMessage;
};

#endif // SHUTDOWNDIALOG_H
