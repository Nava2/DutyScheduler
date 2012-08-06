#include <QtGui>
#include "schedulewidget.h"



scheduleWidget::scheduleWidget(QWidget *parent)
    : QWidget(parent)
{


}

scheduleWidget::scheduleWidget(QString input, QString staffteamfilename, QWidget *parent)
    : QWidget(parent)
{
    QStringList inp = input.split(",",QString::SkipEmptyParts);

    if (inp.at(2) == "Y")
        examSchedule = true;
    else
        examSchedule = false;

    startDate = new QDate();
    endDate = new QDate();

    *startDate = QDate::fromString(inp.at(0),"dd/MM/yyyy");
    *endDate = QDate::fromString(inp.at(1),"dd/MM/yyyy");

    for (int x = 0; x<7; x++)
    {
        donsNeeded[x] = inp.at(x+3).toInt();
        rasNeeded[x] = inp.at(x+10).toInt();
    }

    // create list of dates
    datesList = new QList<sDate*>;

    QDate dateCounter;
    sDate *sDateIterator;
    dateCounter.setDate(startDate->year(),startDate->month(),startDate->day());
    int length = startDate->daysTo(*endDate) + 1;

    for(int z = 0; z<length; z++)
    {
        if(examSchedule)
            sDateIterator = new sDate(dateCounter,theTeam->count(),theTeam->count());
        else
            sDateIterator = new sDate(dateCounter,donsNeeded[dateCounter.dayOfWeek()-1],rasNeeded[dateCounter.dayOfWeek()-1]);


        datesList->append(sDateIterator);
        dateCounter = dateCounter.addDays(1);
    }


    //create the night class lists
    for(int q = 0; q<7; q++)
        nightClasses[q] = new QList<int>;


    //call some other functions
    loadStaffTeamData(staffteamfilename);
    createScheduleGroupBox();
    prepInterface();
    updateNeeded();

}

scheduleWidget::~scheduleWidget()
{

}

void scheduleWidget::createScheduleGroupBox()
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

