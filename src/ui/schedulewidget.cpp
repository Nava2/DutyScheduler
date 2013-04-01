#include <QtGui>

#include <QDateTime>
#include <QList>
#include <QString>
#include <QSignalMapper>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>

#include <math.h>


#include "schedulewizzard.h"
#include "schedulewidget.h"

#include "obj/stafflist.h"

#include "mainwindow.h"

ScheduleWidget::ScheduleWidget(QWidget *parent)
    : QWidget(parent)
{
    MainWindow *w = dynamic_cast<MainWindow *>(parent);
    connect(this, SIGNAL(updateSaveState()), w, SLOT(onUpdateSaveState()));
    _adjustCBDayDuty = false;
}

ScheduleWidget::ScheduleWidget(const QString &staffteamfilename, const ScheduleWizzard &swiz, QWidget *parent)
    : QWidget(parent)
{
    _adjustCBDayDuty = false;

    // read from the wizzard
    examSchedule = swiz.isExamSchedule();
    swiz.getDates(startDate, endDate);
    swiz.getWeeklyValues(donsNeeded, rasNeeded);

    // io handler
    iohandle = new IOHandler;

    QDate dateCounter;
    SDate *sDateIterator;
    dateCounter.setDate(startDate.year(), startDate.month(), startDate.day());

    int length = startDate.daysTo(endDate) + 1;

    for(int z = 0; z<length; z++)
    {
        if(examSchedule)
            sDateIterator = new SDate(dateCounter, true, theTeam.count(), theTeam.count());
        else
            sDateIterator = new SDate(dateCounter, false, donsNeeded[dateCounter.dayOfWeek()-1], rasNeeded[dateCounter.dayOfWeek()-1]);


        datesList.append(*sDateIterator);
        dateCounter = dateCounter.addDays(1);

        delete sDateIterator;
    }

    //create the night class lists
    nightClasses.clear();
    for(int q = 0; q<7; q++)
        nightClasses.append(QList<QString >());

    //call some other functions
    loadStaffTeamData(staffteamfilename);
    createScheduleGroupBoxs();
    prepInterface();

    updateNeeded();

    MainWindow *w = dynamic_cast<MainWindow *>(parent);
    connect(this, SIGNAL(updateSaveState()), w, SLOT(onUpdateSaveState()));
}

ScheduleWidget::ScheduleWidget(const QString &fileNameSchedule,
                               StaffList &team,
                               QList<Exam::Ptr> &finals,
                               QList<Exam::Ptr> &midterms,
                               QWidget *parent) // this constructor is used by LoadSchedule
    : QWidget(parent)
{    
    _adjustCBDayDuty = false;
    iohandle = new IOHandler;

    bool valid = iohandle->loadSchedule(fileNameSchedule, team, datesList, nightClasses, donsNeeded, rasNeeded);

    if (!valid) {
        return;
    }

    theTeam = team;
    theFinals = finals;
    theMidterms = midterms;

    startDate = datesList.first();
    endDate = datesList.last();

    //create the night class lists
    for(int q = 0; q<7; q++)
        nightClasses.append(QList<QString >());

    foreach (SDate _sdate, datesList) {
        Staff::ShiftTypes weekend = _sdate.isWeekend() ? Staff::NIGHT | Staff::WEEKEND : Staff::NIGHT;

        QString amID = _sdate.getAM();
        if (amID != SDate::AM_NOT_SET) {
            if (theTeam[amID]->getId() < theTeam.count())
                theTeam[amID]->addShift( weekend | Staff::AM);    //add the shift count to the staff object
        }

        foreach (QString id, _sdate.getDons()) {
            Staff::Ptr pstaff = theTeam[id];
            if (pstaff->getId() < theTeam.count())
                pstaff->addShift(weekend);
        }

        foreach (QString id, _sdate.getRas()) {
            Staff::Ptr pstaff = theTeam[id];
            if (pstaff->getId() < theTeam.count())
                pstaff->addShift(weekend);
        }

        // adjust for day duty
        foreach (Staff::Ptr p, theTeam) {
            if (_sdate.isOnDayDuty(p->uid())) {
                p->addShift(Staff::DAY);
            }
        }


    }

    createScheduleGroupBoxs();

    onDeckItems = new QList<QListWidgetItem*>;
    onDutyItems = new QList<QListWidgetItem*>;

    foreach (Staff::Ptr pstaff, theTeam)
    {
        // make the ondeck list
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(pstaff->getFirstName() + " " + pstaff->getLastName());
        item->setData(Qt::UserRole, pstaff->uid());
        onDeckItems->append(item);
        onDeckList->insertItem(0,item);


        // make the onduty list
        QListWidgetItem *item2 = new QListWidgetItem();
        item2->setText(pstaff->getFirstName() + " " + pstaff->getLastName());
        item2->setData(Qt::UserRole, pstaff->uid());
        onDutyItems->append(item2);
        onDutyList->insertItem(0,item2);
        item2->setHidden(true);

        if (pstaff->getPosition())
        {
            item->setBackgroundColor(QColor(255,0,0,127));
            item2->setBackgroundColor(QColor(255,0,0,127));
        }
    }

    foreach(SDate sdate, datesList)
    {
        if (sdate.isFull())
        {
            QTextCharFormat format = calendar->dateTextFormat(sdate);
            format.setBackground(Qt::green);
            calendar->setDateTextFormat(sdate, format);
        }
        if (sdate.isSpecial())
        {
            QTextCharFormat format = calendar->dateTextFormat(sdate);
            format.setBackground(Qt::darkGray);
            calendar->setDateTextFormat(sdate, format);
        }
    }

    dateClicked(startDate);

    QTableWidgetItem *row = averagesTable->item(0, 0);
    QString id = row->data(Qt::UserRole).toString();

    schedViewWidget->setToStaff(theTeam[id], datesList);

    updateNeeded();

    updateStats();

    MainWindow *w = dynamic_cast<MainWindow *>(parent);
    connect(this, SIGNAL(updateSaveState()), w, SLOT(onUpdateSaveState()));
}

