#include <QtGui>

#include <QDateTime>
#include <QList>
#include <QString>


#include "schedulewizzard.h"
#include "schedulewidget.h"



ScheduleWidget::ScheduleWidget(QWidget *parent)
    : QWidget(parent)
{


}

ScheduleWidget::ScheduleWidget(const QString &staffteamfilename, const ScheduleWizzard &swiz, QWidget *parent)
    : QWidget(parent)
{
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
            sDateIterator = new SDate(dateCounter,theTeam->count(),theTeam->count());
        else
            sDateIterator = new SDate(dateCounter, donsNeeded[dateCounter.dayOfWeek()-1], rasNeeded[dateCounter.dayOfWeek()-1]);


        datesList.append(*sDateIterator);
        dateCounter = dateCounter.addDays(1);

        delete sDateIterator;
    }

    //create the night class lists
    for(int q = 0; q<7; q++)
        nightClasses.append(new QList<int>);

    //call some other functions
    loadStaffTeamData(staffteamfilename);
    createScheduleGroupBox();
    prepInterface();

    updateNeeded();

}

ScheduleWidget::ScheduleWidget(const QString &fileNameSchedule, StaffList::Ptr team, QList<Exam::Ptr> *exams, QWidget *parent) // this constructor is used by LoadSchedule
    : QWidget(parent)
{
    iohandle = new IOHandler;

    iohandle->loadSchedule(fileNameSchedule, datesList, nightClasses, donsNeeded, rasNeeded);

    theTeam = team;
    theExams = new QList<Exam::Ptr>(*exams);

    startDate = datesList.first();
    endDate = datesList.last();

    foreach (SDate _sdate, datesList) {
        bool weekend = _sdate.isWeekend();

        if (_sdate.getAM() < theTeam->count())
            theTeam->at(_sdate.getAM())->addShift(weekend, true);    //add the shift count to the staff object

        foreach (int id, _sdate.getDons()) {
            if (id < theTeam->count())
                theTeam->at(id)->addShift(weekend);
        }

        foreach (int id, _sdate.getRas()) {
            if (id < theTeam->count())
                theTeam->at(id)->addShift(weekend);
        }

    }

    createScheduleGroupBox();

    onDeckItems = new QList<QListWidgetItem*>;
    onDutyItems = new QList<QListWidgetItem*>;

    for(int x = 0; x < theTeam->count(); x++)
    {
        // make the ondeck list
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(theTeam->at(x)->getFirstName() + " " + theTeam->at(x)->getLastName());
        item->setData(Qt::UserRole,theTeam->at(x)->getId());
        onDeckItems->append(item);
        onDeckList->insertItem(0,item);


        // make the onduty list
        QListWidgetItem *item2 = new QListWidgetItem();
        item2->setText(theTeam->at(x)->getFirstName() + " " + theTeam->at(x)->getLastName());
        item2->setData(Qt::UserRole,theTeam->at(x)->getId());
        onDutyItems->append(item2);
        onDutyList->insertItem(0,item2);
        item2->setHidden(true);

        if(theTeam->at(x)->getPosition())
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
    int id = row->data(Qt::UserRole).toInt();

    schedViewWidget->setToStaff((*theTeam)[id], datesList);

    updateNeeded();

    updateStats();
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

    if (theExams) {
        delete theExams;
    }

    for(int x = 0; x < 7; x++)
    {
        delete nightClasses[x];
    }

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
}

void ScheduleWidget::createScheduleGroupBox()
{
    createScheduleControls();
    createScheduleStats();
    createCalendar();
    createLists();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(scheduleStatsGroupBox,0,0,2,1);
    layout->addWidget(OnDutyGroupBox,0,1,2,1);
    layout->addWidget(OnDeckGroupBox,0,2,3,1);
    layout->addWidget(scheduleControls,0,4);
    layout->addWidget(calendar,1,4);

    setLayout(layout);
    setWindowTitle("Schedule Tool");
}

void ScheduleWidget::createScheduleControls()
{
    copyList = new QList<int>;
    copyAM = 999;

    scheduleControls = new QGroupBox("Controls");

    exportButton = new QPushButton("Export Schedule");
    spDutyFlagButton = new QPushButton("Set date as Special Duty");
    copyButton = new QPushButton("Copy On Duty");
    pasteButton = new QPushButton("Paste On Duty");
    currentDateLabel = new QLabel(QDate::shortDayName(startDate.dayOfWeek()) + " " + QDate::shortMonthName(startDate.month()) + " " + QString::number(startDate.day()));
    currentDateLabelFIXED = new QLabel("Current Date:");
    donsNeededLabelFIXED = new QLabel("Don-ons needed:");
    rasNeededLabelFIXED = new QLabel("RAs needed :");
    donsNeededLabel = new QLabel("0");
    rasNeededLabel = new QLabel("0");

    donsNeededLabelFIXED->setStatusTip("The number of DON-ons still needed for the selected day.");
    rasNeededLabelFIXED->setStatusTip("The number of RAs still needed for the selected day.");
    donsNeededLabel->setStatusTip("The number of DON-ons still needed for the selected day.");
    rasNeededLabel->setStatusTip("The number of RAs still needed for the selected day.");

    exportButton->setStatusTip("Export the schedule as a Comma Separated File (\".csv\")");
    spDutyFlagButton->setStatusTip("Mark the selected date as Special Duty. No staff can be added to this day. Click again to un-mark as special duty.");
    copyButton->setStatusTip("Copy the list of the On Duty staff for the selected day.");
    pasteButton->setStatusTip("Paste the copied list of staff into the selected day. Copied staff who can't work this day won't be added.");

    connect(exportButton,SIGNAL(clicked()),this,SLOT(exportSchedule()));
    connect(spDutyFlagButton,SIGNAL(clicked()),this,SLOT(setSpecialDuty()));
    connect(copyButton,SIGNAL(clicked()),this,SLOT(copySlot()));
    connect(pasteButton,SIGNAL(clicked()),this,SLOT(pasteSlot()));

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(exportButton,0,0,1,2);
    layout->addWidget(spDutyFlagButton,1,0,1,2);
    layout->addWidget(copyButton,2,0);
    layout->addWidget(pasteButton,2,1);
    layout->addWidget(currentDateLabel, 3,1);
    layout->addWidget(currentDateLabelFIXED,3,0);
    layout->addWidget(donsNeededLabelFIXED,4,0);
    layout->addWidget(rasNeededLabelFIXED,5,0);
    layout->addWidget(donsNeededLabel,4,1);
    layout->addWidget(rasNeededLabel,5,1);

    scheduleControls->setLayout(layout);
}

void ScheduleWidget::createScheduleStats()
{
    scheduleStatsGroupBox = new QGroupBox("Stats");
    scheduleStatsGroupBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    Qt::ItemFlags flags = 0;
    flags |= Qt::ItemIsEnabled;

    averagesTable = new QTableWidget(2,5);
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

    statsTable = new QTableWidget(theTeam->count(),5);
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



    for (int x = 0; x < theTeam->count(); x++)
    {
        statsTable->setRowHeight(x,20);
        //name
        QTableWidgetItem *nameItem = new QTableWidgetItem();
        nameItem->setText(theTeam->at(x)->getFirstName() + " " + theTeam->at(x)->getLastName().left(1));
        nameItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        nameItem->setFlags(flags);
        statsTableItems->append(nameItem);

        //position
        QTableWidgetItem *positionItem = new QTableWidgetItem();
        if(theTeam->at(x)->getPosition())
            positionItem->setText("Don");
        else
            positionItem->setText("RA");

        positionItem->setFlags(flags);


        //total
        QTableWidgetItem *totalItem = new QTableWidgetItem();
        totalItem->setText(QString::number(theTeam->at(x)->getShifts()));
        totalItem->setFlags(flags);
        totalItem->setTextAlignment(Qt::AlignCenter);

        //weekend
        QTableWidgetItem *weekendItem = new QTableWidgetItem();
        weekendItem->setText(QString::number(theTeam->at(x)->getWeekendShifts()));
        weekendItem->setFlags(flags);
        weekendItem->setTextAlignment(Qt::AlignCenter);

        //AM
        QTableWidgetItem *amItem = new QTableWidgetItem();
        amItem->setText(QString::number(theTeam->at(x)->getAMShifts()));
        amItem->setFlags(flags);
        amItem->setTextAlignment(Qt::AlignCenter);

        statsTable->setItem(x,0,nameItem);
        statsTable->setItem(x,1,positionItem);
        statsTable->setItem(x,2,totalItem);
        statsTable->setItem(x,3,weekendItem);
        statsTable->setItem(x,4,amItem);

    }

    statsTable->setSortingEnabled(true);

    schedViewWidget = new SchedViewer(startDate, endDate);

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(averagesTable, 0, 0, 1, 5);

    layout->addWidget(statsTable, 1, 0, 1, 5);

    layout->addWidget(schedViewWidget, 2, 0, 1, 1);

    scheduleStatsGroupBox->setLayout(layout);

}

void ScheduleWidget::createCalendar()
{
    calendar = new QCalendarWidget;
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

    OnDeckGroupBox = new QGroupBox("On Deck");
    OnDutyGroupBox = new QGroupBox("On Duty");

    QGridLayout *layout = new QGridLayout;

    onDeckList = new MyQListWidget();
    onDutyList = new MyQListWidget();

    onDeckList->setStatusTip("The staff who are able to work on the selected day. Click to add a staff to the duty list. Right click a don to make them AM.");
    onDutyList->setStatusTip("The staff who are on duty for the selected day. Click to remove a staff from being on duty. The AM is bolded.");

    onDeckList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    onDutyList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    onDeckList->setMinimumWidth(100);
    onDutyList->setMinimumWidth(100);

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

    layout = new QGridLayout;
    layout->addWidget(onDeckList,0,0);
    OnDeckGroupBox->setLayout(layout);
}


void ScheduleWidget::loadStaffTeamData(QString filename)
{
    theTeam = StaffList::Ptr(new StaffList);
    theExams = new QList<Exam::Ptr>;

    bool ok = iohandle->loadStaffTeam(filename, *theTeam, *theExams);

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

    foreach (Staff::Ptr pStaff, *theTeam) {
        // make the ondeck list
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(pStaff->getFirstName() + " " + pStaff->getLastName());
        item->setData(Qt::UserRole, pStaff->getId());

        qDebug() << item->data(Qt::UserRole) << ")\t" << item->text();

        onDeckItems->append(item);
        onDeckList->insertItem(0, item);

        // make the onduty list
        QListWidgetItem *item2 = new QListWidgetItem();
        item2->setText(pStaff->getFirstName() + " " + pStaff->getLastName());
        item2->setData(Qt::UserRole, pStaff->getId());
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
                    nightClasses[y]->append(pStaff->getId());
                }
                mask <<= 1;
            }
        }

        // get the staff's dates of availabillity
        QList<QDate > availList = pStaff->getAvailability();
        foreach (QDate date, availList)
        {
            if(date.daysTo(startDate) > 0 || date.daysTo(endDate) < 0)
                continue;

            int index = dateToIndex(date);

            datesList[index].addCantWork(pStaff->getId());
        }

        if(examSchedule)
        {
            QStringList list = pStaff->getExams().split(",",QString::SkipEmptyParts);

            for(int p = 0; p < list.count(); p++)
            {
                QString inp = list.at(p);

                inp.remove("(",Qt::CaseInsensitive);
                inp.remove(")",Qt::CaseInsensitive);
                int id = inp.toInt();

                Exam exam = *(theExams->at(id));

                if(exam.daysTo(startDate) > 0 || exam.daysTo(endDate) < 0)
                    continue;

                int dateIndex = dateToIndex(exam);

                if(exam.getNight())
                    datesList[dateIndex].addCantWork(pStaff->getId());

                if(dateIndex != 0)
                    datesList[dateIndex-1].addCantWork(pStaff->getId());
            }
        }
    }

    QTableWidgetItem *row = averagesTable->item(0, 0);
    int id = row->data(Qt::UserRole).toInt();

    schedViewWidget->setToStaff((*theTeam)[id], datesList);

}

