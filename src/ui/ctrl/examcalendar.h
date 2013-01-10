#ifndef EXAMCALENDAR_H
#define EXAMCALENDAR_H

#include <QObject>
#include <QCalendarWidget>
#include <QCheckBox>

#include <QList>
#include <QMap>
#include <QMultiMap>

#include "obj/exam.h"
#include "obj/staff.h"

namespace ctrl {

class ExamCalendar : public QObject
{
    Q_OBJECT
public:
    explicit ExamCalendar(const ExamListModel &exams, const bool isMidterm, QCalendarWidget * const calendar, const QList<QCheckBox *> &cbs, QObject *parent = 0);
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


private:
    bool _isMidterm; //!< True if midterm set

    QCalendarWidget *_cal; //!< Holds reference to the QCalendarWidget controlled
    
    QList<QCheckBox *> _checkboxes;

    ExamListModel _exams;

    QList<Exam::Ptr> _curExams;
    Staff::Ptr _curStaff;
};

}

#endif // EXAMCALENDAR_H