ScheduleWidget::~ScheduleWidget()
{

    QList<QTableWidgetItem*>::iterator it_i = statsTableItems->begin();

    for (; it_i != statsTableItems->end();)
    {
        delete *it_i;
        it_i = statsTableItems->erase(it_i);
    }
    delete statsTableItems;

    QList<QListWidgetItem*>::iterator it_i2 = onDeckItems->begin();
    for (; it_i2 != onDeckItems->end();)
    {
        delete *it_i2;
        it_i2 = onDeckItems->erase(it_i2);
    }
    delete onDeckItems;

    QList<QListWidgetItem*>::iterator it_i3 = onDutyItems->begin();
    for (; it_i3 != onDutyItems->end();)
    {
        delete *it_i3;
        it_i3 = onDutyItems->erase(it_i3);
    }
    delete onDutyItems;

    delete setAsAMAction;

    delete copyList;

    delete iohandle;
}

void ScheduleWidget::createScheduleGroupBoxs()
{
    createScheduleControls();
    createScheduleStats();
    createCalendar();
    createLists();

    QWidget *listOuter = new QWidget(this);
    listOuter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGridLayout *listsLayout = new QGridLayout(listOuter);


    QGroupBox *gbDayDuty = new QGroupBox("Exam Day Duty:", this);
    gbDayDuty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
//    gbDayDuty->setMinimumWidth(cbDayDuty[0]->width() * 2.25);

    QLabel *dayDutyLabel[] = {new QLabel("8:00 AM-1:30 PM", gbDayDuty),
                              new QLabel("1:30 PM-7:00 PM", gbDayDuty)};

    QGridLayout *layoutDayDuty = new QGridLayout(gbDayDuty);
    gbDayDuty->setLayout(layoutDayDuty);
//    layoutDayDuty->setSpacing(5);

    for (int i = 0; i < 2; ++i) {
        cbDayDuty[i] = new QComboBox(this);
        cbDayDuty[i]->setInsertPolicy(QComboBox::InsertAlphabetically);
        connect(cbDayDuty[i], SIGNAL(currentIndexChanged(int)), this, SLOT(changeDayDutyIndex(int)));

        dayDutyLabel[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        cbDayDuty[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

        layoutDayDuty->addWidget(dayDutyLabel[i], 0, i, 1, 1, Qt::AlignLeft);
        layoutDayDuty->addWidget(cbDayDuty[i], 1, i, 1, 1, Qt::AlignLeft);
    }

    listsLayout->addWidget(OnDutyGroupBox, 0, 0, 1, 1);
    listsLayout->addWidget(OnDeckGroupBox, 0, 1, 1, 1);
    listsLayout->addWidget(gbDayDuty, 1, 0, 1, 2);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(scheduleStatsGroupBox,0,0,2,1);
    layout->addWidget(listOuter, 0, 1, 2, 1);
    layout->addWidget(scheduleControls, 0, 3);
    layout->addWidget(calendar,1,3);

    setLayout(layout);
    setWindowTitle("Schedule Tool");

    scheduleMapper = new QSignalMapper(this);

    foreach (Staff::Ptr pstaff, theTeam) {
        QAction *action = new QAction("Show Schedule", this);

        connect(action, SIGNAL(triggered()), scheduleMapper, SLOT(map()));

        scheduleMapper->setMapping(action, pstaff->uid());

        showScheduleActions += action;
    }

    connect(scheduleMapper, SIGNAL(mapped(const QString &)),
                 this, SLOT(showSchedule(const QString &)));
//    connect(this, SIGNAL(showSchedule(const QString &)), this, SLOT(onShowSchedule(const QString &)));
}

void ScheduleWidget::createScheduleControls()
{
    copyList = new QList<QString >;
    copyAM = SDate::AM_NOT_SET;

    scheduleControls = new QGroupBox("Controls", this);

    exportButton = new QPushButton("Export Schedule", this);
    spDutyFlagButton = new QPushButton("Set date as Special Duty", this);
    copyButton = new QPushButton("Copy On Duty", this);
    pasteButton = new QPushButton("Paste On Duty", this);
    currentDateLabel = new QLabel(QDate::shortDayName(startDate.dayOfWeek()) + " " + QDate::shortMonthName(startDate.month()) + " " + QString::number(startDate.day()));
    currentDateLabel->setParent(this);
    currentDateLabelFIXED = new QLabel("Current Date:", this);
    donsNeededLabelFIXED = new QLabel("Don-ons needed:", this);
    rasNeededLabelFIXED = new QLabel("RAs needed :", this);

    rasNeededLabel  = new QLabel("0", this);
    donsNeededLabel = new QLabel("0", this);

    spnDonsNeeded = new QSpinBox(this);
    spnDonsNeeded->setMinimum(0);
    spnDonsNeeded->setMaximum(floor(theTeam.countDon() / 2.0));
    connect(spnDonsNeeded, SIGNAL(valueChanged(int)), this, SLOT(updateDonsNeededDay(int)));

    spnRAsNeeded = new QSpinBox(this);
    spnRAsNeeded->setMinimum(1);
    spnRAsNeeded->setMaximum(floor(theTeam.countRA() / 2.0));
    connect(spnRAsNeeded, SIGNAL(valueChanged(int)), this, SLOT(updateRAsNeededDay(int)));

    donsNeededLabelFIXED->setStatusTip("The number of DON-ons still needed for the selected day.");
    rasNeededLabelFIXED->setStatusTip("The number of RAs still needed for the selected day.");
    spnDonsNeeded->setStatusTip("The number of DON-ons still needed for the selected day.");
    spnRAsNeeded->setStatusTip("The number of RAs still needed for the selected day.");

    exportButton->setStatusTip("Export the schedule as a Comma Separated File (\".csv\")");
    spDutyFlagButton->setStatusTip("Mark the selected date as Special Duty. No staff can be added to this day. Click again to un-mark as special duty.");
    copyButton->setStatusTip("Copy the list of the On Duty staff for the selected day.");
    pasteButton->setStatusTip("Paste the copied list of staff into the selected day. Copied staff who can't work this day won't be added.");

    connect(exportButton,SIGNAL(clicked()),this,SLOT(exportSchedule()));
    connect(spDutyFlagButton,SIGNAL(clicked()),this,SLOT(setSpecialDuty()));
    connect(copyButton,SIGNAL(clicked()),this,SLOT(copySlot()));
    connect(pasteButton,SIGNAL(clicked()),this,SLOT(pasteSlot()));

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(exportButton, 0, 0, 1, 4);
    layout->addWidget(spDutyFlagButton, 1, 0, 1, 4);
    layout->addWidget(copyButton, 2, 0, 1, 2);
    layout->addWidget(pasteButton, 2, 2, 1, 2);
    layout->addWidget(currentDateLabel, 3, 2, 1, 2);
    layout->addWidget(currentDateLabelFIXED,3, 0, 1, 2);

    layout->addWidget(donsNeededLabelFIXED, 4, 0, 1, 1);
    layout->addWidget(rasNeededLabelFIXED, 5, 0, 1, 1);

    layout->addWidget(donsNeededLabel, 4, 1, 1, 1);
    layout->addWidget(rasNeededLabel, 5, 1, 1, 1);

    layout->addWidget(spnDonsNeeded, 4, 3, 1, 1);
    layout->addWidget(spnRAsNeeded, 5, 3, 1, 1);

    scheduleControls->setLayout(layout);
}

void ScheduleWidget::createScheduleStats()
{
    scheduleStatsGroupBox = new QGroupBox("Stats", this);
    scheduleStatsGroupBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    Qt::ItemFlags flags = 0;
    flags |= Qt::ItemIsEnabled;

    averagesTable = new QTableWidget(2, 5, this);
    averagesTable->setStatusTip("This table shows the average number of shifts for various types of shifts for Dona and RAs. (Weekend shifts are included in 'total shifts')");
    averagesTable->setHorizontalHeaderLabels(QString(",Position,Total,Weekend,AM").split(",",QString::KeepEmptyParts));
    averagesTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    averagesTable->setMaximumHeight(80);
    averagesTable->setMinimumWidth(450);
    averagesTable->setRowHeight(0,25);
    averagesTable->setRowHeight(1,25);
    averagesTable->setSelectionMode(QAbstractItemView::NoSelection);
    averagesTable->setColumnWidth(0,120);
    averagesTable->setColumnWidth(1,50);
    averagesTable->setColumnWidth(2,90);
    averagesTable->setColumnWidth(3,90);
    averagesTable->setColumnWidth(4,70);

    raAverageItem = new QTableWidgetItem("0");
    donAverageItem = new QTableWidgetItem("0");
    raAverageWeekendItem = new QTableWidgetItem("0");
    donAverageWeekendItem = new QTableWidgetItem("0");
    amAverageItem = new QTableWidgetItem("0");

    raAverageItem->setTextAlignment(Qt::AlignCenter);
    donAverageItem->setTextAlignment(Qt::AlignCenter);
    raAverageWeekendItem->setTextAlignment(Qt::AlignCenter);
    donAverageWeekendItem->setTextAlignment(Qt::AlignCenter);
    amAverageItem->setTextAlignment(Qt::AlignCenter);

    raAverageItem->setFlags(flags);
    donAverageItem->setFlags(flags);
    raAverageWeekendItem->setFlags(flags);
    donAverageWeekendItem->setFlags(flags);
    amAverageItem->setFlags(flags);

    averagesTable->setItem(0,0,new QTableWidgetItem("Average"));
    averagesTable->setItem(1,0,new QTableWidgetItem("Average"));
    averagesTable->setItem(0,1,new QTableWidgetItem("Don"));
    averagesTable->setItem(1,1,new QTableWidgetItem("RA"));
    averagesTable->setItem(0,2,donAverageItem);
    averagesTable->setItem(0,3,donAverageWeekendItem);
    averagesTable->setItem(0,4,amAverageItem);
    averagesTable->setItem(1,2,raAverageItem);
    averagesTable->setItem(1,3,raAverageWeekendItem);

    statsTable = new QTableWidget(theTeam.count(), 5, this);
    statsTable->setStatusTip("This table shows the number of shifts assigned to each staff member. Click a staff member's name to show their individual schedule.");
    connect(statsTable,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(showStaffSchedule(QTableWidgetItem*)));
    statsTable->setHorizontalHeaderLabels(QString("Name,Position,Total Shifts,Weekend Shifts,AM Shifts").split(","));
    statsTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    statsTable->setMinimumWidth(450);
    statsTable->setSelectionMode(QAbstractItemView::NoSelection);
    statsTable->setColumnWidth(0,120);
    statsTable->setColumnWidth(1,50);
    statsTable->setColumnWidth(2,90);
    statsTable->setColumnWidth(3,90);
    statsTable->setColumnWidth(4,70);


    statsTableItems = new QList<QTableWidgetItem*>;

    for (int x = 0; x < theTeam.count(); x++)
    {
        statsTable->setRowHeight(x,20);
        //name
        QTableWidgetItem *nameItem = new QTableWidgetItem;
        nameItem->setText(theTeam[x]->getFirstName() + " " + theTeam[x]->getLastName().left(1));
        nameItem->setData(Qt::UserRole,theTeam[x]->uid());
        nameItem->setFlags(flags);
        statsTableItems->append(nameItem);

        //position
        QTableWidgetItem *positionItem = new QTableWidgetItem;
        if(theTeam.at(x)->getPosition())
            positionItem->setText("Don");
        else
            positionItem->setText("RA");

        positionItem->setFlags(flags);


        //total
        QTableWidgetItem *totalItem = new QTableWidgetItem;
        totalItem->setText(QString::number(theTeam.at(x)->getShifts()));
        totalItem->setFlags(flags);
        totalItem->setTextAlignment(Qt::AlignCenter);

        //weekend
        QTableWidgetItem *weekendItem = new QTableWidgetItem;
        weekendItem->setText(QString::number(theTeam.at(x)->getWeekendShifts()));
        weekendItem->setFlags(flags);
        weekendItem->setTextAlignment(Qt::AlignCenter);

        //AM
        QTableWidgetItem *amItem = new QTableWidgetItem;
        amItem->setText(QString::number(theTeam.at(x)->getAMShifts()));
        amItem->setFlags(flags);
        amItem->setTextAlignment(Qt::AlignCenter);

        statsTable->setItem(x,0,nameItem);
        statsTable->setItem(x,1,positionItem);
        statsTable->setItem(x,2,totalItem);
        statsTable->setItem(x,3,weekendItem);
        statsTable->setItem(x,4,amItem);

    }

    statsTable->setSortingEnabled(true);

    schedViewWidget = new SchedViewer(startDate, endDate, this);

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(averagesTable, 0, 0, 1, 5);

    layout->addWidget(statsTable, 1, 0, 1, 5);

    layout->addWidget(schedViewWidget, 2, 0, 1, 1);

    scheduleStatsGroupBox->setLayout(layout);

}

void ScheduleWidget::createCalendar()
{
    calendar = new QCalendarWidget(this);
    calendar->setMinimumDate(startDate);
    calendar->setMaximumDate(endDate);
    calendar->setFirstDayOfWeek(Qt::Monday);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setGridVisible(true);
    calendar->setStatusTip("The dates of the schedule. Dates turn green when their duty roster is full. Special Duty dates are gray.");

    QTextCharFormat format = calendar->weekdayTextFormat(Qt::Sunday);
    calendar->setWeekdayTextFormat(Qt::Friday, format);
    format.setForeground(QBrush(Qt::black,Qt::SolidPattern));
    calendar->setWeekdayTextFormat(Qt::Sunday, format);

    connect(calendar,SIGNAL(clicked(QDate)),this,SLOT(dateClicked(QDate)));
}

void ScheduleWidget::createLists()
{
    OnDeckGroupBox = new QGroupBox("On Deck", this);
    OnDutyGroupBox = new QGroupBox("On Duty", this);

    QGridLayout *layout = new QGridLayout(this);

    onDeckList = new MyQListWidget(OnDeckGroupBox);
    onDutyList = new MyQListWidget(OnDutyGroupBox);

    onDeckList->setStatusTip("The staff who are able to work on the selected day. Click to add a staff to the duty list. Right click a don to make them AM.");
    onDutyList->setStatusTip("The staff who are on duty for the selected day. Click to remove a staff from being on duty. The AM is bolded.");

    onDeckList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    onDutyList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    onDeckList->setMinimumWidth(150);
    onDutyList->setMinimumWidth(150);

    setAsAMAction = new QAction("Set as AM", this);

    connect(onDeckList,SIGNAL(rightClickSignal(QPoint)),this,SLOT(showMenu(QPoint)));
    connect(onDutyList,SIGNAL(rightClickSignal(QPoint)),this,SLOT(showMenu(QPoint)));
    connect(setAsAMAction,SIGNAL(triggered()),this,SLOT(setAsAM()));

    connect(onDeckList,SIGNAL(leftClickSignal(QListWidgetItem*)),this,SLOT(addStaff(QListWidgetItem*)));
    connect(onDutyList,SIGNAL(leftClickSignal(QListWidgetItem*)),this,SLOT(removeStaff(QListWidgetItem*)));

    onDeckList->setSortingEnabled(true);
    onDutyList->setSortingEnabled(true);

    layout->addWidget(onDutyList,0,0);
    OnDutyGroupBox->setLayout(layout);

    layout = new QGridLayout(this);
    layout->addWidget(onDeckList,0,0);
    OnDeckGroupBox->setLayout(layout);


    defaultRABack = Qt::white;
    defaultDonBack = QColor(255,0,0,127);
}


void ScheduleWidget::loadStaffTeamData(QString filename)
{
    theTeam.clear();
    theFinals.clear();
    theMidterms.clear();

    bool ok = iohandle->loadStaffTeam(filename, theTeam, theFinals, theMidterms);

    if (!ok) {
        QString title, msg;
        iohandle->getErrorInfo(title, msg);
        QMessageBox::warning(this, title, msg);
        return;
    }
}

void ScheduleWidget::prepInterface()
{
    onDeckItems = new QList<QListWidgetItem*>;
    onDutyItems = new QList<QListWidgetItem*>;

    foreach (Staff::Ptr pStaff, theTeam) {
        // make the ondeck list
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(pStaff->getFirstName() + " " + pStaff->getLastName());
        item->setData(Qt::UserRole, pStaff->uid());

        qDebug() << item->data(Qt::UserRole) << ")\t" << item->text();

        onDeckItems->append(item);
        onDeckList->insertItem(0, item);

        // make the onduty list
        QListWidgetItem *item2 = new QListWidgetItem;
        item2->setText(pStaff->getFirstName() + " " + pStaff->getLastName());
        item2->setData(Qt::UserRole, pStaff->uid());
        onDutyItems->append(item2);
        onDutyList->insertItem(0,item2);
        item2->setHidden(true);

        if(pStaff->getPosition())
        {
            item->setBackgroundColor(QColor(255,0,0,127));
            item2->setBackgroundColor(QColor(255,0,0,127));
        }


        // get the staff's night class availability
        if(!examSchedule)
        {
            int nights = pStaff->getNightClass();
            int mask = 0x01;
            for (int y = 0; y<7; y++)
            {
                if(nights & mask)
                {   // add this staff member to the list of who's
                    // unavailable on this night
                    nightClasses[y] += pStaff->uid();
                }
                mask <<= 1;
            }
        }

        // get the staff's dates of availabillity
        QList<QDate > availList;
        pStaff->getAvailability(availList);
        foreach (QDate date, availList)
        {
            if(date.daysTo(startDate) > 0 || date.daysTo(endDate) < 0)
                continue;

            int index = dateToIndex(date);

            datesList[index].addCantWork(pStaff->uid());
        }

        // add midterms' ids
        foreach (Exam::Ptr pExam, pStaff->getMidterms())
        {
            Exam exam = *pExam;

            if(exam.daysTo(startDate) > 0 || exam.daysTo(endDate) < 0)
                continue;

            int dateIndex = dateToIndex(exam);

            if (exam.getPeriod() == Exam::NIGHT)
                datesList[dateIndex].addCantWork(pStaff->uid());

            if(dateIndex != 0)
                datesList[dateIndex-1].addCantWork(pStaff->uid());
        }

        if(examSchedule)
        {
            foreach (Exam::Ptr pExam, pStaff->getFinals())
            {
                Exam exam = *pExam;

                if(exam.daysTo(startDate) > 0 || exam.daysTo(endDate) < 0)
                    continue;

                int dateIndex = dateToIndex(exam);

                if(exam.getPeriod() == Exam::NIGHT)
                    datesList[dateIndex].addCantWork(pStaff->uid());

                if(dateIndex != 0)
                    datesList[dateIndex-1].addCantWork(pStaff->uid());
            }
        }
    }

    QTableWidgetItem *row = averagesTable->item(0, 0);
    QString id = row->data(Qt::UserRole).toString();

    schedViewWidget->setToStaff(theTeam[id], datesList);

}

int ScheduleWidget::dateToIndex(const QDate &date)
{
    int index = datesList[0].daysTo(date);
    return index;
}

void ScheduleWidget::dateClicked(QDate dateSelected)
{
    currentDateLabel->setText(QDate::shortDayName(dateSelected.dayOfWeek()) + " " +
                              QDate::shortMonthName(dateSelected.month()) + " " +
                              QString::number(dateSelected.day()));

    int dateIndex = dateToIndex(dateSelected); // get the index of the date in "datesList"
    SDate *prev = nullptr, *next = nullptr;
    if (dateIndex > 0) {
        prev = &(datesList[dateIndex-1]);
    }
    if (dateIndex < (datesList.size() - 1)) {
        next = &(datesList[dateIndex + 1]);
    }

    cbDayDuty[0]->setEnabled(datesList[dateIndex].isExam());
    cbDayDuty[1]->setEnabled(datesList[dateIndex].isExam());

    dayDutyPrevIDs[0] = dayDutyPrevIDs[1] = "";

    QList<Exam::Ptr> curEPtrs, nextEPtrs;
    if (datesList[dateIndex].isExam()) {
        QDate cDate = datesList[dateIndex];
        foreach (Exam::Ptr ptr, theFinals) {
            QDate d = *ptr;
            if (d == cDate) {
                curEPtrs += ptr;
            } else if (next && d == *next) {
                nextEPtrs += ptr;
            }
        }
    } else {
        _adjustCBDayDuty = true;
        cbDayDuty[0]->clear();
        cbDayDuty[1]->clear();
        _adjustCBDayDuty = false;
    }

    QMap<QString, QString> dayDutyIDs[2]; // uid, name

    foreach (Staff::Ptr p, theTeam) {
        QString name = p->getFirstName() + " " + p->getLastName();
        dayDutyIDs[0][p->uid()] = name;
        dayDutyIDs[1][p->uid()] = name;
    }

    for(int x = 0; x < theTeam.count(); x++)
    {
        QListWidgetItem *deckItem = onDeckItems->at(x);
        QListWidgetItem *dutyItem = onDutyItems->at(x);

        QString id = deckItem->data(Qt::UserRole).toString();
        QFont font = dutyItem->font();
        //check who's on duty already
        if (datesList[dateIndex].isOn(id, false))
        {
            if (datesList[dateIndex].getAM() == id)
                font.setBold(true);
            else
                font.setBold(false);

            dutyItem->setFont(font);

            dutyItem->setHidden(false);
            deckItem->setHidden(true);

            if (datesList[dateIndex].isExam()) {
                dayDutyIDs[0].remove(id);
                dayDutyIDs[1].remove(id);
            }

            continue;
        }

        // check for exams:
        if (datesList[dateIndex].isExam()) {
            // already scheduled for day duty..
            bool canWorkNight = id != datesList[dateIndex].dayShiftMember(0) &&
                    id != datesList[dateIndex].dayShiftMember(1);

            // check all exams on current day
            foreach (Exam::Ptr p, curEPtrs) {
                QList<QString> staff = p->getStaff();
                if (staff.contains(id)) {
                    // staff member has this exam
                    switch (p->getPeriod()) {
                    case Exam::MORNING:
                        dayDutyIDs[0].remove(id);
                        break; // don't care about the current day
                    case Exam::NIGHT:
                        canWorkNight = false;
                        // intentional fall through

                    case Exam::AFTERNOON:
                        dayDutyIDs[0].remove(id);
                        dayDutyIDs[1].remove(id);
                        break;
                    }
                }
            }

            foreach (Exam::Ptr p, nextEPtrs) {
                QList<QString> staff = p->getStaff();
                if (staff.contains(id)) {
                    // staff member has this exam
                    canWorkNight = false;

                    // since its the next day, give 24h
                    switch (p->getPeriod()) {
                    case Exam::MORNING:
                        // no day duty day before
                        dayDutyIDs[0].remove(id);
                        dayDutyIDs[1].remove(id);
                        break;
                    case Exam::AFTERNOON:
                        dayDutyIDs[1].remove(id);
                        break;
                    case Exam::NIGHT:
                        // night exam next night, so no night duty night before
                        // but day duty is possible
                        break;
                    }
                }
            }

            if (!canWorkNight) {
                // can't work this night, aren't on duty
                deckItem->setHidden(true);
                dutyItem->setHidden(true);
                continue;
            } else {
                deckItem->setHidden(false);
                dutyItem->setHidden(true);
            }
        }

        // know a staff member can work today
        // they are not on duty
        font.setBold(false);
        dutyItem->setFont(font);
        dutyItem->setHidden(true); //so this person is not onduty.

        if (!datesList[dateIndex].isExam()){
            // not an exam day
            // check availabilities
            if (nightClasses[dateSelected.dayOfWeek()-1].contains(id) || datesList[dateIndex].staffCantWork(id))
            {
                deckItem->setHidden(true);
            } else {
                deckItem->setHidden(false);
            }
        }

        if (prev && prev->isOn(id, true)) {
            deckItem->setBackgroundColor(Qt::yellow);
        } else if (theTeam[id]->getPosition()) {
            // don
            deckItem->setBackgroundColor(defaultDonBack);
        } else {
            deckItem->setBackgroundColor(defaultRABack);
        }
    }

    // fill combo-boxes with available day duty staff
    // also set the current index to be the on duty member
    _adjustCBDayDuty = true;
    for (int i = 0; i < 2; ++i) {
        cbDayDuty[i]->clear();

        cbDayDuty[i]->addItem("");

        QMap<QString, QString> tMap;
        foreach (QString k, dayDutyIDs[i].keys()) {
            QString v = dayDutyIDs[i][k];
            tMap.insert(v, k);
        }

        QList<QString> keys = tMap.keys();
        qSort(keys.begin(), keys.end());

        foreach (QString key, keys) {
            cbDayDuty[i]->addItem(key, tMap[key]);
        }

        QString dutyID = datesList[dateIndex].dayShiftMember(i);
        if (!dutyID.isEmpty()) {
            int index = cbDayDuty[i]->findData(dutyID, Qt::UserRole);
            cbDayDuty[i]->setCurrentIndex(index);
        }
    }
    _adjustCBDayDuty = false;


//    QDate tDate = calendar->selectedDate();
//    int i = dateToIndex(tDate);

//    datesList[i].setDonsNeeded(spnDonsNeeded->value());
//    datesList[i].setRasNeeded(spnRAsNeeded->value());

    spnDonsNeeded->setValue(datesList[dateIndex].getDonsNeeded());
    spnRAsNeeded->setValue(datesList[dateIndex].getRAsNeeded());

    updateNeeded();
}

void ScheduleWidget::addStaff(QListWidgetItem *item)
{
    QString staffId = onDeckList->currentItem()->data(Qt::UserRole).toString();
    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam.at(staffId)->getPosition();

    if (datesList[dateIndex].isOn(staffId, true))
        return;

    if (datesList[dateIndex].isSpecial())
        return;

    //color the calendar if the date is full
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);

        return;
    }

    if(pos)// adding a don on
    {
        if (datesList[dateIndex].donsFull())
            return;
    }
    else
    {
        if (datesList[dateIndex].rasFull())
            return;
    }
    datesList[dateIndex].addStaff(staffId, pos);

    int itemIndex = onDeckItems->indexOf(item);
    item->setHidden(true);
    onDutyItems->at(itemIndex)->setHidden(false);
    updateNeeded();

    Staff::ShiftTypes shft = datesList[dateIndex].isWeekend() ? Staff::WEEKEND : Staff::NONE;
    theTeam[staffId]->addShift(shft | Staff::NIGHT);

    emit updateSaveState();

    updateStats();
}

