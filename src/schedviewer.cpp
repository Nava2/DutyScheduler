#include "schedviewer.h"

SchedViewer::SchedViewer(const QDate &startDate, const QDate &lastDate, QWidget *parent) :
    QWidget(parent),  calendarStaff(NULL), box(NULL), labelBox(NULL), nameLabel(NULL),
    externLayout(NULL), internLayout(NULL), labelLayout(NULL)
{
    box = new QGroupBox("Schedule:");

    calendarStaff = new QCalendarWidget;
    calendarStaff->setSelectionMode(QCalendarWidget::NoSelection);
    calendarStaff->setMinimumDate(startDate.addDays(-1));
    calendarStaff->setSelectedDate(startDate.addDays(-1));
    calendarStaff->setMaximumDate(lastDate);
    calendarStaff->setFirstDayOfWeek(Qt::Monday);
    calendarStaff->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendarStaff->setGridVisible(true);

    QTextCharFormat format = calendarStaff->weekdayTextFormat(Qt::Sunday);
    calendarStaff->setWeekdayTextFormat(Qt::Friday, format);
    format.setForeground(QBrush(Qt::black, Qt::SolidPattern));
    calendarStaff->setWeekdayTextFormat(Qt::Sunday, format);
    format = calendarStaff->weekdayTextFormat(Qt::Monday);

    labelLayout = new QGridLayout;
    nameLabel = new QLabel("<b>Name:</b> ");

    QList<QLabel *> labelList;
    QList<QColor> colorList;

#define LABEL_CREATE(var, text, color) \
    var = new QLabel(text); \
    labelList.append(var); \
    colorList.append(color);

    LABEL_CREATE(amLabel, "AM", Qt::green);
    LABEL_CREATE(onLabel, "On Duty", Qt::magenta);
    LABEL_CREATE(noWorkLabel, "Requested Off", Qt::cyan);
    LABEL_CREATE(nightClassLabel, "Night Class", Qt::blue);
    LABEL_CREATE(midtermLabel, "Midterm", Qt::yellow);
    LABEL_CREATE(finalLabel, "Final", Qt::red);

#undef LABEL_CREATE

    QFont font = amLabel->font();
    font.setPixelSize(10);

    for (int x = 0; x < labelList.size(); x++) {
        QLabel *label = labelList[x];

        label->setFont(font);

        QPalette palette(label->palette());

        //white text
        QBrush brush(colorList[x]);
        brush.setStyle(Qt::SolidPattern);

        //set white text
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);

        //set palette
        label->setPalette(palette);

        labelLayout->addWidget(label, x / 2, 1 + x % 2, 1, 1);
    }

    labelLayout->addWidget(nameLabel, 0, 0, -1, 1);

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
    // delete in order of internal component out
    DELETE_IF(calendarStaff);
    DELETE_IF(amLabel);
    DELETE_IF(onLabel);
    DELETE_IF(nameLabel);
    DELETE_IF(noWorkLabel);
    DELETE_IF(midtermLabel);

    DELETE_IF(labelLayout);
    DELETE_IF(labelBox);

    DELETE_IF(internLayout);

    DELETE_IF(box);

    DELETE_IF(externLayout);
}

#undef DELETE_IF

void SchedViewer::setToStaff(Staff::Ptr pStaff, QList<SDate> &datesList) {

    if (!pStaff) {
        pStaff = staffMember;
        if (!pStaff)
            return;
    }

    nameLabel->setText("<b>Name:</b> " + pStaff->getFirstName() + " " + pStaff->getLastName());

    QTextCharFormat format = calendarStaff->weekdayTextFormat(Qt::Monday);
    foreach (SDate sdate, datesList)
    {
        if (sdate.isOn(pStaff->uid()))
        {
            format.setBackground(Qt::magenta);
            if (sdate.getAM() == pStaff->uid())
                format.setBackground(Qt::green);
        } else if (!sdate.canWork(pStaff->uid())) {
            format.setBackground(Qt::cyan);
        } else if (pStaff->isNightClass(sdate)) {
            format.setBackground(Qt::blue);
        } else {
            format.setBackground(Qt::white);
        }

        calendarStaff->setDateTextFormat(sdate, format);
    }

    format.setBackground(Qt::yellow);
    foreach (Exam::Ptr p, pStaff->getMidterms()) {
        Exam ex = *p;
        if (ex.getPeriod() == Exam::NIGHT) {
            calendarStaff->setDateTextFormat(ex, format);
        }

        calendarStaff->setDateTextFormat(ex.addDays(-1), format);
    }

    format.setBackground(Qt::red);
    foreach (Exam::Ptr p, pStaff->getFinals()) {
        Exam ex = *p;
        if (ex.getPeriod() == Exam::NIGHT) {
            calendarStaff->setDateTextFormat(ex, format);
        }

        calendarStaff->setDateTextFormat(ex.addDays(-1), format);
    }

    staffMember = pStaff;
}

Staff::Ptr SchedViewer::getStaff() {
    return staffMember;
}
