#ifndef MYQLISTWIDGET_H
#define MYQLISTWIDGET_H

#include <QListWidget>

class MyQListWidget : public QListWidget
{
Q_OBJECT
public:
    explicit MyQListWidget(QWidget *parent = 0);

signals:
    void rightClickSignal(QPoint p);
    void leftClickSignal(QListWidgetItem *item);

private:
    bool eventFilter(QObject *, QEvent *);

};

#endif // MYQLISTWIDGET_H