void ScheduleWidget::removeStaff(QListWidgetItem *item)
{

    QString staffId = onDutyList->currentItem()->data(Qt::UserRole).toString();
    int dateIndex = dateToIndex(calendar->selectedDate());

    bool isAM = (staffId == datesList[dateIndex].getAM());
    // remove the staff from the sDate object for the selected day.
    datesList[dateIndex].removeStaff(staffId, theTeam.at(staffId)->getPosition());

    //make the item invisible in the on duty list.
    item->setHidden(true);
    QFont font = item->font();
    font.setBold(false);
    item->setFont(font);

    // make the item visible again in the on deck list.
    onDeckItems->at(onDutyItems->indexOf(item))->setHidden(false);
    updateNeeded();

    //color the calendar date white cause it ain't full
    QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
    format.setBackground(Qt::white);
    calendar->setDateTextFormat(calendar->selectedDate(),format);


    //update the stats counters

    Staff::ShiftTypes shft = datesList[dateIndex].isWeekend() ? Staff::WEEKEND : Staff::NONE;
    if (isAM)
        shft = shft | Staff::AM;

    theTeam[staffId]->removeShift(shft | Staff::NIGHT);

    if (datesList[dateIndex].isExam()) {
        // fix the day duty lists
        for (int i = 0; i < 2; ++i) {
            cbDayDuty[i]->addItem(theTeam[staffId]->getFirstName() + " " + theTeam[staffId]->getLastName(), staffId);
        }
    }


    emit updateSaveState();

    updateStats();
}

