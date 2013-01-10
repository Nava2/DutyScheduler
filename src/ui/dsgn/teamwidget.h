#ifndef TEAMWIDGET_H
#define TEAMWIDGET_H

#include <QWidget>

#include "ui/multiselectcalendarwidget.h"
#include "ctrl/examcalendar.h"

namespace Ui {
class TeamWidget;
}

class TeamWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TeamWidget(QWidget *parent = 0);
    ~TeamWidget();

public slots:
    void updateTimeOffCount(const QDate &date, bool selected);

    /*!
     * \brief toggleWeekday Called when the QCheckBox is toggled for a week
     * \param dayOfWeek
     */
    void toggleWeekday(const int dayOfWeek);

private:
    /*!
     * \brief initPersonal Initializes the personal box.
     */
    void initPersonal();

    /*!
     * \brief initTimeOff Initializes the time off request calendar
     */
    void initTimeOff();

    /*!
     * \brief initExams Initializes exam controllers
     */
    void initExams();


    Ui::TeamWidget *ui;
    MultiSelectCalendarWidget *_calTimeOff;

    ctrl::ExamCalendar *ctrlExamCalMid, *ctrlExamCalFin;

    int _countTimeOff; //!< Total time off requests

    QMap<QString, Qt::DayOfWeek> mapShortDayToDOW;
};

#endif // TEAMWIDGET_H
