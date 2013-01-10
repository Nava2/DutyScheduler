#include "examcalendar.h"

#include <QTextCharFormat>
#include <QSignalMapper>

using namespace ctrl;

ExamCalendar::ExamCalendar(ExamList * const exams, const bool isMidterm,
                           QCalendarWidget * const calendar,
                           const QList<QCheckBox *> &cbs,
                           QObject *parent) :
    QObject(parent), _isMidterm(isMidterm), _cal(calendar), _checkboxes(cbs), _exams(exams)
{

    connect(_cal, SIGNAL(clicked(QDate)), this, SLOT(onCalendarDateClicked(QDate)));

    QSignalMapper *mapper = new QSignalMapper(this);

    foreach (QCheckBox *cb, _checkboxes) {
        QString txt = cb->text().toLower();
        if (txt == "morning") {
            mapper->setMapping(cb, Exam::MORNING);
        } else if (txt == "afternoon") {
            mapper->setMapping(cb, Exam::AFTERNOON);
        } else {
            mapper->setMapping(cb, Exam::NIGHT);
        }

        connect(cb, SIGNAL(clicked()), mapper, SLOT(map()));
    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(onCheckBoxCheck(int)));
}

ExamCalendar::~ExamCalendar() {
    ; // pass
}

// accessors:
/////////////////

QList<QCheckBox *> ExamCalendar::checkboxes() const {
    return _checkboxes;
}

QCalendarWidget *ExamCalendar::calendar() const {
    return _cal;
}

bool ExamCalendar::isMidterm() const {
    return _isMidterm;
}

void ExamCalendar::setStaff(const Staff::Ptr &staff) {
    _curStaff = staff;

    if (staff) {
        // valid ptr:

    } else {
        reset();
    }
}

void ExamCalendar::reset() {
    for (QDate dIter = _cal->minimumDate(); dIter <= _cal->maximumDate(); dIter = dIter.addDays(1)) {
        _cal->setDateTextFormat(dIter, _cal->weekdayTextFormat(static_cast<Qt::DayOfWeek>(dIter.dayOfWeek())));
    }

    foreach (QCheckBox *box, _checkboxes) {
        box->setChecked(false);
    }
}

// SLOTS:
////////////////////////////

void ExamCalendar::onCalendarDateClicked(const QDate &date) {
    _curExams = _exams.getExams(date);

    foreach (Exam::Ptr p, _curExams) {
        if (p) {
            _checkboxes[(int)p->getPeriod()]->setChecked(true);
        }
    }

    _curDate = date;
}

void ExamCalendar::onCheckBoxCheck(const int time) {
    Exam::Ptr eptr = _curExams[time];

    if (!eptr) {
        eptr = Exam::Ptr(new Exam(-1, _curDate, static_cast<Exam::Period>(time)));

        eptr->addStaff(_curStaff->getId());

        _exams->add(eptr);
        _curExams = _exams->getExams(_curDate);
    } else {
        eptr->addStaff(_curStaff->getId());
    }
}

