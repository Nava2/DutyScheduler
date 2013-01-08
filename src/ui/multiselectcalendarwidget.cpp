#include "multiselectcalendarwidget.h"

MultiSelectCalendarFilter::MultiSelectCalendarFilter(QWidget * const target, QObject *parent) :
    QObject(parent), _target(target)
{
}

bool MultiSelectCalendarFilter::eventFilter(QObject *obj, QEvent *event) {

}

MultiSelectCalendarWidget::MultiSelectCalendarWidget(QCalendarWidget * const cal, QObject *parent) :
    QObject(parent), _cal(cal)
{
}

MultiSelectCalendarWidget::~MultiSelectCalendarWidget() {

}

QCalendarWidget *MultiSelectCalendarWidget::calendar() {
    return _cal;
}
