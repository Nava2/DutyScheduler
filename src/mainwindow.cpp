#include "mainwindow.h"

#include <QVariantMap>
#include <QVariant>

#include <QFileInfo>
#include <QFile>

#include "json.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenu();
    (void)statusBar();

    currentStaffTeamFile = "";
    usingStaffTeamFile = "";

    m = new MainWidget();

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

    saveScheduleAct = new QAction("Save Schedule", this);
    saveScheduleAct->setStatusTip("Save the current schedule to work on it later.");
    connect(saveScheduleAct, SIGNAL(triggered()),this, SLOT(saveSchedule()));
    saveScheduleAct->setDisabled(true);

    openScheduleAct = new QAction("Open Schedule", this);
    openScheduleAct->setStatusTip("Open and existing schedule.");
    connect(openScheduleAct, SIGNAL(triggered()),this, SLOT(loadSchedule()));


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
    scheduleMenu->addAction(saveScheduleAct);
    scheduleMenu->addAction(openScheduleAct);


    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}

//MENU SLOTS

void MainWindow::newStaffTeam()
{
    m->reset();
    currentStaffTeamFile = "";
}

void MainWindow::openStaffTeam() {

    // lists for loading
    QList<Staff::Ptr> *staffList = new QList<Staff::Ptr>;
    QList<Exam::Ptr> *examList = new QList<Exam::Ptr>;

    // get the file name, use currentStaffTeamFile if possible
    QString fileName("");
    if (currentStaffTeamFile.isEmpty()) {
        fileName = QFileDialog::getOpenFileName(this, "Open Staff Team..", QDir::home().path());
    } else {
        fileName = QFileDialog::getOpenFileName(this, "Open Staff Team..",
                                                QFileInfo(currentStaffTeamFile).dir().path());
    }

    bool ok = iohandle.loadStaffTeam(fileName, *staffList, *examList);

    if (!ok) {
        // baaaad
        QString msg, error;
        iohandle.getErrorInfo(msg, error);
        QMessageBox::warning(this, msg, error);

        return;
    }

    m->reset();
    m->load(staffList, examList);
    sList = staffList;
    eList = examList;
    currentStaffTeamFile = fileName;
}

void MainWindow::saveStaffTeam()
{
    if (currentStaffTeamFile.isEmpty()) {
        currentStaffTeamFile = QFileDialog::getSaveFileName(this, "Save as..", QDir::home().path());
    }

    saveStaffTeamName(currentStaffTeamFile);
}

void MainWindow::saveAsStaffTeam() {
    if (!currentStaffTeamFile.isEmpty()) {
        QFileInfo f(currentStaffTeamFile);
        currentStaffTeamFile = QFileDialog::getSaveFileName(this,
                                                            "Save as..",
                                                            f.dir().path());
    } else {
        currentStaffTeamFile = QFileDialog::getSaveFileName(this);
    }
    saveStaffTeamName(currentStaffTeamFile);
}

void MainWindow::saveStaffTeamName(const QString &fileName)
{
    QList<Staff::Ptr> *_sList = m->getStaff();
    QList<Exam::Ptr> *_eList = m->getExams();

    bool ok = iohandle.saveStaffTeam(fileName, *_sList, *_eList);

    if (!ok) {
        // badnews bears.
        QString msg, error;
        iohandle.getErrorInfo(msg, error);
        QMessageBox::warning(this, msg, error);

        return;
    }

    sList = _sList;
    eList = _eList;
}

void MainWindow::saveSchedule()
{
    QString fileName = QFileDialog::getSaveFileName(this);

    if (fileName.isEmpty())
        return;

    if(fileName.right(4) != ".txt")
    {
        QMessageBox::warning(this, "Save Schedule","File must have extention '.txt'.");
        return;
    }


    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Save Schedule","Cannot write file.");
        return;
    }
    file.close();

    s->saveMidSchedule(fileName);

}

