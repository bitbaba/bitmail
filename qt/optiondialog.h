#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QSpinBox;

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(bool fNew = false, QWidget *parent = 0);
    ~OptionDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::OptionDialog *ui;
public:
    void SetEmail(const QString & e);
    QString GetEmail() const;

    void SetNick(const QString & n);
    QString GetNick() const;

    void SetPassphrase(const QString & p);
    QString GetPassphrase() const;

    void SetBits(const int & n);
    int GetBits() const;

private:
    QLineEdit *m_leEmail;
    QLineEdit *m_leNick;
    QLineEdit *m_lePassphrase;
    QSpinBox  *m_sbBits;

private:
    bool       m_fNew;
};

#endif // OPTIONDIALOG_H
