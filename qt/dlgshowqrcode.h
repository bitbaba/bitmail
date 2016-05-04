#ifndef DLGSHOWQRCODE_H
#define DLGSHOWQRCODE_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class DlgShowQrcode;
}

class DlgShowQrcode : public QDialog
{
    Q_OBJECT

public:
    explicit DlgShowQrcode(QWidget *parent = 0);
    ~DlgShowQrcode();

    void ShowQrImage(const std::string & qrimagepath, const std::string & email, const std::string & certid);

private:
    Ui::DlgShowQrcode *ui;

    QLabel*  m_ctrlLabel;
};

#endif // DLGSHOWQRCODE_H