void ScheduleWidget::setAsAM()
{
    QString staffId = onDeckList->currentItem()->data(Qt::UserRole).toString();
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (!theTeam.at(staffId)->getPosition())
        return;

    if (datesList[dateIndex].isOn(staffId, true))
        return;

    if (datesList[dateIndex].getAM() != SDate::AM_NOT_SET)
        return;

    if (datesList[dateIndex].isSpecial())
        return;

    datesList[dateIndex].setAM(staffId);


    onDeckList->currentItem()->setHidden(true);
    QListWidgetItem *item = onDutyItems->at(onDeckItems->indexOf(onDeckList->currentItem()));

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setHidden(false);


    //color the date on the calendar if it's full.
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    Staff::ShiftTypes shft = datesList[dateIndex].isWeekend() ? Staff::WEEKEND : Staff::NONE;
    theTeam[staffId]->addShift(shft | Staff::AM);

    emit updateSaveState();

    updateStats();
}

void ScheduleWidget::showMenu(QPoint p)
{
    QListWidget *list = (QListWidget*)sender();

    QListWidgetItem *item = list->itemAt(p);

    if (item == NULL)
        return;

    QString staffId = item->data(Qt::UserRole).toString();

    QMenu *rightClickMenu = new QMenu(this);

    if(onDeckItems->contains(item) && theTeam[staffId]->getPosition())
    {
        rightClickMenu->addAction(setAsAMAction);
        rightClickMenu->addSeparator();
    }

    rightClickMenu->addAction(showScheduleActions[theTeam[staffId]->getId()]);

    //QPoint point = p + list->pos();
    rightClickMenu->exec(list->mapToGlobal(p));
//    rightClickMenu->exec(mapToGlobal(p + list->parentWidget()->pos() + list->pos()));
}