int ScheduleWidget::dateToIndex(const QDate &date)
{
    int index = startDate.daysTo(date);
    return index;
}

void ScheduleWidget::dateClicked(QDate dateSelected)
{
    currentDateLabel->setText(QDate::shortDayName(dateSelected.dayOfWeek()) + " " + QDate::shortMonthName(dateSelected.month()) + " " + QString::number(dateSelected.day()));

    int dateIndex = dateToIndex(dateSelected); // get the index of the date in "datesList"

    for(int x = 0; x<theTeam->count(); x++)
    {
        QListWidgetItem *deckItem = onDeckItems->at(x);
        QListWidgetItem *dutyItem = onDutyItems->at(x);

        int id = deckItem->data(Qt::UserRole).toInt();
        QFont font = dutyItem->font();
        //check who's on duty already
        if (datesList[dateIndex].isOn(id))
        {
            if (datesList[dateIndex].getAM() == id)
                font.setBold(true);
            else
                font.setBold(false);

            dutyItem->setFont(font);

            dutyItem->setHidden(false);
            deckItem->setHidden(true);
        } else {
            font.setBold(false);
            dutyItem->setFont(font);
            dutyItem->setHidden(true); //so this person is not onduty.

            // check availabilities
            if (nightClasses[dateSelected.dayOfWeek()-1]->contains(id) || datesList[dateIndex].staffCantWork(id))
            {
                deckItem->setHidden(true);
            }
            else
            {
                deckItem->setHidden(false);
            }
        }
    }


    updateNeeded();
}

