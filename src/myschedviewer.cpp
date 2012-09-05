#include "myschedviewer.h"

mySchedViewer::mySchedViewer(QString name, int staffId, QList<SDate> *datesList, QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(close()));

    calendarStaff = new QCalendarWidget;
    calendarStaff->setSelectionMode(QCalendarWidget::NoSelection);
    calendarStaff->setMinimumDate(datesList->first().addDays(-1));
    calendarStaff->setSelectedDate(datesList->first().addDays(-1));
    calendarStaff->setMaximumDate(datesList->last());
    calendarStaff->setFirstDayOfWeek(Qt::Monday);
    calendarStaff->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    QTextCharFormat format = calendarStaff->weekdayTextFormat(Qt::Sunday);
    calendarStaff->setWeekdayTextFormat(Qt::Friday, format);
    format.setForeground(QBrush(Qt::black,Qt::SolidPattern));
    calendarStaff->setWeekdayTextFormat(Qt::Sunday, format);
    format = calendarStaff->weekdayTextFormat(Qt::Monday);


    QDate dateCounter = datesList->first();
    for (int x = 0; x < datesList->count(); x++)
    {
        if(datesList->at(x).isOn(staffId))
        {
            format.setBackground(Qt::magenta);
            if(datesList->at(x).getAM() == staffId)
                format.setBackground(Qt::green);
        }
        else
            format.setBackground(Qt::white);

        calendarStaff->setDateTextFormat(dateCounter,format);
        dateCounter = dateCounter.addDays(1);
    }


    amLabel = new QLabel("AM = Green");
    onLabel = new QLabel("On Duty = Purple");

    layout = new QVBoxLayout();
    layout->addWidget(amLabel);
    layout->addWidget(onLabel);
    layout->addWidget(calendarStaff);
    layout->addWidget(buttonBox);
    setLayout(layout);
    setWindowTitle("Schedule for " + name);
}

mySchedViewer::~mySchedViewer()
{
    delete buttonBox;
    delete calendarStaff;
    delete amLabel;
    delete onLabel;
    delete layout;
}