void scheduleWidget::createScheduleControls()
{
    copyList = new QList<int>;
    copyAM = 999;

    scheduleControls = new QGroupBox("Controls");

    exportButton = new QPushButton("Export Schedule");
    spDutyFlagButton = new QPushButton("Flag selected date as Sp.Duty");
    copyButton = new QPushButton("Copy");
    pasteButton = new QPushButton("Paste");
    currentDateLabel = new QLabel(QDate::shortDayName(startDate->dayOfWeek()) + " " + QDate::shortMonthName(startDate->month()) + " " + QString::number(startDate->day()));
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

void scheduleWidget::createScheduleStats()
{

    scheduleStatsGroupBox = new QGroupBox("Stats");
    scheduleStatsGroupBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    donAverageLabel = new QLabel("0");
    raAverageLabel = new QLabel("0");
    raAverageLabelFIXED = new QLabel("RA Avg. shifts:");
    donAverageLabelFIXED = new QLabel("Don Avg. shifts:");

    donAverageWeekendLabel = new QLabel("0");
    raAverageWeekendLabel = new QLabel("0");
    raAverageWeekendLabelFIXED = new QLabel("RA Avg. Weekend shifts:");
    donAverageWeekendLabelFIXED = new QLabel("Don Avg. Weekend shifts:");

    donAverageLabel->setStatusTip("The average number of shifts for dons, no differentiation between AM and DON-on. (Total or weekend depending on the selection)");
    raAverageLabel->setStatusTip("The average number of shifts for RAs. (Total or weekend depending on the selection)");
    raAverageLabelFIXED->setStatusTip("The average number of shifts for RAs. (Total or weekend depending on the selection)");
    donAverageLabelFIXED->setStatusTip("The average number of shifts for dons, no differentiation between AM and DON-on. (Total or weekend depending on the selection)");

    donAverageWeekendLabel->setStatusTip("The average number of shifts for dons, no differentiation between AM and DON-on. (Total or weekend depending on the selection)");
    raAverageWeekendLabel->setStatusTip("The average number of shifts for RAs. (Total or weekend depending on the selection)");
    raAverageWeekendLabelFIXED->setStatusTip("The average number of shifts for RAs. (Total or weekend depending on the selection)");
    donAverageWeekendLabelFIXED->setStatusTip("The average number of shifts for dons, no differentiation between AM and DON-on. (Total or weekend depending on the selection)");

    averagesTable = new QTableWidget(2,5);
    averagesTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    averagesTable->setMinimumWidth(500);
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

    averagesTable->setItem(0,0,new QTableWidgetItem("Average"));
    averagesTable->setItem(1,0,new QTableWidgetItem("Average"));
    averagesTable->setItem(0,1,new QTableWidgetItem("Don"));
    averagesTable->setItem(1,1,new QTableWidgetItem("RA"));
    averagesTable->setItem(0,2,donAverageItem);
    averagesTable->setItem(0,3,donAverageWeekendItem);
    averagesTable->setItem(0,4,amAverageItem);
    averagesTable->setItem(1,2,raAverageItem);
    averagesTable->setItem(1,3,raAverageWeekendItem);

    averagesTable->setHorizontalHeaderLabels(QString(",Position,Total,Weekend,AM").split(",",QString::KeepEmptyParts));

    statsTable = new QTableWidget(theTeam->count(),5);
    statsTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    statsTable->setMinimumWidth(500);
    statsTable->setSelectionMode(QAbstractItemView::NoSelection);
    statsTable->setColumnWidth(0,120);
    statsTable->setColumnWidth(1,50);
    statsTable->setColumnWidth(2,90);
    statsTable->setColumnWidth(3,90);
    statsTable->setColumnWidth(4,70);

    statsTableItems = new QList<QTableWidgetItem*>;


    QStringList labels;                                                                         //set the top row titles
    labels << "Name" << "Position" << "Total Shifts" << "Weekend Shifts" << "AM Shifts";
    statsTable->setHorizontalHeaderLabels(labels);

    for (int x = 0; x < theTeam->count(); x++)
    {
        //name
        QTableWidgetItem *nameItem = new QTableWidgetItem();
        nameItem->setText(theTeam->at(x)->getFirstName() + " " + theTeam->at(x)->getLastName().left(1));
        nameItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        nameItem->setFlags(0x0);
        nameItem->setFlags(Qt::ItemIsEnabled);
        nameItem->setTextAlignment(Qt::AlignCenter);


        //position
        QTableWidgetItem *positionItem = new QTableWidgetItem();
        if(theTeam->at(x)->getPosition())
            positionItem->setText("Don");
        else
            positionItem->setText("RA");
        positionItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        positionItem->setFlags(0x0);
        positionItem->setFlags(Qt::ItemIsEnabled);
        positionItem->setTextAlignment(Qt::AlignCenter);

        //total
        QTableWidgetItem *totalItem = new QTableWidgetItem();
        totalItem->setText(QString::number(theTeam->at(x)->getShifts()));
        totalItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        totalItem->setFlags(0x0);
        totalItem->setFlags(Qt::ItemIsEnabled);
        totalItem->setTextAlignment(Qt::AlignCenter);

        //weekend
        QTableWidgetItem *weekendItem = new QTableWidgetItem();
        weekendItem->setText(QString::number(theTeam->at(x)->getWeekendShifts()));
        weekendItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        weekendItem->setFlags(0x0);
        weekendItem->setFlags(Qt::ItemIsEnabled);
        weekendItem->setTextAlignment(Qt::AlignCenter);

        //AM
        QTableWidgetItem *amItem = new QTableWidgetItem();
        amItem->setText(QString::number(theTeam->at(x)->getAMShifts()));
        amItem->setData(Qt::UserRole,theTeam->at(x)->getId());
        amItem->setFlags(0x0);
        amItem->setFlags(Qt::ItemIsEnabled);
        amItem->setTextAlignment(Qt::AlignCenter);


        statsTableItems->append(nameItem);

        statsTable->setItem(x,0,nameItem);
        statsTable->setItem(x,1,positionItem);
        statsTable->setItem(x,2,totalItem);
        statsTable->setItem(x,3,weekendItem);
        statsTable->setItem(x,4,amItem);


    }
        //statsTable->setSortingEnabled(true);

    QGridLayout *layout = new QGridLayout;

    /*layout->addWidget(donAverageLabelFIXED,0,0);
    layout->addWidget(donAverageLabel,0,1);
    layout->addWidget(raAverageLabelFIXED,1,0);
    layout->addWidget(raAverageLabel,1,1);
    layout->addWidget(donAverageWeekendLabelFIXED,0,2);
    layout->addWidget(donAverageWeekendLabel,0,3);
    layout->addWidget(raAverageWeekendLabelFIXED,1,2);
    layout->addWidget(raAverageWeekendLabel,1,3);*/

    layout->addWidget(averagesTable,0,0,1,5);


    layout->addWidget(statsTable,2,0,1,5);

    scheduleStatsGroupBox->setLayout(layout);

}

void scheduleWidget::createCalendar()
{
    calendar = new QCalendarWidget;
    calendar->setMinimumDate(*startDate);
    calendar->setMaximumDate(*endDate);
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

void scheduleWidget::createLists()
{

    OnDeckGroupBox = new QGroupBox("On Deck");
    OnDutyGroupBox = new QGroupBox("On Duty");

    QGridLayout *layout = new QGridLayout;

    onDeckList = new MyQListWidget();
    onDutyList = new MyQListWidget();

    onDeckList->setStatusTip("The staff who are able to work on the selected day. Click to add a staff to the duty list. Dons are italisized, right click to make AM.");
    onDutyList->setStatusTip("The staff who are on duty for the selected day. Click to remove a staff from being on duty. Dons are italisized, the AM is bolded.");

    onDeckList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    onDutyList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    onDeckList->setMaximumWidth(180);
    onDutyList->setMaximumWidth(180);

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


void scheduleWidget::loadStaffTeamData(QString filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Duty Scheduler","Cannot read file.");
        return;
    }

    theTeam = new QList<staff*>;
    theExams = new QList<exam*>;

    QTextStream ts(&file);

    QString currentLine = "";

    QStringList current_Line;

    //IMPORT STAFF MEMBERS
    int id = 999;
    QString first = "";
    QString last = "";
    bool pos = false;
    bool gen = false;
    int night = 0;
    QString avail = "";
    QString exams = "";

    //IMPORT EXAMS
    QString date = "";
    bool e_night = false;


    bool ExamsFlag = false;//this flag tells us where we are in the text file.

    while(!ts.atEnd())
    {
        currentLine = ts.readLine();

        if (ts.atEnd())
            break;

        if (currentLine == "[STAFF]")
            continue;

        if (currentLine == "[EXAMS]")
        {
            ExamsFlag = true;
            continue;
        }

        //split the input line into an array of strings
        current_Line = currentLine.split(",", QString::SkipEmptyParts);

        staff *s;
        exam *e;

        if(!ExamsFlag)//looking at staff data
        {
            avail = "";
            exams = "";

            id = current_Line.at(0).toInt();
            first = current_Line.at(1);
            last = current_Line.at(2);

            if (current_Line.at(3) == "D")
                pos = true;
            else
                pos = false;

            if (current_Line.at(4) == "M")
                gen = true;
            else
                gen = false;

            night = current_Line.at(5).toInt();

            int y = 6;
            bool flag = true;

            while(flag)
            {

                if (y >= current_Line.size())
                {
                    flag = false;
                }
                else if(current_Line.at(y).startsWith("("))
                {
                    exams += current_Line.at(y) + ",";
                }
                else
                {
                    avail += current_Line.at(y) + ",";
                }

                y++;
            }

            s = new staff(id,first,last,pos,gen,night);
            s->setAvailability(avail);
            s->setExams(exams);

            theTeam->append(s);
        }
        else if(examSchedule) // we are now looking at exam data
        {
            id = current_Line.at(0).toInt();
            date = current_Line.at(1);

            if (current_Line.at(2) == "1")
                e_night = true;
            else
                e_night = false;

            e = new exam(id,date,e_night);

            theExams->append(e);
        }
    }
}

void scheduleWidget::prepInterface()
{

    onDeckItems = new QList<QListWidgetItem*>;
    onDutyItems = new QList<QListWidgetItem*>;

    QFont font;

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
            font = item->font();
            font.setItalic(true);
            item->setFont(font);
            item2->setFont(font);
        }


        // get the staff's night class availability
        if(!examSchedule)
        {
            int nights = theTeam->at(x)->getNightClass();
            int mask = 0x01;
            for (int y = 0; y<7; y++)
            {
                if(nights & mask)
                {   // add this staff member to the list of who's
                    // unavailable on this night
                    nightClasses[y]->append(theTeam->at(x)->getId());
                }
                mask *= 2;
            }
        }

        // get the staff's four dates of availabillity
        QString avail = theTeam->at(x)->getAvailability();
        QStringList availList = avail.split(",",QString::SkipEmptyParts);
        QDate date;
        for (int z = 0; z < availList.size(); z++)
        {
            date = date.fromString(availList.at(z),"dd/MM/yyyy");

            if(date.daysTo(*startDate) > 0 || date.daysTo(*endDate) < 0)
                continue;

            int index = dateToIndex(date);

            datesList->at(index)->addCantWork(theTeam->at(x)->getId());
        }

        if(examSchedule)
        {
            QStringList list = theTeam->at(x)->getExams().split(",",QString::SkipEmptyParts);

            for(int p = 0; p < list.count(); p++)
            {
                QString inp = list.at(p);

                inp.remove("(",Qt::CaseInsensitive);
                inp.remove(")",Qt::CaseInsensitive);
                int id = inp.toInt();

                //if(!theExams->contains(id))
                //    continue;

                QDate date;
                date = QDate::fromString(theExams->at(id)->getDate(),"dd/MM/yyyy");

                if(date.daysTo(*startDate) > 0 || date.daysTo(*endDate) < 0)
                    continue;

                int dateIndex = dateToIndex(date);

                if(theExams->at(id)->getNight())
                    datesList->at(dateIndex)->addCantWork(theTeam->at(x)->getId());

                if(dateIndex != 0)
                    datesList->at(dateIndex-1)->addCantWork(theTeam->at(x)->getId());

            }
        }
    }
}

