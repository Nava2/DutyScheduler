#include "myqlabel.h"
#include <QMouseEvent>

MyQLabel::MyQLabel(QWidget *parent) : QLabel(parent)
{
}


void MyQLabel::mouseReleaseEvent ( QMouseEvent * event )
{
    emit clicked();
}
