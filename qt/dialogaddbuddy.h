#ifndef DIALOGADDBUDDY_H
#define DIALOGADDBUDDY_H

#include <QDialog>
#include <QTextEdit>

namespace Ui {
class DialogAddBuddy;
}

class DialogAddBuddy : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddBuddy(QWidget *parent = 0);
    ~DialogAddBuddy();

private:
    Ui::DialogAddBuddy *ui;

private:
    void UpdateGUI(const std::string & sCertPem);

public:
    std::string GetCertPem() const;

    void SetCertPem(const std::string & sCertPem);

private slots:
    void on_textEdit_textChanged();

private:
    QTextEdit * m_edit;
};

#endif // DIALOGADDBUDDY_H
