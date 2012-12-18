#ifndef TEAMWIDGET_H
#define TEAMWIDGET_H

#include <QWidget>

namespace Ui {
class TeamWidget;
}

class TeamWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TeamWidget(QWidget *parent = 0);
    ~TeamWidget();
    
private:
    Ui::TeamWidget *ui;
};

#endif // TEAMWIDGET_H