void ScheduleWidget::setSpecialDuty()
{
    int index = dateToIndex(calendar->selectedDate());
    if(datesList[index].isSpecial())
    {
        datesList[index].setSpecial(false);
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::white);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }
    else
    {
        datesList[index].setSpecial(true);
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::darkGray);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    emit updateSaveState();
}

void ScheduleWidget::updateNeeded()
{
    int index = dateToIndex(calendar->selectedDate());
//    qDebug() << "Date index:" << index;

    donsNeededLabel->setText(QString::number(datesList[index].getDonsLeft()));
    rasNeededLabel->setText(QString::number(datesList[index].getRasLeft()));
}

void ScheduleWidget::showStaffSchedule(QTableWidgetItem *item)
{
    if(item->column() != 0)
        return;

    showSchedule(item->data(Qt::UserRole).toString());
}

void ScheduleWidget::showSchedule(const QString &id) {
    Staff::Ptr pstaff = theTeam[id];
    if (pstaff && pstaff->getId() < theTeam.count())
        schedViewWidget->setToStaff(pstaff, datesList);
    else
        qDebug() << "Trying to set schedule to bad uid [" << id << "]";
}

void ScheduleWidget::changeDayDutyIndex(const int newIndex) {
    if (_adjustCBDayDuty || newIndex == -1)
        return ;

    QComboBox *cmb = dynamic_cast<QComboBox *>(sender());

    // read the currently stored values
    QString sid; // the previously stored ID
    for (int i = 0; i < 2; ++i) {
        if (cmb == cbDayDuty[i] && !dayDutyPrevIDs[i].isEmpty()) {
            sid = dayDutyPrevIDs[i];
            //TODO Store day duty
            break;
        }
    }

    // search setting the new staff member off-deck, and old one on-deck
    // reset to be ondeck
    QString id = cmb->itemData(newIndex, Qt::UserRole).toString();
    bool f[] = {false, false};
    for (int i = 0; i < theTeam.size()  && !(f[0] && f[1]); ++i) {
        QListWidgetItem *ptr = onDeckItems->at(i);
        Staff::Ptr sptr = theTeam[i];
        if (sptr->uid() == id) {
            sptr->addShift(Staff::DAY);
            ptr->setHidden(true);
            f[0] = true;
        }

        if (sptr->uid() == sid) {
            ptr->setHidden(false);
            sptr->removeShift(Staff::DAY);
            f[1] = true;
        }
    }

    // save the values of both comboboxes, and to the day
    SDate &d = datesList[dateToIndex(calendar->selectedDate())];

    for (int i = 0; i < 2; ++i) {
        QString uid = cbDayDuty[i]->itemData(cbDayDuty[i]->currentIndex(), Qt::UserRole).toString();
        dayDutyPrevIDs[i] = uid;
        d.setDayShiftMember(i, uid);
    }

    emit updateSaveState();

    updateStats();
}