int scheduleWidget::dateToIndex(QDate date)
{
    int index = startDate->daysTo(date);
    return index;
}

void scheduleWidget::dateClicked(QDate dateSelected)
{
    currentDateLabel->setText(QDate::shortDayName(dateSelected.dayOfWeek()) + " " + QDate::shortMonthName(dateSelected.month()) + " " + QString::number(dateSelected.day()));

    int dateIndex = dateToIndex(dateSelected); // get the index of the date in "datesList"
    int id;

    QListWidgetItem *deckItem;
    QListWidgetItem *dutyItem;

    for(int x = 0; x<theTeam->count(); x++)
    {
        deckItem = onDeckItems->at(x);
        dutyItem = onDutyItems->at(x);

        id = deckItem->data(Qt::UserRole).toInt();
        QFont font = dutyItem->font();
        //check who's on duty already
        if (datesList->at(dateIndex)->isOn(id))
        {


            if (datesList->at(dateIndex)->getAM() == id)
                font.setBold(true);
            else
                font.setBold(false);

            dutyItem->setFont(font);

            dutyItem->setHidden(false);
            deckItem->setHidden(true);

            continue;
        }
        font.setBold(false);
        dutyItem->setFont(font);
        dutyItem->setHidden(true); //so this person is not onduty.

        // check availabilities
        if (nightClasses[dateSelected.dayOfWeek()-1]->contains(id) || datesList->at(dateIndex)->staffCantWork(id))
        {
            deckItem->setHidden(true);
        }
        else
        {
            deckItem->setHidden(false);
        }
    }
    updateNeeded();
}

