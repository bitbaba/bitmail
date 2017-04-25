#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent=0);
    ~ClickableLabel();
protected:
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent(QMouseEvent *);

signals:
    void clicked(ClickableLabel* label);
    void dbclicked(ClickableLabel * label);
};

#endif // CLICKABLELABEL_H