void ScheduleWidget::addStaff(QListWidgetItem *item)
{
    int staffId = onDeckList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam->at(staffId)->getPosition();

    if (datesList[dateIndex].isOn(staffId))
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

    int itemIndex = onDeckItems->indexOf(item);
    item->setHidden(true);
    onDutyItems->at(itemIndex)->setHidden(false);
    updateNeeded();

    //color the calendar if the date is full
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }


    bool isWeekend = datesList[dateIndex].isWeekend();

    theTeam->at(staffId)->addShift(isWeekend, false);

    updateStats();
}

void ScheduleWidget::removeStaff(QListWidgetItem *item)
{

    int staffId = onDutyList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());

    bool isAM = (staffId == datesList[dateIndex].getAM());
    // remove the staff from the sDate object for the selected day.
    datesList[dateIndex].removeStaff(staffId, theTeam->at(staffId)->getPosition());

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

    bool isWeekend = datesList[dateIndex].isWeekend();

    theTeam->at(staffId)->removeShift(isWeekend, isAM);

    updateStats();
}

void ScheduleWidget::setAsAM()
{
    int staffId = onDeckList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (!theTeam->at(staffId)->getPosition())
        return;

    if (datesList[dateIndex].isOn(staffId))
        return;

    if (datesList[dateIndex].getAM() != 999)
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

    bool isWeekend = datesList[dateIndex].isWeekend();

    theTeam->at(staffId)->addShift(isWeekend, true);

    updateStats();
}

