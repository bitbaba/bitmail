#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class VersionDialog;
}

class VersionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VersionDialog(QWidget *parent = 0);
    ~VersionDialog();

private:
    Ui::VersionDialog *ui;
    QLabel * m_lblCoreVersion;
    QLabel * m_lblQtVersion;
};

#endif // VERSIONDIALOG_H