void scheduleWidget::addStaff(QListWidgetItem *item)
{

    int staffId = onDeckList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam->at(staffId)->getPosition();

    if (datesList->at(dateIndex)->isOn(staffId))
        return;

    if (datesList->at(dateIndex)->isSpecial())
        return;

    if(pos)// adding a don on
    {
        if (datesList->at(dateIndex)->donsFull())
            return;
    }
    else
    {
        if (datesList->at(dateIndex)->rasFull())
            return;
    }
    datesList->at(dateIndex)->addStaff(staffId, pos);

    int itemIndex = onDeckItems->indexOf(item);
    item->setHidden(true);
    onDutyItems->at(itemIndex)->setHidden(false);
    updateNeeded();

    //color the calendar if the date is full
    if(datesList->at(dateIndex)->isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }


    bool isWeekend = (datesList->at(dateIndex)->getDate().dayOfWeek() == 5 || datesList->at(dateIndex)->getDate().dayOfWeek() == 6);

    theTeam->at(staffId)->addShift(isWeekend, false);

    updateStats(false);
}

void scheduleWidget::removeStaff(QListWidgetItem *item)
{

    int staffId = onDutyList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());

    bool isAM = (staffId == datesList->at(dateIndex)->getAM());
    // remove the staff from the sDate object for the selected day.
    datesList->at(dateIndex)->removeStaff(staffId, theTeam->at(staffId)->getPosition());

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

    bool isWeekend = (datesList->at(dateIndex)->getDate().dayOfWeek() == 5 || datesList->at(dateIndex)->getDate().dayOfWeek() == 6);

    theTeam->at(staffId)->removeShift(isWeekend, isAM);

    updateStats(false);
}