void ScheduleWidget::showMenu(QPoint p)
{
    QListWidget *list = (QListWidget*)QObject::sender();

    QListWidgetItem *item = list->itemAt(p);

    if (item == 0)
        return;

    int staffId = item->data(Qt::UserRole).toInt();

    QMenu *rightClickMenu = new QMenu(this);

    if(onDeckItems->contains(item) && theTeam->at(staffId)->getPosition())
    {
        rightClickMenu->addAction(setAsAMAction);
    }

    //QPoint point = p + list->pos();
    rightClickMenu->exec(mapToGlobal(p + list->parentWidget()->pos() + list->pos()));
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
}

void ScheduleWidget::updateNeeded()
{
    int index = dateToIndex(calendar->selectedDate());
    qDebug() << "Date index:" << index;
    donsNeededLabel->setText(QString::number(datesList[index].getDonsNeeded()));
    rasNeededLabel->setText(QString::number(datesList[index].getRasNeeded()));
}

void ScheduleWidget::showStaffSchedule(QTableWidgetItem *item)
{
    if(item->column() != 0)
        return;

    int staffId = item->data(Qt::UserRole).toInt();
    if (staffId < theTeam->count())
        schedViewWidget->setToStaff(theTeam->at(staffId), datesList);
    else
        qDebug() << "Trying to set bad index" << staffId;
}

