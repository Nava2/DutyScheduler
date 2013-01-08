#include "multiselectcalendarwidget.h"

#include <QMouseEvent>
#include <QTableView>
#include <QCalendarWidget>

#include <algorithm>

MultiSelectCalendarFilter::MultiSelectCalendarFilter(QWidget * const target, QObject *parent) :
    QObject(parent), _target(target)
{
    _tbl = qFindChild<QTableView *>(_target);
}

bool MultiSelectCalendarFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress ||
            event-type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mev = qobject_cast<QMouseEvent*>(event);

        if (!mev) {
            // mev failed the dynamic cast
            return false;
        }

        QModelIndex idx = _tbl->indexAt(mev->pos());

        if (mev->type() == QEvent::MouseButtonPress) {
            // starting
            if (idx.row() != 0 && idx.column() != 0) {
                startIdx = idx;
                _state = STARTED;
            }
        } else if (mev->type() == QEvent::MouseButtonRelease) {
            if (_state == STARTED &&
                    idx.row() != 0 && idx.column() != 0) {
                // we have selected an initial and its valid
                endIdx = idx;
            } else {
                // bad selection
                _state = INITIAL;

                return false;
            }

            int rowStart = startIdx.row();
            int rowEnd = endIdx.row();

            int colStart = startIdx.column();
            int colEnd = endIdx.column();

            QItemSelection sel;

            if (rowStart > rowEnd) {
                std::swap(rowStart, rowEnd);
            }
            // don't care about columns

            for (int row = rowStart; row <= rowEnd; row++) {
                int col = 0;
                if (row == rowStart && row != rowEnd) {
                    col = colStart;
                } else if (row == rowEnd) {
                    col = (row == rowStart) ? colStart : 1;
                } else {
                    col = 1;
                }

                for ( ; col <_tbl->model()->columnCount(); col++)
                    emit selectCoord(row, col);
            }
        }
    }
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
