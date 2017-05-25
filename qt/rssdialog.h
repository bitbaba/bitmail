#ifndef RSSDIALOG_H
#define RSSDIALOG_H

#include <QDialog>


class QTextEdit;
class QComboBox;
class BitMail;
class QRadioButton;

namespace Ui {
class RssDialog;
}

class RssDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RssDialog(BitMail * bm, QWidget *parent = 0);
    ~RssDialog();

    bool isPublic() const;
    bool isPrivate() const;
    bool isGroup()const;

    QString groupId() const;

    QString content() const;

private slots:
    void on_raGroup_toggled(bool checked);

    void on_raPrivate_toggled(bool checked);

    void on_raPublic_toggled(bool checked);

private:
    Ui::RssDialog *ui;
    BitMail   * m_bitmail;
    QTextEdit * m_txtContent;
    QComboBox * m_cbbGroup;
    QRadioButton * m_raPublic;
    QRadioButton * m_raPrivate;
    QRadioButton * m_raGroup;
};

#endif // RSSDIALOG_H