void ScheduleWidget::copySlot()
{
    copyAM = datesList[dateToIndex(calendar->selectedDate())].getAM();

    for(int x = 0; x < onDutyItems->count(); x++)
    {
        if(!onDutyItems->at(x)->isHidden())
            copyList->append(onDutyItems->at(x)->data(Qt::UserRole).toInt());
    }

}

void ScheduleWidget::pasteSlot()
{
    setAsAM(copyAM);
    for(int x = 0; x < copyList->count(); x++)
    {
        addStaff(copyList->at(x));
    }

    copyAM = 999;
    copyList->clear();
}


void ScheduleWidget::setAsAM(int staffId)
{
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (staffId == 999)
        return;

    if (!datesList[dateIndex].canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1]->contains(staffId))
        return;

    if (!theTeam->at(staffId)->getPosition())
        return;

    if (datesList[dateIndex].isOn(staffId))
        return;

    if (datesList[dateIndex].getAM() != 999)
        return;

    if (datesList[dateIndex].isSpecial())
        return;

    datesList[dateIndex].setAM(staffId);


    onDeckItems->at(staffId)->setHidden(true);
    QListWidgetItem *item = onDutyItems->at(staffId);

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

    bool isWeekend = datesList[dateIndex].isWeekend();

    theTeam->at(staffId)->addShift(isWeekend, true);

    updateStats();
}