void scheduleWidget::setAsAM()
{
    int staffId = onDeckList->currentItem()->data(Qt::UserRole).toInt();
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (!theTeam->at(staffId)->getPosition())
        return;

    if (datesList->at(dateIndex)->isOn(staffId))
        return;

    if (datesList->at(dateIndex)->getAM() != 999)
        return;

    if (datesList->at(dateIndex)->isSpecial())
        return;

    datesList->at(dateIndex)->setAM(staffId);


    onDeckList->currentItem()->setHidden(true);
    QListWidgetItem *item = onDutyItems->at(onDeckItems->indexOf(onDeckList->currentItem()));

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setHidden(false);


    //color the date on the calendar if it's full.
    if(datesList->at(dateIndex)->isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    bool isWeekend = (datesList->at(dateIndex)->getDate().dayOfWeek() == 5 || datesList->at(dateIndex)->getDate().dayOfWeek() == 6);

    theTeam->at(staffId)->addShift(isWeekend, true);

    updateStats(false);
}

void scheduleWidget::showMenu(QPoint p)
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

void scheduleWidget::setSpecialDuty()
{
    int index = dateToIndex(calendar->selectedDate());
    if(datesList->at(index)->isSpecial())
    {
        datesList->at(index)->setSpecial(false);
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::white);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }
    else
    {
        datesList->at(index)->setSpecial(true);
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::darkGray);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }


}

void scheduleWidget::updateNeeded()
{
    donsNeededLabel->setText(QString::number(datesList->at(dateToIndex(calendar->selectedDate()))->getDonsNeeded()));
    rasNeededLabel->setText(QString::number(datesList->at(dateToIndex(calendar->selectedDate()))->getRasNeeded()));
}

