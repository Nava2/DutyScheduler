#include "myqtablewidgetitem.h"
#include <QMouseEvent>

MyQTableWidgetItem::MyQTableWidgetItem(QObject *parent) : QTableWidgetItem(parent)
{
}

void MyQTableWidgetItem::mouseReleaseEvent ( QMouseEvent * event )
{
    emit clicked();
}
