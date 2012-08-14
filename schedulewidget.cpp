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

scheduleWidget::scheduleWidget(QString fileName, QWidget *parent) // this constructor is used by LoadSchedule
    : QWidget(parent)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Open Schedule","Cannot open file.");
        return;
    }

    QTextStream ts(&file);

    QStringList inp = ts.readLine().split(":",QString::SkipEmptyParts);     //get start date
    QDate date = QDate::fromString(inp.at(1),"dd/MM/yyyy");
    startDate = new QDate();
    startDate->setDate(date.year(),date.month(),date.day());

    inp = ts.readLine().split(":",QString::SkipEmptyParts);                 //get end date
    date = QDate::fromString(inp.at(1),"dd/MM/yyyy");
    endDate = new QDate();
    endDate->setDate(date.year(),date.month(),date.day());

    inp = ts.readLine().split(":",QString::SkipEmptyParts);                 //get staff req for each night
    for(int x = 0; x < 7; x++)
    {
        donsNeeded[x] = inp.at(x).toInt();
        rasNeeded[x] = inp.at(x+7).toInt();
    }


    for(int q = 0; q<7; q++)                                                //get night classes
        nightClasses[q] = new QList<int>;

    for(int x = 0; x<7; x++)
    {
        inp = ts.readLine().split("-",QString::KeepEmptyParts);
        if (inp.at(1).isEmpty())
            continue;

        inp = inp.at(1).split(":", QString::SkipEmptyParts);

        for (int y = 0; y < inp.count(); y++)
            nightClasses[x]->append(inp.at(y).toInt());

    }


    theTeam = new QList<staff*>;                                            //get staff
    inp = ts.readLine().split(":",QString::SkipEmptyParts);
    int count = inp.at(1).toInt();

    for(int x = 0; x < count; x++)
    {
        inp = ts.readLine().split(":",QString::SkipEmptyParts);
        staff *st = new staff();

        st->setId(inp.at(0).toInt());
        st->setFirstName(inp.at(1));
        st->setLastName(inp.at(2));

        if(inp.at(3) == "D")
            st->setPosition(true);
        else
            st->setPosition(false);

        theTeam->append(st);
    }

    datesList = new QList<sDate*>;                                          //make the sdate objects
    QDate dateCounter;
    sDate *sDateIterator;
    dateCounter.setDate(startDate->year(),startDate->month(),startDate->day());
    int length = startDate->daysTo(*endDate) + 1;

    for(int z = 0; z<length; z++)
    {
        sDateIterator = new sDate(dateCounter,donsNeeded[dateCounter.dayOfWeek()-1],rasNeeded[dateCounter.dayOfWeek()-1]);
        datesList->append(sDateIterator);
        dateCounter = dateCounter.addDays(1);
    }

    for(int x = 0; x < datesList->count(); x++)                           //fill the dates
    {
        inp = ts.readLine().split(":",QString::KeepEmptyParts);

        if (inp.at(1) == "Y")
        {
            datesList->at(x)->setSpecial(true);
            continue;
        }

        bool weekend = (datesList->at(x)->getDate().dayOfWeek() == 5 || datesList->at(x)->getDate().dayOfWeek() == 6);

        QStringList unavail = inp.at(2).split(",", QString::SkipEmptyParts);
        for(int z = 0; z < unavail.count(); z++)
            datesList->at(x)->addCantWork(unavail.at(z).toInt());

        datesList->at(x)->setAM(inp.at(3).toInt());             //add the staff to the date object
        if (inp.at(3).toInt() < theTeam->count())
            theTeam->at(inp.at(3).toInt())->addShift(weekend, true);    //add the shift count to the staff object

        QStringList dons_list = inp.at(4).split(",", QString::SkipEmptyParts);
        for(int d = 0; d < dons_list.count(); d++)
        {
            datesList->at(x)->addStaff(dons_list.at(d).toInt(), true);      //add staff id to date obj
            theTeam->at(dons_list.at(d).toInt())->addShift(weekend,false);  //add date to staff obj
        }

        QStringList ras_list = inp.at(5).split(",", QString::SkipEmptyParts);
        for(int r = 0; r < ras_list.count(); r++)
        {
            datesList->at(x)->addStaff(ras_list.at(r).toInt(), false);      //add staff id to date obj
            theTeam->at(ras_list.at(r).toInt())->addShift(weekend,false);   // add date to staff obj
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
    for(int x = 0; x < datesList->count(); x++)
    {
        if (datesList->at(x)->isFull())
        {
            QTextCharFormat format = calendar->dateTextFormat(datesList->at(x)->getDate());
            format.setBackground(Qt::green);
            calendar->setDateTextFormat(datesList->at(x)->getDate(),format);
        }
        if (datesList->at(x)->isSpecial())
        {
            QTextCharFormat format = calendar->dateTextFormat(datesList->at(x)->getDate());
            format.setBackground(Qt::darkGray);
            calendar->setDateTextFormat(datesList->at(x)->getDate(),format);
        }
    }
    dateClicked(*startDate);
    updateStats();

}

scheduleWidget::~scheduleWidget()
{
    QList<QTableWidgetItem*>::iterator it_i = statsTableItems->begin();

    for (; it_i != statsTableItems->end();)
    {
        delete *it_i;
        it_i = statsTableItems->erase(it_i);
    }
    delete statsTableItems;


    QList<staff*>::iterator it_s = theTeam->begin();
    for(; it_s != theTeam->end(); )
    {
        delete *it_s;
        it_s = theTeam->erase(it_s);
    }
    delete theTeam;

    QList<exam*>::iterator it_e = theExams->begin();
    for(; it_e != theExams->end(); )
    {
        delete *it_e;
        it_e = theExams->erase(it_e);
    }
    delete theExams;

    delete startDate;
    delete endDate;

    for(int x = 0; x < 7; x++)
    {
        nightClasses[x]->clear();
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


    QList<sDate*>::iterator it_d = datesList->begin();
    for(; it_d != datesList->end();)
    {
        delete *it_d;
        it_d = datesList->erase(it_d);
    }
    delete datesList;

    delete setAsAMAction;

    copyList->clear();
    delete copyList;



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
    spDutyFlagButton = new QPushButton("Set date as Special Duty");
    copyButton = new QPushButton("Copy On Duty");
    pasteButton = new QPushButton("Paste On Duty");
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

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(averagesTable,0,0,1,5);

    layout->addWidget(statsTable,1,0,1,5);

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

    updateStats();
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

    updateStats();
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

    updateStats();
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

void scheduleWidget::showStaffSchedule(QTableWidgetItem *item)
{
    if(item->column() != 0)
        return;

    int staffId = item->data(Qt::UserRole).toInt();
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

    updateStats();
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

    updateStats();
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

void scheduleWidget::updateStats()
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

}

void scheduleWidget::saveMidSchedule(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Save Schedule","Cannot write file.");
        return;
    }

    QTextStream ts(&file);

    ts << "start:" << startDate->toString("dd/MM/yyyy") << endl;
    ts << "end:" << endDate->toString("dd/MM/yyyy") << endl;

    //x:y:z:a:b:c:d:1:2:3:4:5:6:7
    for(int d = 0; d < 7; d++)
        ts << QString::number(donsNeeded[d]) << ":";
    for(int r = 0; r < 7; r++)
        ts << QString::number(rasNeeded[r]) << ":";
    ts << endl;


    //a line for each day of the week with the staff who can't work ids. Monday first.
    //1-id:id:id:
    //2-id:id:
    // etc
    for(int n = 0; n<7; n++)
    {
        ts << QString::number(n) << "-";
        for(int i = 0; i < nightClasses[n]->count(); i++)
        {
            ts << QString::number(nightClasses[n]->at(i)) << ":";
        }
        ts << endl;
    }

    ts << "#staff:" << QString::number(theTeam->count()) << endl;
    // ID:firstname:lastname:position
    for(int x = 0; x < theTeam->count(); x++)
    {
        ts << QString::number(theTeam->at(x)->getId()) + ":"
              + theTeam->at(x)->getFirstName() + ":"
              + theTeam->at(x)->getLastName() + ":";

        if(theTeam->at(x)->getPosition())
            ts << "D" << endl;
        else
            ts << "R" << endl;
    }


    // dd/MM/yyyy:isSpecial:cantwork,cantwork,cantwork:AM:don,don,don,don:ra,ra,ra,ra,ra
    for(int x = 0; x < datesList->count(); x++)
    {
        ts << datesList->at(x)->getDate().toString("dd/MM/yyyy") << ":";

        if(datesList->at(x)->isSpecial())
            ts << "Y:";
        else
            ts << "N:";

        ts << datesList->at(x)->getCantWork() << ":";
        ts << QString::number(datesList->at(x)->getAM()) << ":";
        ts << datesList->at(x)->getDons() << ":";
        ts << datesList->at(x)->getRas() << endl;

    }

    file.close();

}