void MainWindow::loadSchedule()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Schedule File..", QDir::home().path());
    if (fileName.isEmpty())
        return;

    if (centralWidget() == s)
        delete s;

    QList<Staff::Ptr> *team = m->getStaff();
    QList<Exam::Ptr> *exams = m->getExams();

    if ((!team || team->count() == 0) || (!exams || exams->count() == 0)) {
        QString StaffTeamFilename = iohandle.getCurrentStaffFile();

        if (StaffTeamFilename.isEmpty()) {
            QMessageBox msgBox2;
            msgBox2.setWindowTitle("Duty Schedule Tool");
            msgBox2.setText("Select the staff team to use for your new schedule.");
            msgBox2.setStandardButtons(QMessageBox::Ok);
            msgBox2.setDefaultButton(QMessageBox::Ok);
            msgBox2.exec();

            iohandle.clearErrorInfo();
            do {
                StaffTeamFilename = QFileDialog::getOpenFileName(this, "Open Staff Team..", QDir::home().path());

                QString title(""), msg("");
                iohandle.getErrorInfo(msg, title);
                if (!msg.isEmpty() && !title.isEmpty())
                    QMessageBox::warning(this, title, msg);
            } while (!iohandle.checkFileName(StaffTeamFilename));
        }

        iohandle.loadStaffTeam(StaffTeamFilename, *team, *exams);
    }

    s = new ScheduleWidget(fileName, team, exams );

    setCentralWidget(s);

    openScheduleAct->setDisabled(true);
    newScheduleAct->setDisabled(true);
    saveScheduleAct->setEnabled(true);
}

void MainWindow::newSchedule()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Duty Schedule Tool");
    msgBox.setText("Any unsaved staff team data will be lost. Continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int msgbox_ret = msgBox.exec();

    if (msgbox_ret == 0x10000)
        return;

    ScheduleWizzard sw;
    int ret = sw.exec();

    if (ret == 0)
        return;

    QString wizzard = sw.getValues();

    if(wizzard == "")
    {
        QMessageBox::warning(this, "Duty Schedule Tool","ERROR: Bad date inputs.");
        return;
    }

    QString StaffTeamFilename = iohandle.getCurrentStaffFile();

    if (StaffTeamFilename.isEmpty()) {
        QMessageBox msgBox2;
        msgBox2.setWindowTitle("Duty Schedule Tool");
        msgBox2.setText("Select the staff team to use for your new schedule.");
        msgBox2.setStandardButtons(QMessageBox::Ok);
        msgBox2.setDefaultButton(QMessageBox::Ok);
        msgBox2.exec();

        iohandle.clearErrorInfo();
        do {
            StaffTeamFilename = QFileDialog::getOpenFileName(this, "Open Staff Team..", QDir::home().path());

            QString title(""), msg("");
            iohandle.getErrorInfo(msg, title);
            if (!msg.isEmpty() && !title.isEmpty())
                QMessageBox::warning(this, title, msg);
        } while (!iohandle.checkFileName(StaffTeamFilename));
    }

    QFile StaffTeamFile(StaffTeamFilename);

    if (!StaffTeamFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Duty Schedule Tool","Cannot read file.");
        return;
    }
    StaffTeamFile.close();

    m->reset();

    s = new ScheduleWidget(StaffTeamFilename, sw);

// new QScheduleApplet
    setCentralWidget(s);


    newScheduleAct->setDisabled(true);
    newStaffTeamAct->setDisabled(true);
    openStaffTeamAct->setDisabled(true);
    saveStaffTeamAct->setDisabled(true);
    saveAsStaffTeamAct->setDisabled(true);
    saveScheduleAct->setEnabled(true);
    openScheduleAct->setDisabled(true);
}

void MainWindow::about()
{
    QMessageBox::about(this, "Duty Schedule Tool", "This tool is intended to assist with the process of creating a duty schedule. \n\nFor quick help, hover your mouse over a button or field that you are unsure about and a description will appear in the status bar at the bottom of the application.");
}


