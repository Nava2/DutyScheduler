#ifndef EXAMCALENDAR_H
#define EXAMCALENDAR_H

#include <QObject>
#include <QCalendarWidget>
#include <QCheckBox>

#include <QList>
#include <QMap>
#include <QMultiMap>

#include "obj/exam.h"
#include "obj/examlist.h"
#include "obj/staff.h"

namespace ctrl {

class ExamCalendar : public QObject
{
    Q_OBJECT
public:
    explicit ExamCalendar(ExamList * const exams, const bool isMidterm, QCalendarWidget * const calendar, const QList<QCheckBox *> &cbs, QObject *parent = 0);
    virtual
    ~ExamCalendar();

    QCalendarWidget *calendar() const;
    QList<QCheckBox *> checkboxes() const;

    bool isMidterm() const;

    /*!
     * \brief setStaff Sets the staff member to be the new staff member.
     * \param staff If not passed, will reset all components
     */
    void setStaff(const Staff::Ptr &staff = Staff::Ptr());

    void reset();

signals:
    
public slots:
    void onCalendarDateClicked(const QDate &date);

    void onCheckBoxCheck(const int time);


private:
    bool _isMidterm; //!< True if midterm set

    QCalendarWidget *_cal; //!< Holds reference to the QCalendarWidget controlled
    
    QList<QCheckBox *> _checkboxes;

    QList<Exam::Ptr> _curExams; //!< Holds the exam pointers associated with the checkboxes
    QDate _curDate;

    Staff::Ptr _curStaff;

    ExamList *_exams;
};

}

#endif // EXAMCALENDAR_H