void ScheduleWidget::copySlot()
{
    copyAM = datesList[dateToIndex(calendar->selectedDate())].getAM();
    copyList->clear();

    for(int x = 0; x < onDutyItems->count(); x++)
    {
        QListWidgetItem *item = onDutyItems->at(x);
        if(!item->isHidden())
            copyList->append(item->data(Qt::UserRole).toString());
    }

}

void ScheduleWidget::pasteSlot()
{
    setAsAM(copyAM);

    QList<QString > badList;
    foreach (QString uid, *copyList)
    {
        QDate cd = calendar->selectedDate();

        int index = dateToIndex(cd);

        SDate sd = datesList[index];

        if (!sd.canWork(uid)) {
            badList.append(uid);
        }
    }

    if (badList.count() == 0) {
        foreach (QString uid, *copyList)
            addStaff(uid);

        emit updateSaveState();
    } else {
        QString out;
        foreach (QString uid, badList) {
            Staff::Ptr ptr = theTeam[uid];
            out += ptr->getFirstName() + " " + ptr->getLastName() + ", ";
        }

        out = out.left(out.length() - 2);

        QMessageBox::warning(this, "Can not copy date",
                             "The following people can not work: " + out);
    }
}


void ScheduleWidget::setAsAM(const QString &staffId)
{
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (staffId == SDate::AM_NOT_SET)
        return;

    if (!datesList[dateIndex].canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1].contains(staffId))
        return;

    if (!theTeam.at(staffId)->getPosition())
        return;

    if (datesList[dateIndex].isOn(staffId, true))
        return;

    if (datesList[dateIndex].getAM() != SDate::AM_NOT_SET)
        return;

    if (datesList[dateIndex].isSpecial())
        return;

    datesList[dateIndex].setAM(staffId);

    Staff::Ptr pstaff = theTeam[staffId];

    onDeckItems->at(pstaff->getId())->setHidden(true);
    QListWidgetItem *item = onDutyItems->at(pstaff->getId());

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setHidden(false);


    //color the date on the calendar if it's full.
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    Staff::ShiftTypes isWeekend = datesList[dateIndex].isWeekend() ? Staff::WEEKEND : Staff::NONE;

    pstaff->addShift(isWeekend | Staff::AM);

    if (datesList[dateIndex].isExam()) {
        // fix the day duty lists
        for (int i = 0; i < 2; ++i) {
            int index = cbDayDuty[i]->findData(staffId, Qt::UserRole);
            if (index > 0) {
                cbDayDuty[i]->removeItem(index);
            }
        }
    }

    emit updateSaveState();

    updateStats();
}