void scheduleWidget::showStaffSchedule()
{
    int staffId = teamStatsLabels->indexOf((MyQLabel*)QObject::sender());

    mySchedViewer *sv;
    sv = new mySchedViewer(theTeam->at(staffId)->getFirstName(),staffId,datesList);
    sv->setModal(false);
    sv->show();
}

void scheduleWidget::copySlot()
{
    copyAM = datesList->at(dateToIndex(calendar->selectedDate()))->getAM();

    for(int x = 0; x < onDutyItems->count(); x++)
    {
        if(!onDutyItems->at(x)->isHidden())
            copyList->append(onDutyItems->at(x)->data(Qt::UserRole).toInt());
    }

}

void scheduleWidget::pasteSlot()
{
    setAsAM(copyAM);
    for(int x = 0; x < copyList->count(); x++)
    {
        addStaff(copyList->at(x));
    }

    copyAM = 999;
    copyList->clear();
}


void scheduleWidget::setAsAM(int staffId)
{
    int dateIndex = dateToIndex(calendar->selectedDate());

    if (staffId == 999)
        return;

    if (!datesList->at(dateIndex)->canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1]->contains(staffId))
        return;

    if (!theTeam->at(staffId)->getPosition())
        return;

    if (datesList->at(dateIndex)->isOn(staffId))
        return;

    if (datesList->at(dateIndex)->getAM() != 999)
        return;

    if (datesList->at(dateIndex)->isSpecial())
        return;

    datesList->at(dateIndex)->setAM(staffId);


    onDeckItems->at(staffId)->setHidden(true);
    QListWidgetItem *item = onDutyItems->at(staffId);

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->setHidden(false);


    //color the date on the calendar if it's full.
    if(datesList->at(dateIndex)->isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    bool isWeekend = (datesList->at(dateIndex)->getDate().dayOfWeek() == 5 || datesList->at(dateIndex)->getDate().dayOfWeek() == 6);

    theTeam->at(staffId)->addShift(isWeekend, true);

    updateStats(false);
}


void scheduleWidget::addStaff(int staffId)
{

    int dateIndex = dateToIndex(calendar->selectedDate());
    bool pos = theTeam->at(staffId)->getPosition();

    if (!datesList->at(dateIndex)->canWork(staffId))
        return;

    if (nightClasses[calendar->selectedDate().dayOfWeek()-1]->contains(staffId))
        return;

    if (datesList->at(dateIndex)->isOn(staffId))
        return;

    if (datesList->at(dateIndex)->isSpecial())
        return;

    if(pos)// adding a don on
    {
        if (datesList->at(dateIndex)->donsFull())
            return;
    }
    else
    {
        if (datesList->at(dateIndex)->rasFull())
            return;
    }
    datesList->at(dateIndex)->addStaff(staffId, pos);

    onDeckItems->at(staffId)->setHidden(true);
    onDutyItems->at(staffId)->setHidden(false);
    updateNeeded();

    //color the calendar if the date is full
    if(datesList->at(dateIndex)->isFull())
    {
        QTextCharFormat format = calendar->dateTextFormat(calendar->selectedDate());
        format.setBackground(Qt::green);
        calendar->setDateTextFormat(calendar->selectedDate(),format);
    }

    //update the stats counters

    bool isWeekend = (datesList->at(dateIndex)->getDate().dayOfWeek() == 5 || datesList->at(dateIndex)->getDate().dayOfWeek() == 6);

    theTeam->at(staffId)->addShift(isWeekend, false);

    updateStats(false);
}

