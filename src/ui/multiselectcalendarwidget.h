#ifndef MULTISELECTCALENDARWIDGET_H
#define MULTISELECTCALENDARWIDGET_H

#include <QObject>
#include <QEvent>
#include <QCalendarWidget>
#include <QTableView>

class MultiSelectCalendarFilter : public QObject {
    Q_OBJECT

public:
    explicit MultiSelectCalendarFilter(QWidget * const target, QObject *parent = nullptr);


signals:
    void selectCoord(const int row, const int col);

protected:
    enum STATE {
        INITIAL = 0,
        STARTED
    };

    bool eventFilter(QObject *obj, QEvent *event);

private:
    QCalendarWidget *_target;
    QTableView *_tbl;

    QModelIndex startIdx, endIdx;

    STATE _state;
};

class MultiSelectCalendarWidget : public QObject
{
    Q_OBJECT
public:
    explicit MultiSelectCalendarWidget(QCalendarWidget * const cal, QObject *parent = 0);

    virtual
    ~MultiSelectCalendarWidget();
    
    QCalendarWidget *calendar();

signals:
    
public slots:
    void onSelectCoord(const int row, const int col);
    

private:
    QCalendarWidget *_cal;
};

#endif // MULTISELECTCALENDARWIDGET_H