void ScheduleWidget::addStaff(const QString &staffId)
{

    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam.at(staffId)->getPosition();

    if (!datesList[dateIndex].canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1].contains(staffId))
        return;

    if (datesList[dateIndex].isOn(staffId, true))
        return;

    if (datesList[dateIndex].isSpecial())
        return;

    if(pos)// adding a don on
    {
        if (datesList[dateIndex].donsFull())
            return;
    }
    else
    {
        if (datesList[dateIndex].rasFull())
            return;
    }
    datesList[dateIndex].addStaff(staffId, pos);

    Staff::Ptr pstaff = theTeam[staffId];

    onDeckItems->at(pstaff->getId())->setHidden(true);
    onDutyItems->at(pstaff->getId())->setHidden(false);
    updateNeeded();

    //color the calendar if the date is full
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    Staff::ShiftTypes isWeekend = datesList[dateIndex].isWeekend() ? Staff::WEEKEND : Staff::NONE;

    pstaff->addShift(isWeekend);

    if (datesList[dateIndex].isExam()) {
        // fix the day duty lists
        for (int i = 0; i < 2; ++i) {
            int index = cbDayDuty[i]->findData(pstaff->uid(), Qt::UserRole);
            if (index > 0) {
                cbDayDuty[i]->removeItem(index);
            }
        }
    }

    emit updateSaveState();

    updateStats();
}

