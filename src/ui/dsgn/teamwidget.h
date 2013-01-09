#ifndef TEAMWIDGET_H
#define TEAMWIDGET_H

#include <QWidget>

#include "ui/multiselectcalendarwidget.h"

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
    /*!
     * \brief initPersonal Initializes the personal box.
     */
    void initPersonal();

    /*!
     * \brief initTimeOff Initializes the time off request calendar
     */
    void initTimeOff();


    Ui::TeamWidget *ui;
    MultiSelectCalendarWidget *_calTimeOff;
};

#endif // TEAMWIDGET_H
