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

private:
    Ui::InviteDialog *ui;

    QLineEdit * m_leEmail;
    QLineEdit * m_leWhisper;
};

#endif // INVITEDIALOG_H
