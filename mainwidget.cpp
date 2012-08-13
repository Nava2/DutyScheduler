#include <QtGui>
#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
{
    theTeam = new QList<staff*>;
    theExams = new QList<exam*>;
    createStaffElements();
}

mainWidget::~mainWidget()
{
    while (!theTeam->isEmpty())
    {
        staff *s = theTeam->takeLast();
        delete s;
    }
    delete theTeam;

    while (!theExams->isEmpty())
    {
        exam *e = theExams->takeLast();
        delete e;
    }
    delete theExams;
}

//SLOTS
void mainWidget::updateStaffMember()
{
    if(firstNameEdit->text() == "" || lastNameEdit->text() == "")
        return;

    /*QMessageBox msgBox;
    msgBox.setText("Are you sure?");
    msgBox.setInformativeText("Do you want to update " + staffTeamList->currentItem()->text() + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int msgbox_ret = msgBox.exec();

    if (msgbox_ret == 0x10000)
        return;*/

    bool g;//gender
    if(maleRadio->isChecked())
    {    g = true;  }
    if(femaleRadio->isChecked())
    {    g = false; }


    bool p;//position
    if(donRadio->isChecked())
    {    p = true;  }
    if(raRadio->isChecked())
    {    p = false; }

    int n = 0;//night class indicator
    int mask = 1;
    //each bit of the char is a 'bool', true means there is a night class.
    for(int x = 0; x<7; x++)
    {
        if(nightClassCheckBoxes[x]->isChecked())
            n = (n | mask);

        mask = (mask << 1);
    }

    //AVAILABILITY
    QString a = "";
    //add availability to the staff object
    for (int y = 0; y<4; y++)
    {
        if (arrayGroupBox[y]->isChecked())
            a += arrayDateEdit[y]->date().toString("dd/MM/yyyy,");

    }

    //EXAMS
    QString e = "";
    QListWidgetItem *i_e;
    for (int x = 0; x < examsList->count(); x++)
    {
        i_e = examsList->item(x);
        e += "(" + i_e->data(Qt::UserRole).toString() + "),";
    }

    QListWidgetItem *i = staffTeamList->currentItem();//get the list item from the list widget
    int id = i->data(Qt::UserRole).toInt();//the list item's user data is the staff id
    i->setText(firstNameEdit->text() + " " + lastNameEdit->text()); //change the text in the list
    theTeam->at(id)->update(firstNameEdit->text().trimmed(),lastNameEdit->text().trimmed(),p,g,n); //change the actual staff object
    theTeam->at(id)->setAvailability(a);// set the avail
    theTeam->at(id)->setExams(e);//set exams
    clearSelections();
}

void mainWidget::addStaffMember()
{
    if(firstNameEdit->text() == "" || lastNameEdit->text() == "")
        return;

    bool g;//gender
    if(maleRadio->isChecked())
    {    g = true;  }
    if(femaleRadio->isChecked())
    {    g = false; }


    bool p;//position
    if(donRadio->isChecked())
    {    p = true;  }
    if(raRadio->isChecked())
    {    p = false; }

    int n = 0;//night class indicator
    int mask = 1;
    //each bit of the char is a 'bool', true means there is a night class.
    for(int x = 0; x<7; x++)
    {
        if(nightClassCheckBoxes[x]->isChecked())
            n = (n | mask);

        mask = (mask << 1);
    }

    // MAKE THE STAFF
    staff *s;//make a staff member pointer
    s = new staff(theTeam->size(),firstNameEdit->text().trimmed(),lastNameEdit->text().trimmed(),p,g,n);

    //AVAILABILITY
    QString a = "";
    for (int y = 0; y<4; y++)
    {
        if (arrayGroupBox[y]->isChecked())
            a += arrayDateEdit[y]->date().toString("dd/MM/yyyy,");

    }
    s->setAvailability(a);

    //EXAMS
    QString e = "";
    QListWidgetItem *i;
    for (int x = 0; x < examsList->count(); x++)
    {
        i = examsList->item(x);
        e += "(" + i->data(Qt::UserRole).toString() + "),";
    }

    s->setExams(e);

    // DATA STUFF
    theTeam->append(s); // add this staff to the team qlist

    QListWidgetItem *item = new QListWidgetItem; // this item represents the staff in the visible list widget

    item->setText(s->getFirstName() + " " + s->getLastName()); // set the text of the list item

    item->setData(Qt::UserRole, s->getId());//attach the staff's id number to the list item for later use.

    staffTeamList->insertItem(0,item);

    clearSelections();
}

