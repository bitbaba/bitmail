#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{

}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *)
{
    emit clicked(this);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *)
{
    emit dbclicked(this);
}
