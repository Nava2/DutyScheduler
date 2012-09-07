#include "schedviewer.h"

SchedViewer::SchedViewer(const QDate &startDate, const QDate &lastDate, QWidget *parent) :
    QWidget(parent), staffMember(NULL)
{
    box = new QGroupBox("Schedule:");

    calendarStaff = new QCalendarWidget;
    calendarStaff->setSelectionMode(QCalendarWidget::NoSelection);
    calendarStaff->setMinimumDate(startDate);
    calendarStaff->setSelectedDate(startDate.addDays(-1));
    calendarStaff->setMaximumDate(lastDate);
    calendarStaff->setFirstDayOfWeek(Qt::Monday);
    calendarStaff->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    QTextCharFormat format = calendarStaff->weekdayTextFormat(Qt::Sunday);
    calendarStaff->setWeekdayTextFormat(Qt::Friday, format);
    format.setForeground(QBrush(Qt::black, Qt::SolidPattern));
    calendarStaff->setWeekdayTextFormat(Qt::Sunday, format);
    format = calendarStaff->weekdayTextFormat(Qt::Monday);

    labelLayout = new QGridLayout;
    nameLabel = new QLabel("<b>Name:</b> ");

    amLabel = new QLabel("AM = Green");
    onLabel = new QLabel("On Duty = Purple");
    noWorkLabel = new QLabel("Can't work = Cyan");

    labelLayout->addWidget(nameLabel, 0, 0, -1, 1);
    labelLayout->addWidget(amLabel, 0, 1, 1, 1);
    labelLayout->addWidget(onLabel, 1, 1, 1, 1);
    labelLayout->addWidget(noWorkLabel, 2, 1, 1, -1);

    labelBox = new QGroupBox;
    labelBox->setLayout(labelLayout);

    internLayout = new QGridLayout();
    internLayout->addWidget(labelBox, 0, 0);
    internLayout->addWidget(calendarStaff, 1, 0);

    box->setLayout(internLayout);

    externLayout = new QVBoxLayout;
    externLayout->addWidget(box);
    setLayout(externLayout);
}

#define DELETE_IF(ptr) if (ptr) delete ptr;

SchedViewer::~SchedViewer()
{
    DELETE_IF(calendarStaff);
    DELETE_IF(amLabel);
    DELETE_IF(onLabel);
    DELETE_IF(nameLabel);
    DELETE_IF(noWorkLabel);
    DELETE_IF(internLayout);
    DELETE_IF(externLayout);

    DELETE_IF(box);
    DELETE_IF(labelLayout);
    DELETE_IF(labelBox);
}

#undef DELETE_IF

void SchedViewer::setToStaff(Staff::Ptr pStaff, QList<SDate> &datesList) {

    if (!pStaff) {
        pStaff = staffMember;
    }

    nameLabel->setText("<b>Name:</b> " + pStaff->getFirstName() + " " + pStaff->getLastName());

    QTextCharFormat format = calendarStaff->weekdayTextFormat(Qt::Monday);
    foreach (SDate sdate, datesList)
    {
        if (sdate.isOn(pStaff->getId()))
        {
            format.setBackground(Qt::magenta);
            if (sdate.getAM() == pStaff->getId())
                format.setBackground(Qt::green);
        } else if (!sdate.canWork(pStaff->getId()) || pStaff->isNightClass(sdate)) {
            format.setBackground(Qt::cyan);
        } else {
            format.setBackground(Qt::white);
        }

        calendarStaff->setDateTextFormat(sdate, format);
    }

    staffMember = pStaff;
}

Staff::Ptr SchedViewer::getStaff() {
    return staffMember;
}