void ScheduleWidget::addStaff(int staffId)
{

    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam->at(staffId)->getPosition();

    if (!datesList[dateIndex].canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1]->contains(staffId))
        return;

    if (datesList[dateIndex].isOn(staffId))
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

    onDeckItems->at(staffId)->setHidden(true);
    onDutyItems->at(staffId)->setHidden(false);
    updateNeeded();

    //color the calendar if the date is full
    if(datesList[dateIndex].isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    bool isWeekend = datesList[dateIndex].isWeekend();

    theTeam->at(staffId)->addShift(isWeekend, false);

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

    QFileDialog dialog(this);
    dialog.setNameFilter("Comma Separated (*.csv)");

    QString exportFilename = dialog.getSaveFileName();
    if (exportFilename.isEmpty())
        return;

    if (exportFilename.right(4) != ".csv")
    {
        QMessageBox::warning(this, "Export Schedule","Filename must be of type \".csv\"");
        return;
    }

    QFile file(exportFilename);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Export Schedule","Cannot write file.");
        return;
    }

    QTextStream ts(&file);

    QList<QStringList*> lists;
    for(int x = 0; x<7; x++)
    {
        QStringList *dailylist = new QStringList();
        lists.append(dailylist);
    }

    QStringList writtenDates;
    writtenDates << "" << "" << "" << "" << "" << "" << "";
    int maxNeededForWeek = 0;
    int dateCounter = 0;

    //for(int y = 0; y < datesList->at(0)->getDate().dayOfWeek() - 1; y++)        // if the first day is in the middle of the week we need pre-padding
    //    lists.at(y)->append("\"\",");

    int numWeeks = endDate.weekNumber() - startDate.weekNumber() + 1;         // the number of weeks spanned by our schedule
    for(int weekCounter = 0; weekCounter < numWeeks; weekCounter++)
    {
        for(int dayOfWeekCounter = datesList[dateCounter].getWeekday()-1; dayOfWeekCounter < 7; dayOfWeekCounter++)
        {
            SDate date = datesList[dateCounter];
            writtenDates.replace(dayOfWeekCounter,"\"" + QDate::shortDayName(date.dayOfWeek()) + " " + QDate::shortMonthName(date.month()) + " " + QString::number(date.day()) + "\"");

            QString on = date.exportOn();
            if(on == "special")
            {
                lists.at(dayOfWeekCounter)->append("Special Duty");
            }
            else if (on == "")
            {
                lists.at(dayOfWeekCounter)->append("No AM");
            }
            else
            {
                *lists.at(dayOfWeekCounter) = on.split(",",QString::SkipEmptyParts);

                for(int z = 0; z < lists.at(dayOfWeekCounter)->count(); z++)             // swap the staff id's for their names in the string list
                    lists.at(dayOfWeekCounter)->replace(z, "\"" + theTeam->at(lists.at(dayOfWeekCounter)->at(z).toInt())->getFirstName()
                                                   + " " + theTeam->at(lists.at(dayOfWeekCounter)->at(z).toInt())->getLastName().left(1)+"\"");
            }
            if(lists.at(dayOfWeekCounter)->count() > maxNeededForWeek)               // this tells us how many rows will be needed this week.
                maxNeededForWeek = lists.at(dayOfWeekCounter)->count();

            dateCounter++;
            if (dateCounter >= datesList.count())                                  // check this for accuracy
                break;
        }

        for (int p = 0; p < 7; p++)                                                 // fill out the days so that they are all the same length
        {
            int count = maxNeededForWeek - lists.at(p)->count();
            for(int q = 0; q <= count; q++)
                lists.at(p)->append("\"\"");
        }


        //write the stuff out to the file.
        ts << writtenDates.join(",") << endl;
        QString out;
        for(int x = 0; x < maxNeededForWeek; x++)
        {
            for(int y = 0; y < 7; y++)
            {
                out += lists.at(y)->at(x) + ",";
            }
            ts << out << endl;
            out = "";
        }
        ts << endl;


        //clean up stuff for the next week.
        maxNeededForWeek = 0;
        writtenDates.clear();
        writtenDates << "" << "" << "" << "" << "" << "" << "";

        for(int x = 0; x < 7; x++)
            lists.at(x)->clear();


    }

    for(int x = 0; x<7; x++)
        delete lists.at(x);

    ts << endl << endl;
    ts << "Don Average, Don Weekend Average, AM Average" << endl;
    ts << donAverageItem->text() << "," << donAverageWeekendItem->text() << "," << amAverageItem->text() << endl << endl;
    ts << "RA Average,RA Weekend Average" << endl;
    ts << raAverageItem->text() << "," << raAverageWeekendItem->text() << endl << endl;


    ts << "Name,Position,Total Shifts,Weekend Shifts,AM Shifts" << endl;
    for (int x = 0; x < statsTable->rowCount(); x++)
        ts << statsTable->item(x,0)->text() << "," << statsTable->item(x,1)->text() << "," << statsTable->item(x,2)->text() << "," << statsTable->item(x,3)->text() << "," << statsTable->item(x,4)->text() << endl;

    file.close();

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
        int staffId = statsTableItems->at(x)->data(Qt::UserRole).toInt();
        int row = statsTableItems->at(x)->row();

        statsTable->item(row,2)->setText(QString::number(theTeam->at(staffId)->getShifts()));
        statsTable->item(row,3)->setText(QString::number(theTeam->at(staffId)->getWeekendShifts()));
        statsTable->item(row,4)->setText(QString::number(theTeam->at(staffId)->getAMShifts()));

        if(theTeam->at(staffId)->getPosition())
        {
            dAverage += theTeam->at(staffId)->getShifts();
            dWAverage += theTeam->at(staffId)->getWeekendShifts();
            amAverage += theTeam->at(staffId)->getAMShifts();
            dCount++;
        }
        else
        {
            rAverage += theTeam->at(staffId)->getShifts();
            rWAverage += theTeam->at(staffId)->getWeekendShifts();
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

void ScheduleWidget::saveMidSchedule(QString fileName)
{
    bool ok = iohandle->saveSchedule(fileName, datesList, nightClasses, donsNeeded, rasNeeded);

    if (!ok) {
        QString title, msg;
        iohandle->getErrorInfo(title, msg);
        QMessageBox::warning(this, title, msg);
        iohandle->clearErrorInfo();
    }
}


