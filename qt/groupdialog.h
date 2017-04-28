#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>

class BitMail;

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QComboBox;
class QPushButton;

namespace Ui {
class groupDialog;
}

class groupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit groupDialog(BitMail * bm, QWidget *parent = 0);
    ~groupDialog();

    QString groupId() const;
    void groupId(const QString & gid);

    QString groupName()const;
    void groupName(const QString & gn);

    QStringList groupMembers() const;
    void groupMembers(const QStringList & members);

    void head(const QIcon & hd);

private slots:
    void on_btnAdd_clicked();

    void on_listAddedMembers_itemDoubleClicked(QListWidgetItem *item);

    void on_btnSetGroupName_clicked();

    void on_btnLogo_clicked();

    void on_btnRemove_clicked();

private:
    Ui::groupDialog *ui;
    BitMail * m_bitmail;

    QLineEdit  * m_leCreator;
    QLineEdit  * m_leGroupId;
    QLineEdit  * m_leGroupName;
    QComboBox  * m_cbbMembers;
    QListWidget* m_listAddedMembers;
    QPushButton* m_btnLogo;

signals:
    void groupChanged();
};

#endif // GROUPDIALOG_H