void mainWidget::removeStaffMember()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure?");
    msgBox.setInformativeText("Do you want to remove " + staffTeamList->currentItem()->text() + "?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int msgbox_ret = msgBox.exec();

    if (msgbox_ret == 0x10000)
        return;

    QListWidgetItem *i;
    i = staffTeamList->takeItem(staffTeamList->currentRow());
    delete i;
}

void mainWidget::clearSelections()
{
    firstNameEdit->setText("");
    lastNameEdit->setText("");

    donRadio->setChecked(false);
    raRadio->setChecked(true);

    maleRadio->setChecked(false);
    femaleRadio->setChecked(true);

    for(int x=0; x<7; x++)
        nightClassCheckBoxes[x]->setChecked(false);

    for(int y=0; y<4; y++)
    {
        arrayDateEdit[y]->setDate(QDate::currentDate());
        arrayGroupBox[y]->setChecked(false);
    }

    examsList->clear();
    examDateEdit->setDate(QDate::currentDate());
    examNightCheck->setChecked(false);
}

void mainWidget::updateSelections()
{
    clearSelections();
    //get the id of the selected staff member
    int id = staffTeamList->currentItem()->data(Qt::UserRole).toInt();

    //now update the right hand stuff so that it matches the selected staff
    firstNameEdit->setText(theTeam->at(id)->getFirstName());
    lastNameEdit->setText(theTeam->at(id)->getLastName());

    if (theTeam->at(id)->getPosition())
        donRadio->setChecked(true);
    else
        raRadio->setChecked(true);

    if (theTeam->at(id)->getGender())
        maleRadio->setChecked(true);
    else
        femaleRadio->setChecked(true);

    //Night Classes
    int nights = theTeam->at(id)->getNightClass();
    int mask = 0x01;
    for (int x = 0; x<7; x++)
    {
        if(nights & mask)
            nightClassCheckBoxes[x]->setChecked(true);
        else
            nightClassCheckBoxes[x]->setChecked(false);

        mask *= 2;
    }

    //Availability

    QStringList avail = theTeam->at(id)->getAvailability().split(',',QString::SkipEmptyParts);

    QDate d;
    for(int y = 0; y<4; y++)
    {
        if (avail.isEmpty())
        {
            arrayGroupBox[y]->setChecked(false);
        }
        else
        {
            arrayGroupBox[y]->setChecked(true);
            d = QDate::fromString(avail.takeFirst(),"dd/MM/yyyy");
            arrayDateEdit[y]->setDate(d);
        }
    }


    //EXAMS
    QString e = theTeam->at(id)->getExams();
    QStringList exams = e.split(',',QString::SkipEmptyParts);
    int exams_int[exams.size()];

    for (int z = 0; z < exams.size(); z++)
    {
        QString temp = "";
        temp = exams.at(z);
        temp.remove("(", Qt::CaseInsensitive);
        temp.remove(")", Qt::CaseInsensitive);
        exams_int[z] = temp.toInt();
    }

    QListWidgetItem *i;
    for (int a = 0; a < exams.size(); a++)
    {

        QString text = "";
        if(theExams->at(exams_int[a])->getNight())
        {
            text = theExams->at(exams_int[a])->getDate() + " (night)";
        }
        else
        {
            text = theExams->at(exams_int[a])->getDate();
        }
        i = new QListWidgetItem(text);
        i->setData(Qt::UserRole,exams_int[a]);
        examsList->insertItem(0,i);
    }

}

void mainWidget::addExam()
{
    exam *e = new exam(theExams->count(),examDateEdit->date().toString("dd/MM/yyyy"),examNightCheck->isChecked());

    QListWidgetItem *item = new QListWidgetItem();

    if (examNightCheck->isChecked())
        item->setText(e->getDate() + " (night)");
    else
        item->setText(e->getDate());

    item->setData(Qt::UserRole,e->getId());

    theExams->append(e);//adds the pointer to the exam in the main exams list for the team
    examsList->insertItem(0,item);//adds the list item to the small exams list

}

