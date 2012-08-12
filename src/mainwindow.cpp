#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentStaffTeamFile(""), usingStaffTeamFile("")
{
    createActions();
    createMenu();
    (void)statusBar();

    m = new mainWidget();

    setCentralWidget(m);

}

MainWindow::~MainWindow()
{
    
}

void MainWindow::createActions()
{
    newStaffTeamAct = new QAction("New Team", this);
    newStaffTeamAct->setStatusTip("Create a new staff team");
    connect(newStaffTeamAct, SIGNAL(triggered()),this, SLOT(newStaffTeam()));

    openStaffTeamAct = new QAction("Open Team", this);
    openStaffTeamAct->setStatusTip("Open an existing staff team");
    connect(openStaffTeamAct, SIGNAL(triggered()),this, SLOT(openStaffTeam()));

    saveStaffTeamAct = new QAction("Save Team", this);
    saveStaffTeamAct->setStatusTip("Save the opened staff team");
    connect(saveStaffTeamAct, SIGNAL(triggered()),this, SLOT(saveStaffTeam()));

    saveAsStaffTeamAct = new QAction("Save Team As", this);
    saveAsStaffTeamAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsStaffTeamAct, SIGNAL(triggered()), this, SLOT(saveAsStaffTeam()));

    newScheduleAct = new QAction("New Schedule", this);
    newScheduleAct->setStatusTip("Create a new duty schedule");
    connect(newScheduleAct, SIGNAL(triggered()),this, SLOT(newSchedule()));

    openScheduleAct = new QAction("Open Schedule", this);
    openScheduleAct->setStatusTip("Open an existing schedule");
    connect(openScheduleAct, SIGNAL(triggered()),this, SLOT(openSchedule()));

    aboutAct = new QAction("Help Me!", this);
    connect(aboutAct, SIGNAL(triggered()),this, SLOT(about()));

}

void MainWindow::createMenu()
{
    staffTeamMenu = menuBar()->addMenu("Staff Team");
    staffTeamMenu->addAction(newStaffTeamAct);
    staffTeamMenu->addAction(openStaffTeamAct);
    staffTeamMenu->addAction(saveStaffTeamAct);
    staffTeamMenu->addAction(saveAsStaffTeamAct);

    scheduleMenu = menuBar()->addMenu("Schedule");
    scheduleMenu->addAction(newScheduleAct);
    //scheduleMenu->addAction(openScheduleAct);

    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}

//MENU SLOTS

void MainWindow::newStaffTeam()
{
    m->reset();
    currentStaffTeamFile = "";
}

void MainWindow::openStaffTeam()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (!fileName.isEmpty())
        MainWindow::loadStaffTeamFile(fileName);

    if(fileName.right(4) != ".txt")
    {
        QMessageBox::warning(this, "Open Staff Team","Incorrect File, must have extention '.txt'.");
        return;
    }

}

void MainWindow::saveStaffTeam()
{
    if (currentStaffTeamFile.isEmpty())
        MainWindow::saveAsStaffTeam();
    else
        MainWindow::saveStaffTeamFile(currentStaffTeamFile);
}

void MainWindow::saveAsStaffTeam()
{
    QString fileName = QFileDialog::getSaveFileName(this);

    if (fileName.isEmpty())
        return;

    if(fileName.right(4) != ".txt")
    {
        QMessageBox::warning(this, "Save Staff Team","Incorrect File, must have extention '.txt'.");
        return;
    }

    MainWindow::saveStaffTeamFile(fileName);
}


void MainWindow::newSchedule()
{
    QMessageBox msgBox;
    msgBox.setText("Any unsaved staff team data will be lost. Continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int msgbox_ret = msgBox.exec();

    if (msgbox_ret == 0x10000)
        return;

    scheduleWizzard sw;
    int ret = sw.exec();
    //qDebug() << QString::number(ret);
    if (ret == 0)
        return;

    QMessageBox msgBox2;
    msgBox2.setText("New Schedule");
    msgBox2.setInformativeText("Select the staff team to use for your new schedule.");
    msgBox2.setStandardButtons(QMessageBox::Ok);
    msgBox2.setDefaultButton(QMessageBox::Ok);
//    int msgbox_ret2 = msgBox2.exec();
    msgBox2.exec(); // ignore the return for now?

    QString StaffTeamFilename = QFileDialog::getOpenFileName(this);
    qDebug() << StaffTeamFilename;

    if (StaffTeamFilename.isEmpty())
        return;

    if(StaffTeamFilename.right(4) != ".txt")
    {
        QMessageBox::warning(this, "Load Staff Team","Incorrect File, must have extention '.txt'.");
        return;
    }
    QFile StaffTeamFile(StaffTeamFilename);

    if (!StaffTeamFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Duty Scheduler","Cannot read file.");
        return;
    }

    StaffTeamFile.close();

    m->reset();

    s = new scheduleWidget(sw.getValues(), StaffTeamFilename);
    setCentralWidget(s);

    newScheduleAct->setDisabled(true);
    newStaffTeamAct->setDisabled(true);
    openStaffTeamAct->setDisabled(true);
    saveStaffTeamAct->setDisabled(true);
    saveAsStaffTeamAct->setDisabled(true);
}

void MainWindow::openSchedule()
{

}

void MainWindow::about()
{
    QMessageBox::about(this, "The Duty Schedule Tool", "This tool is intended to assist with the process of creating a duty schedule. \n\nFor quick help, hover your mouse over a button or field that you are unsure about and a description will appear in the status bar at the bottom of the application.");
}



//FILE HANDLERS
void MainWindow::loadStaffTeamFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Duty Scheduler","Cannot read file.");
        return;
    }

    m->reset();

    QTextStream ts(&file);

    QList<staff*> *staffList = new QList<staff*>;
    QList<exam*> *examList = new QList<exam*>;

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

            staffList->append(s);
        }
        else // we are now looking at exam data
        {
            id = current_Line.at(0).toInt();
            date = current_Line.at(1);

            if (current_Line.at(2) == "1")
                e_night = true;
            else
                e_night = false;

            e = new exam(id,date,e_night);

            examList->append(e);
        }
    }

    m->load(staffList, examList);
    currentStaffTeamFile = fileName;

    //statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::saveStaffTeamFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Save Staff Team File","Cannot write file.");
        return;
    }

    sList = m->getStaff();
    eList = m->getExams();

    QTextStream ts(&file);

    QStringList t = m->getTeam().split(",",QString::SkipEmptyParts);
    int t_ids[t.size()];

    ts << "[STAFF]" << endl;
    for (int x = 0; x < t.size(); x++)
    {
        t_ids[x] = t.at(x).toInt();

        ts << QString::number(x) << ","
           << sList->at(t_ids[x])->getFirstName() << ","
           << sList->at(t_ids[x])->getLastName() << ","
           << QString(sList->at(t_ids[x])->getPosition()?"D":"R") << ","
           << QString(sList->at(t_ids[x])->getGender()?"M":"F") << ","
           << QString::number(sList->at(t_ids[x])->getNightClass()) << ","
           << sList->at(t_ids[x])->getAvailability()
           << sList->at(t_ids[x])->getExams() << endl;
    }
    ts << "[EXAMS]" << endl;
    for (int x = 0; x < eList->size(); x++)
    {
        ts << QString::number(eList->at(x)->getId()) << ","
           << eList->at(x)->getDate() << ","
           << QString(eList->at(x)->getNight()?"1":"0") << endl;
    }

    file.close();
    currentStaffTeamFile = fileName;
    //statusBar()->showMessage(tr("File saved"), 2000);
}




