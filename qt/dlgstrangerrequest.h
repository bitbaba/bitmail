#ifndef DLGSTRANGERREQUEST_H
#define DLGSTRANGERREQUEST_H

#include <QDialog>
#include <string>

namespace Ui {
class DlgStrangerRequest;
}

class DlgStrangerRequest : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStrangerRequest(QWidget *parent = 0);
    ~DlgStrangerRequest();
    std::string GetEmailAddress() const;
    std::string GetCertID() const;
private:
    Ui::DlgStrangerRequest *ui;
};

#endif // DLGSTRANGERREQUEST_H