void mainWidget::removeExam()
{
    QListWidgetItem *i;
    i = examsList->takeItem(examsList->currentRow());
    delete i;
}







// GUI STUFF
void mainWidget::createStaffElements()
{
    // call the functions to set up all our widgets
    createPositionGroupBox();
    createGenderGroupBox();
    createNightClassGroupBox();
    createAvailabilityGroupBox();
    createExamScheduleGroupBox();
    createStaffControls();
    createNameGroupBox();

    //list to display the staff
    staffTeamList = new QListWidget;
    staffTeamList->setSortingEnabled(true);//The list should sort automatically
    staffTeamList->setSelectionMode(QAbstractItemView::SingleSelection);
    staffTeamList->setStatusTip("The list of staff members of this team.");
    // connect so that when the user changes the selection in the list the right hand side updates.
    connect(staffTeamList,SIGNAL(itemSelectionChanged()),this,SLOT(updateSelections()));
    //connect(staffTeamList, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(updateSelections()));

    //set up the layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(staffTeamList,0,0,6,1);
    layout->addWidget(controlsGroupBox,0,1,6,1);
    layout->addWidget(nameGroupBox,0,2,1,2);
    layout->addWidget(positionGroupBox,1,2,1,1);
    layout->addWidget(genderGroupBox,1,3,1,1);
    layout->addWidget(nightClassGroupBox,2,2,1,2);
    layout->addWidget(availabilityGroupBox,3,2,1,2);
    layout->addWidget(examScheduleGroupBox,4,2,1,2);
    setLayout(layout);

    setWindowTitle("Duty Schedule Creation Aid");
}

void mainWidget::createNameGroupBox()
{
    nameGroupBox = new QGroupBox;
    firstNameLabel = new QLabel("First Name:");
    lastNameLabel = new QLabel("Last Name:");
    firstNameEdit = new QLineEdit;
    lastNameEdit = new QLineEdit;

    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(firstNameLabel);
    layout->addWidget(firstNameEdit);
    layout->addWidget(lastNameLabel);
    layout->addWidget(lastNameEdit);

    nameGroupBox->setLayout(layout);
}

void mainWidget::createPositionGroupBox()
{
    positionGroupBox = new QGroupBox(tr("Position"));
    donRadio = new QRadioButton("Don");
    raRadio = new QRadioButton("RA");
    raRadio->setChecked(true);

    QHBoxLayout *positionLayout = new QHBoxLayout;
    positionLayout->addWidget(donRadio);
    positionLayout->addWidget(raRadio);

    positionGroupBox->setLayout(positionLayout);
}

void mainWidget::createGenderGroupBox()
{
    genderGroupBox = new QGroupBox(tr("Gender"));
    maleRadio = new QRadioButton(tr("male"));
    femaleRadio = new QRadioButton(tr("female"));
    femaleRadio->setChecked(true);

    QHBoxLayout *genderLayout = new QHBoxLayout;
    genderLayout->addWidget(maleRadio);
    genderLayout->addWidget(femaleRadio);

    genderGroupBox->setLayout(genderLayout);
}

void mainWidget::createNightClassGroupBox()
{

    nightClassGroupBox = new QGroupBox(tr("Night Class"));
    nightClassGroupBox->setStatusTip("Check off the nights of the week when this staff member has class (or other recurring commitments).");
    QString day[] = {"Mon", "Tue", "Wed", "Thu", "FRI", "SAT", "Sun"};
    QHBoxLayout *nightClassLayout = new QHBoxLayout;

    for(int x = 0; x<7; x++)
    {
        nightClassCheckBoxes[x] = new QCheckBox(day[x]);
        nightClassLayout->addWidget(nightClassCheckBoxes[x]);
    }
    nightClassGroupBox->setLayout(nightClassLayout);

}

void mainWidget::createAvailabilityGroupBox()
{
    availabilityGroupBox = new QGroupBox(tr("Dates Unavailable"));
    availabilityGroupBox->setStatusTip("These are the dates that the selected staff member is unavailable. ex. A wedding, academic commitment, etc.");

    QGridLayout *availabilityLayout = new QGridLayout;
    QGridLayout *subLayout;
    for(int x = 0; x<4; x++)
    {
        arrayDateEdit[x] = new QDateEdit;
        arrayGroupBox[x] = new QGroupBox;

        arrayDateEdit[x]->setCalendarPopup(true);
        arrayDateEdit[x]->setDate(QDate::currentDate());

        arrayGroupBox[x]->setCheckable(true);
        arrayGroupBox[x]->setChecked(false);

        subLayout = new QGridLayout;

        subLayout->addWidget(arrayDateEdit[x]);
        arrayGroupBox[x]->setLayout(subLayout);

        availabilityLayout->addWidget(arrayGroupBox[x],((x>>1) & 0x01), (x & 0x01));

    }

    availabilityGroupBox->setLayout(availabilityLayout);
}

