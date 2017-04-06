#ifndef INVITEDIALOG_H
#define INVITEDIALOG_H

#include <QDialog>

class QLineEdit;

namespace Ui {
class InviteDialog;
}

class InviteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InviteDialog(QWidget *parent = 0);
    ~InviteDialog();

    QString GetEmail() const;

    QString GetWhisper() const;

    QStringList attachments() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_btnChoose_clicked();

private:
    Ui::InviteDialog *ui;

    QLineEdit * m_leEmail;
    QLineEdit * m_leWhisper;
    QLineEdit * m_leAttachment;

};

#endif // INVITEDIALOG_H
