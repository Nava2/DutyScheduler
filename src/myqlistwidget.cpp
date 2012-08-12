#include "myqlistwidget.h"
#include <QMouseEvent>
#include <QDebug>
MyQListWidget::MyQListWidget(QWidget *parent) : QListWidget(parent)
{
    this->viewport()->installEventFilter(this);
    //installEventFilter(this);
}

bool MyQListWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == this->viewport())
    {

        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
            if (mouseEvent->button() == Qt::LeftButton && this->itemAt(mouseEvent->pos()) != 0)
                emit leftClickSignal(this->itemAt(mouseEvent->pos()));

            return false;
        }

        if(event->type() == QEvent::ContextMenu)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
            emit rightClickSignal(mouseEvent->pos());
            return false;
        }


    }
    else
        return QListWidget::eventFilter(obj, event);
}

