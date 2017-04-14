#ifndef NEWGROUPDIALOG_H
#define NEWGROUPDIALOG_H

#include <QDialog>

class BitMail;

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QComboBox;

namespace Ui {
class NewGroupDialog;
}

class NewGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGroupDialog(BitMail * bm, QWidget *parent = 0);
    ~NewGroupDialog();

    QString groupId() const;
    void groupId(const QString & gid);

    QString groupName()const;
    void groupName(const QString & gn);

    QStringList groupMembers() const;
    void groupMembers(const QStringList & members);


private slots:
    void on_btnAdd_clicked();

    void on_listAddedMembers_itemDoubleClicked(QListWidgetItem *item);

    void on_btnSetGroupName_clicked();

private:
    Ui::NewGroupDialog *ui;
    BitMail * m_bitmail;

    QLineEdit  * m_leCreator;
    QLineEdit  * m_leGroupId;
    QLineEdit  * m_leGroupName;
    QComboBox  * m_cbbMembers;
    QListWidget* m_listAddedMembers;

signals:
    void groupChanged();
};

#endif // NEWGROUPDIALOG_H
