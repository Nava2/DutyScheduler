#ifndef MULTISELECTCALENDARWIDGET_H
#define MULTISELECTCALENDARWIDGET_H

#include <QObject>
#include <QEvent>
#include <QCalendarWidget>

class MultiSelectCalendarFilter : public QObject {
    Q_OBJECT

public:
    explicit MultiSelectCalendarFilter(QWidget * const target, QObject *parent = nullptr);


signals:
    void selectCoord(QPoint pt);

protected:
    enum STATE {
        INITIAL = 0,
        MOUSE_DOWN,
        MOUSE_MOVED
    };

    bool eventFilter(QObject *obj, QEvent *event);

private:
    QCalendarWidget *_target;
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
    void onSelectCoord(const QPoint &pt);
    

private:
    QCalendarWidget *_cal;
};

#endif // MULTISELECTCALENDARWIDGET_H
