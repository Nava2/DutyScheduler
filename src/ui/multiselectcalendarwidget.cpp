#include "multiselectcalendarwidget.h"

#include <QMouseEvent>
#include <QTableView>
#include <QCalendarWidget>
#include <QVariant>
#include <QDebug>
#include <QTextCharFormat>

#include <algorithm>

MultiSelectCalendarFilter::MultiSelectCalendarFilter(QCalendarWidget * const target, QObject *parent) :
    QObject(parent), _target(target)
{
    _tbl = qFindChild<QTableView *>(_target);
}

bool MultiSelectCalendarFilter::eventFilter(QObject *obj, QEvent *event) {
    if (obj != _tbl->viewport())
        return false;
//    qDebug() << "Event: " << event;
    if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mev = dynamic_cast<QMouseEvent*>(event);

        if (!mev) {
            // mev failed the dynamic cast
            return false;
        }

        QModelIndex idx = _tbl->indexAt(mev->pos());

        if (idx.row() == 0) {
            return false;
        }

        int first, last;
        getFirstLastIndex(first, last);

        int iidx = idx.row() * _tbl->model()->columnCount() + idx.column();
        qDebug() << "iidx:" << iidx << "f:" << first << "l:" << last;


        if (mev->type() == QEvent::MouseButtonPress) {
            // starting
            if (iidx < first || iidx > last) {
                // the index is outside the selectable area.
                return false;
            }

            if (idx.row() != 0) {
                startIdx = idx;
                _state = STARTED;
            }

            return true;
        } else if (mev->type() == QEvent::MouseButtonRelease) {
            if (_state == STARTED && idx.row() != 0) {
                if (iidx < first) {
                    qDebug() << "Selected past beginning, using first.";
                    endIdx = _tbl->model()->index(first / _tbl->model()->columnCount(), first % _tbl->model()->columnCount());
                } else if (iidx > last) {
                    qDebug() << "Selected past last, using last.";
                    endIdx = _tbl->model()->index(last / _tbl->model()->columnCount(), last % _tbl->model()->columnCount());
                } else {
                    // we have selected an initial and its valid
                    endIdx = idx;
                }
            } else {
                // bad selection
                _state = INITIAL;

                return false;
            }

            int rowStart = startIdx.row();
            int rowEnd = endIdx.row();

            int colStart = startIdx.column();
            int colEnd = endIdx.column();

            if (rowStart > rowEnd) {
                std::swap(rowStart, rowEnd);
            }

            if (rowStart == rowEnd && colStart > colEnd) {
                std::swap(colStart, colEnd);
            }

            for (int row = rowStart; row <= rowEnd; row++) {
                int col = 0, end = _tbl->model()->columnCount()-1;
                if (row == rowStart && row != rowEnd) {
                    col = colStart;
                } else if (row == rowEnd) {
                    col = (row == rowStart) ? colStart : 0;
                    end = colEnd;
                } else {
                    col = 0;
                }

                for ( ; col <= end; col++)
                    emit selectCoord(row, col);
            }

            return true;
        }
    }

    return false;
}

void MultiSelectCalendarFilter::getFirstLastIndex(int &first, int &last) {
    first = -1;
    last = -1;

    QAbstractItemModel *mdl = _tbl->model();

    int iidx = mdl->columnCount();
    while (first == -1 || last == -1) {
        ++iidx;
        int y = iidx % mdl->columnCount();
        int x = iidx / mdl->columnCount();
        QVariant data = mdl->data(mdl->index(x, y), Qt::DisplayRole);
//        qDebug() << "x:" << x << "y:" << y << "v:" << data.toInt();
        bool valid = false;
        int t = data.toInt(&valid);
        if (valid) {
            if (t == 1 && first == -1) {
                first = iidx;
            } else if (t == 1 && first != -1) {
                last = iidx - 1;
            } else if (x == mdl->rowCount() - 1 && y == mdl->columnCount() - 1) {
                last = iidx;
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

void MultiSelectCalendarWidget::onSelectCoord(const int row, const int col) {
    QTableView *tbl = qFindChild<QTableView *>(_cal);
    QAbstractItemModel *mdl = tbl->model();

    QModelIndex idx = mdl->index(row, col);
//    qDebug() << "Clicked Date (" << row << ", " << col << "):" << clkInx << "1st index:" << rowColIdx;

    QDate date(_cal->yearShown(), _cal->monthShown(), mdl->data(idx).toInt());
    qDebug() << "Toggling: " << date;

    if (idx.row() != 0) {
        // not the headers
        QTextCharFormat fmt = _cal->dateTextFormat(date);
        bool selected = false;
        if (fmt.background().color() == _selectedColour) {
            // was selected:
            fmt = _cal->weekdayTextFormat(static_cast<Qt::DayOfWeek>(date.dayOfWeek()));
            selected = false;
        } else {
            // wasn't selected
            fmt.setBackground(_selectedColour);
            selected = true;
        }

        _cal->setDateTextFormat(date, fmt);

        emit ToggleSelected(date, selected);
    }
}

QColor MultiSelectCalendarWidget::selectedColour() const {
    return _selectedColour;
}

void MultiSelectCalendarWidget::selectedColour(const QColor &newColour) {
    _selectedColour = newColour;
}