void ScheduleWidget::exportSchedule()
{
    foreach(SDate sdate, datesList)             // IF there are some dates that aren't full issue a warning.
    {
        if(!sdate.isFull() && !sdate.isSpecial())
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Duty Schedule Tool");
            msgBox.setText("Warning: Some dates of the schedule are not full. Do you want to export anyway?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int msgbox_ret = msgBox.exec();

            if (msgbox_ret == 0x10000)
                return;
            else
                break;
        }
    }

    QString exportFile = iohandle->getSaveFileName(this, IOHandler::CSV_EXPORT);

    QMap<QString, QList<int > > map;

    QList<int > donAvgs;
    donAvgs += donAverageItem->text().toFloat();
    donAvgs += donAverageWeekendItem->text().toFloat();
    donAvgs += amAverageItem->text().toFloat();

    QList<int > raAvgs;
    raAvgs += raAverageItem->text().toFloat();
    raAvgs += raAverageWeekendItem->text().toFloat();

    map["_dAvgs"] = donAvgs;
    map["_rAvgs"] = raAvgs;

    QList<QString > ids;

    for (int i = 0; i < statsTable->rowCount(); i++) {
        QTableWidgetItem *item = statsTableItems->at(i);

        QList<int > vals;

        for (int j = 2; j <= 4; j++) {
            vals += statsTable->item(i, j)->text().toInt();
        }

        map[item->data(Qt::UserRole).toString()] = vals;
        ids += item->data(Qt::UserRole).toString();
    }

    if (!exportFile.isEmpty()) {
        bool ok = iohandle->exportSchedule(exportFile, datesList, theTeam, map, ids);
        if (!ok) {
            QString title, msg;
            iohandle->getErrorInfo(title, msg);
            QMessageBox::warning(this, title, msg);
            iohandle->clearErrorInfo();
        }
    }
}

void ScheduleWidget::updateStats()
{
    double dAverage = 0;
    double rAverage = 0;
    double dWAverage = 0;
    double rWAverage = 0;
    double amAverage = 0;

    int rCount = 0;
    int dCount = 0;


    for(int x = 0; x < statsTableItems->count(); x++)
    {
        QString staffId = statsTableItems->at(x)->data(Qt::UserRole).toString();
        Staff::Ptr pstaff = theTeam.at(staffId);
        int row = statsTableItems->at(x)->row();

        statsTable->item(row,2)->setText(QString::number(pstaff->getShifts()));
        statsTable->item(row,3)->setText(QString::number(pstaff->getWeekendShifts()));
        statsTable->item(row,4)->setText(QString::number(pstaff->getAMShifts()));

        if (pstaff->getPosition()) {
            dAverage += pstaff->getShifts();
            dWAverage += pstaff->getWeekendShifts();
            amAverage += pstaff->getAMShifts();
            dCount++;
        } else {
            rAverage += pstaff->getShifts();
            rWAverage += pstaff->getWeekendShifts();
            rCount++;
        }

    }

    rAverage = rAverage / rCount;
    dAverage = dAverage / dCount;
    rWAverage = rWAverage / rCount;
    dWAverage = dWAverage / dCount;
    amAverage = amAverage / dCount;

    donAverageItem->setText(QString::number(dAverage));
    donAverageWeekendItem->setText(QString::number(dWAverage));
    amAverageItem->setText(QString::number(amAverage));
    raAverageItem->setText(QString::number(rAverage));
    raAverageWeekendItem->setText(QString::number(rWAverage));

    schedViewWidget->setToStaff(Staff::Ptr(), datesList);
}

void ScheduleWidget::refreshStats()
{
    // reset all shifts
    foreach (Staff::Ptr ptr, theTeam) {
        ptr->setShifts(0, 0, 0, 0);
    }
    
    foreach(SDate sdate, datesList)
    {
        if (sdate.isSpecial()) {
//             qDebug() << "Special:" << (QDate)sdate;
            // don't count
            continue;
        }
        
        Qt::DayOfWeek dow = static_cast<Qt::DayOfWeek>(sdate.dayOfWeek());
//         qDebug() << "Date of week:" << dow;
        bool wkd = dow == Qt::Friday || dow == Qt::Saturday;
        Staff::ShiftTypes wkdShift = wkd ? Staff::WEEKEND : Staff::NONE;

        QString am = sdate.getAM();
        QList<QString> dons = sdate.getDons();
        QList<QString> ras = sdate.getRas();
        
        if (am != SDate::AM_NOT_SET)
            theTeam[am]->addShift(wkdShift | Staff::AM);
        
        foreach (QString id, dons) {
            theTeam[id]->addShift(wkdShift);
        }
        
        foreach (QString id, ras) {
            theTeam[id]->addShift(wkdShift);
        }

        for (int i = 0; i < 2; ++i) {
            QString uid = sdate.dayShiftMember(i);
            if (!uid.isEmpty()) {
                theTeam[uid]->addShift(Staff::DAY);
            }
        }
    }
    
    updateStats();
}


void ScheduleWidget::saveMidSchedule(const QString &fileName)
{
    bool ok = iohandle->saveSchedule(fileName, datesList, donsNeeded, rasNeeded);

    if (!ok) {
        QString title, msg;
        iohandle->getErrorInfo(title, msg);
        QMessageBox::warning(this, title, msg);
        iohandle->clearErrorInfo();
    }
}

//// SLOTS
/////////////////////////

void ScheduleWidget::updateDonsNeededDay(const int count) {
    int di = dateToIndex(calendar->selectedDate());

    if (count < datesList[di].getDons().size()) {
        spnDonsNeeded->setValue(datesList[di].getDons().size());
        return;
    }

    datesList[di].setDonsNeeded(count);

    updateNeeded();
}

void ScheduleWidget::updateRAsNeededDay(const int count) {
    int di = dateToIndex(calendar->selectedDate());

    if (count < datesList[di].getRas().size()) {
        spnRAsNeeded->setValue(datesList[di].getRas().size());
        return;
    }

    datesList[di].setRasNeeded(count);

    updateNeeded();
}