void mainWidget::createExamScheduleGroupBox()
{
    examScheduleGroupBox = new QGroupBox(tr("Exams"));
    examsList = new QListWidget;
    examDateEdit = new QDateEdit;
    examNightCheck = new QCheckBox(tr("Night Exam"));
    addExamButton = new QPushButton(tr("ADD"));
    removeExamButton = new QPushButton(tr("REMOVE"));
    examDateEdit->setDate(QDate::currentDate());
    examDateEdit->setCalendarPopup(true);

    examsList->setStatusTip("The list of exams for the selected staff member.");


    connect(addExamButton,SIGNAL(clicked()),this,SLOT(addExam()));
    connect(removeExamButton,SIGNAL(clicked()),this,SLOT(removeExam()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(examsList,0,0,4,1);
    layout->addWidget(examDateEdit,0,1);
    layout->addWidget(examNightCheck,1,1);
    layout->addWidget(addExamButton,2,1);
    layout->addWidget(removeExamButton,3,1);

    examScheduleGroupBox->setLayout(layout);
}

void mainWidget::createStaffControls()
{
    controlsGroupBox = new QGroupBox();

    addStaffButton = new QPushButton(tr("<< ADD"));
    updateStaffButton = new QPushButton(tr("< UPDATE"));
    removeStaffButton = new QPushButton(tr("REMOVE"));
    clearButton = new QPushButton(tr("CLEAR"));

    addStaffButton->setStatusTip("Take the data currently on the right and add this staff to the list.");
    updateStaffButton->setStatusTip("Change the data for the selected staff member to whatever is entered on the right.");
    removeStaffButton->setStatusTip("Delete the selected staff member from the team list.");
    clearButton->setStatusTip("Clear the input fields on the right.");

    connect(addStaffButton, SIGNAL(clicked()), this, SLOT(addStaffMember()));
    connect(updateStaffButton, SIGNAL(clicked()), this, SLOT(updateStaffMember()));
    connect(removeStaffButton, SIGNAL(clicked()), this, SLOT(removeStaffMember()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearSelections()));

    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(addStaffButton);
    controlsLayout->addWidget(updateStaffButton);
    controlsLayout->addWidget(removeStaffButton);
    controlsLayout->addWidget(clearButton);
    controlsGroupBox->setLayout(controlsLayout);
}


// GETTERS/SETTERS for staff and exams
void mainWidget::reset()
{


    QList<staff *>::iterator it_s = theTeam->begin();
    for(; it_s != theTeam->end(); )
    {
        delete *it_s;
        it_s = theTeam->erase(it_s);
    }

    QList<exam *>::iterator it_e = theExams->begin();
    for(; it_e != theExams->end(); )
    {
        delete *it_e;
        it_e = theExams->erase(it_e);
    }


    delete theTeam;
    delete theExams;

    theTeam = new QList<staff*>;
    theExams = new QList<exam*>;

    staffTeamList->clear();
}

QList<staff*> * mainWidget::getStaff()
{
    return theTeam;
}

QList<exam*> * mainWidget::getExams()
{
    return theExams;
}

QString mainWidget::getTeam()
{
    QString teamList = "";

    for (int x = 0; x < staffTeamList->count(); x++)
    {
        teamList = teamList + staffTeamList->item(x)->data(Qt::UserRole).toString() + ",";
    }

    return teamList;
}

void mainWidget::load(QList<staff*> *staffList, QList<exam*> *examList)
{
    theTeam = staffList;
    theExams = examList;

    for(int x=0; x<theTeam->size(); x++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(theTeam->at(x)->getFirstName() + " " + theTeam->at(x)->getLastName());
        item->setData(Qt::UserRole, theTeam->at(x)->getId());
        staffTeamList->insertItem(0,item);
    }

}