void scheduleWidget::exportSchedule()
{
    for(int checkFull = 0; checkFull < datesList->count(); checkFull++)             // IF there are some dates that aren't full issue a warning.
    {
        if(!datesList->at(checkFull)->isFull() && !datesList->at(checkFull)->isSpecial())
        {
            QMessageBox msgBox;
            msgBox.setText("Warning");
            msgBox.setInformativeText("Some dates of the schedule are not full. Do you want to export anyway?");
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

    //ts << "\"" + QDate::longMonthName(datesList->at(0)->getDate().month()) + "\"" << endl;

    QStringList writtenDates;
    writtenDates << "" << "" << "" << "" << "" << "" << "";
    int maxNeededForWeek = 0;
    int dateCounter = 0;

    //for(int y = 0; y < datesList->at(0)->getDate().dayOfWeek() - 1; y++)        // if the first day is in the middle of the week we need pre-padding
    //    lists.at(y)->append("\"\",");

    int numWeeks = endDate->weekNumber() - startDate->weekNumber() + 1;         // the number of weeks spanned by our schedule
    for(int weekCounter = 0; weekCounter < numWeeks; weekCounter++)
    {
        for(int dayOfWeekCounter = datesList->at(dateCounter)->getWeekday()-1; dayOfWeekCounter < 7; dayOfWeekCounter++)
        {
            QDate date = datesList->at(dateCounter)->getDate();
            writtenDates.replace(dayOfWeekCounter,"\"" + QDate::shortDayName(date.dayOfWeek()) + " " + QDate::shortMonthName(date.month()) + " " + QString::number(date.day()) + "\"");

            QString on = datesList->at(dateCounter)->exportOn();
            if(on == "special")
            {
                lists.at(dayOfWeekCounter)->append("Special Duty");
                continue;
            }
            *lists.at(dayOfWeekCounter) = on.split(",",QString::SkipEmptyParts);

            for(int z = 0; z < lists.at(dayOfWeekCounter)->count(); z++)             // swap the staff id's for their names in the string list
                lists.at(dayOfWeekCounter)->replace(z, "\"" + theTeam->at(lists.at(dayOfWeekCounter)->at(z).toInt())->getFirstName()
                                               + " " + theTeam->at(lists.at(dayOfWeekCounter)->at(z).toInt())->getLastName().left(1)+"\"");

            if(lists.at(dayOfWeekCounter)->count() > maxNeededForWeek)               // this tells us how many rows will be needed this week.
                maxNeededForWeek = lists.at(dayOfWeekCounter)->count();

            dateCounter++;
            if (dateCounter >= datesList->count())                                  // check this for accuracy
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

    //old export format
    /*ts << "\"Date\",\"AM\",\"Staff\"" << endl;

    for(int x = 0; x < datesList->count(); x++)
    {
        QStringList inp = datesList->at(x)->exportOn().split(".",QString::KeepEmptyParts);

        if (inp.count() != 3)
        {
            QString out = "\"" + datesList->at(x)->getDate().toString("dd/MM/yyyy") + "\",";
            ts << out << endl;
            continue;
        }

        int AM = inp.first().toInt();

        QStringList dons = inp.at(1).split(",",QString::SkipEmptyParts);

        QStringList ras = inp.at(2).split(",",QString::SkipEmptyParts);

        QString writeString = "\"" + datesList->at(x)->getDate().toString("dd/MM/yyyy") + "\",";

        writeString += "\"" + theTeam->at(AM)->getFirstName() + " " + theTeam->at(AM)->getLastName() + "\",";

        for (int y = 0; y < dons.count(); y++)
            writeString += "\"" + theTeam->at(dons.at(y).toInt())->getFirstName() + " " + theTeam->at(dons.at(y).toInt())->getLastName() + "\",";

        for (int z = 0; z < ras.count(); z++)
            writeString += "\"" + theTeam->at(ras.at(z).toInt())->getFirstName() + " " + theTeam->at(ras.at(z).toInt())->getLastName() + "\",";

        writeString.chop(1);

        ts << writeString << endl;
    }*/
    file.close();

}

void scheduleWidget::updateStats(bool inp)
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

    donAverageLabel->setText(QString::number(dAverage));
    raAverageLabel->setText(QString::number(rAverage));

    donAverageWeekendLabel->setText(QString::number(dWAverage));
    raAverageWeekendLabel->setText(QString::number(rWAverage));
}
