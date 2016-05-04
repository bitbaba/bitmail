#ifndef CREATEPROFILEDIALOG_H
#define CREATEPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class CreateProfileDialog;
}

class CreateProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProfileDialog(QWidget *parent = 0);
    ~CreateProfileDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CreateProfileDialog *ui;
private:
    std::string m_sNick;
    std::string m_sEmail;
    std::string m_sKeyPass;

    std::string m_sCertPem;
    std::string m_sKeyPem;
public:
    std::string GetCertPem() const;
    std::string GetKeyPem() const;
};

#endif // CREATEPROFILEDIALOG_H
