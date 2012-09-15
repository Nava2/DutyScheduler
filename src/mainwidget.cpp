#include <QtGui>
#include <QtGlobal>

#include "mainwidget.h"
#include "stafflist.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    createStaffElements();
}

MainWidget::~MainWidget()
{
    theTeam.clear();
    finalExams.clear();
    midtermExams.clear();

    staffTeamList->clear();

    delete availWidget;

    delete staffTeamList;

    delete examWidget;

}

//SLOTS
void MainWidget::updateStaffMember()
{
    if(firstNameEdit->text() == "" || lastNameEdit->text() == "")
        return;

    bool g = maleRadio->isChecked(); // gender

    bool p = donRadio->isChecked();

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
    // done in one call later..

    //EXAMS
    QList<Exam::Ptr > exams;
    examWidget->getFinals(exams);

    QListWidgetItem *i = staffTeamList->currentItem();//get the list item from the list widget
    QString id = i->data(Qt::UserRole).toString();//the list item's user data is the staff id
    i->setText(firstNameEdit->text() + " " + lastNameEdit->text()); //change the text in the list
    Staff::Ptr pstaff = theTeam.at(id);
    pstaff->update(firstNameEdit->text().trimmed(),lastNameEdit->text().trimmed(),p,g,n); //change the actual staff object
    pstaff->setAvailability(availWidget->getAvail());// set the avail
    pstaff->setFinals(exams);//set exams
    clearSelections();
}

void MainWidget::addStaffMember()
{
    if(firstNameEdit->text() == "" || lastNameEdit->text() == "")
        return;

    bool g = maleRadio->isChecked(); // gender

    bool p = donRadio->isChecked(); // position

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
    Staff::Ptr s(new Staff(theTeam.count(), //make a staff member pointer
                            firstNameEdit->text().trimmed(),
                            lastNameEdit->text().trimmed(), p, g, n));
    //AVAILABILITY
    s->setAvailability(availWidget->getAvail());

    //EXAMS
    QList<Exam::Ptr > exams;
    examWidget->getFinals(exams);

    s->setFinals(exams);

    // DATA STUFF
    theTeam.append(s); // add this staff to the team qlist

    QListWidgetItem *item = new QListWidgetItem; // this item represents the staff in the visible list widget

    item->setText(s->getFirstName() + " " + s->getLastName()); // set the text of the list item

    item->setData(Qt::UserRole, s->uid());//attach the staff's id number to the list item for later use.

    staffTeamList->insertItem(0,item);

    clearSelections();
}

void MainWidget::removeStaffMember()
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

void MainWidget::clearSelections()
{
    firstNameEdit->setText("");
    lastNameEdit->setText("");

    raRadio->setChecked(true);

    femaleRadio->setChecked(true);

    for(int x=0; x<7; x++)
        nightClassCheckBoxes[x]->setChecked(false);

    availWidget->reset();

    examWidget->reset();
}

void MainWidget::updateSelections(QListWidgetItem * item)
{
    clearSelections();
    //get the id of the selected staff member
    QString uid = item->data(Qt::UserRole).toString();
    Staff::Ptr pstaff = theTeam.at(uid);

    //now update the right hand stuff so that it matches the selected staff
    firstNameEdit->setText(pstaff->getFirstName());
    lastNameEdit->setText(pstaff->getLastName());

    donRadio->setChecked(pstaff->getPosition());
    maleRadio->setChecked(pstaff->getGender());

    //Night Classes
    int nights = pstaff->getNightClass();
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

    // QQQ

    QList<AvailableDate > avail;
    pstaff->getAvailability(avail);

    availWidget->setToAvail(avail);

    //EXAMS
    examWidget->setFinals(pstaff->getFinals());
}

void MainWidget::addFinal(const Exam::Ptr e) {
    finalExams.append(e);//adds the pointer to the exam in the main exams list for the team
}

void MainWidget::removeFinal(const Exam::Ptr e)
{
    // nothing to do yet?
}

void MainWidget::addMidterm(const Exam::Ptr e) {
    midtermExams.append(e);
}

void MainWidget::removeMidterm(const Exam::Ptr e)
{
    // nothing to do yet?
}

// GUI STUFF
void MainWidget::createStaffElements()
{
    // call the functions to set up all our widgets
    createPositionGroupBox();
    createGenderGroupBox();
    createNightClassGroupBox();
    createStaffControls();
    createNameGroupBox();

    //list to display the staff
    staffTeamList = new QListWidget;
    staffTeamList->setSortingEnabled(true);//The list should sort automatically
    staffTeamList->setSelectionMode(QAbstractItemView::SingleSelection);
    staffTeamList->setStatusTip("The list of staff members of this team.");
    // connect so that when the user changes the selection in the list the right hand side updates.
    connect(staffTeamList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(updateSelections(QListWidgetItem*)));

    // availability widget:
    availWidget = new AvailabilityWidget;

    // exams widget:
    examWidget = new ExamWidget(finalExams, midtermExams, this);

    //set up the layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(staffTeamList,0,0,6,1);
    layout->addWidget(controlsGroupBox,0,1,6,1);
    layout->addWidget(nameGroupBox,0,2,1,2);
    layout->addWidget(positionGroupBox,1,2,1,1);
    layout->addWidget(genderGroupBox,1,3,1,1);
    layout->addWidget(nightClassGroupBox,2,2,1,2);
    layout->addWidget(availWidget,3,2,1,2);
    layout->addWidget(examWidget,4,2,1,2);
    setLayout(layout);

    setWindowTitle("Duty Schedule Creation Aid");
}

void MainWidget::createNameGroupBox()
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

void MainWidget::createPositionGroupBox()
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

void MainWidget::createGenderGroupBox()
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

void MainWidget::createNightClassGroupBox()
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

void MainWidget::createStaffControls()
{
    controlsGroupBox = new QGroupBox();

    addStaffButton = new QPushButton(tr("<< ADD STAFF"));
    updateStaffButton = new QPushButton(tr("< UPDATE STAFF"));
    removeStaffButton = new QPushButton(tr("REMOVE STAFF"));
    clearButton = new QPushButton(tr("CLEAR INPUTS"));

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
void MainWidget::reset()
{
    theTeam.clear();

    finalExams.clear();
    midtermExams.clear();

    staffTeamList->clear();

    examWidget->reset();
}

StaffList MainWidget::getStaff()
{
    return theTeam;
}

void MainWidget::getExams(QList<Exam::Ptr> &fOut, QList<Exam::Ptr> &mOut) {
    fOut = finalExams;
    mOut = midtermExams;
}

QList<QString > MainWidget::getUIDs() {
    QList<QString > out;

    for (int i = 0; i < staffTeamList->count(); i++) {
        out.append(staffTeamList->item(i)->data(Qt::UserRole).toString());
    }

    return out;
}

QString MainWidget::getTeam()
{
    QString teamList = "";

    QList<QString > ids = getUIDs();

    foreach(QString id, ids)
    {
        teamList += id + ",";
    }

    return teamList;
}

void MainWidget::load(const StaffList &staffList, const QList<Exam::Ptr> &finalList, const QList<Exam::Ptr> &midtermList)
{
    theTeam = staffList;
    finalExams = finalList;
    midtermExams = midtermList;

    for(int x=0; x<theTeam.count(); x++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(theTeam[x]->getFirstName() + " " + theTeam[x]->getLastName());
        item->setData(Qt::UserRole, theTeam[x]->uid());
        staffTeamList->insertItem(0,item);
    }

}



