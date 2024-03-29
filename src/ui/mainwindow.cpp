#include "mainwindow.h"

#include <QVariantMap>
#include <QVariant>

#include <QFileInfo>
#include <QFile>

#include "ui/schedulewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentStaffTeamFile(""), currentScheduleFile(""),
      saveNecessary(false)
{
    createActions();
    createMenu();
    Q_UNUSED(statusBar());

    m = new MainWidget(this);

    setCentralWidget(m);

    saveTimer = new QTimer(this);
    saveTimer->setInterval(1000 * 60 * 5); // every 5 minutes try to save
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(onSaveTimer()));
    saveTimer->start();

    windowState = STAFF_WIDGET;

//    loadSchedule();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::createActions()
{
    QStyle *s = this->style();

    newStaffTeamAct = new QAction("New Team", this);
    newStaffTeamAct->setStatusTip("Create a new staff team");
    newStaffTeamAct->setShortcut(QKeySequence("Ctrl+n"));
    newStaffTeamAct->setIcon(s->standardIcon(QStyle::SP_FileIcon));
    connect(newStaffTeamAct, SIGNAL(triggered()),this, SLOT(newStaffTeam()));

    openStaffTeamAct = new QAction("Open Team", this);
    openStaffTeamAct->setStatusTip("Open an existing staff team");
    openStaffTeamAct->setShortcut(QKeySequence("Ctrl+o"));
    openStaffTeamAct->setIcon(s->standardIcon(QStyle::SP_DriveFDIcon));
    connect(openStaffTeamAct, SIGNAL(triggered()),this, SLOT(openStaffTeam()));

    saveStaffTeamAct = new QAction("Save Team", this);
    saveStaffTeamAct->setStatusTip("Save the opened staff team");
    saveStaffTeamAct->setShortcut(QKeySequence("Ctrl+s"));
    saveStaffTeamAct->setEnabled(false);
    connect(saveStaffTeamAct, SIGNAL(triggered()),this, SLOT(saveStaffTeam()));

    saveAsStaffTeamAct = new QAction("Save Team As", this);
    saveAsStaffTeamAct->setStatusTip(tr("Save the document under a new name"));
    saveAsStaffTeamAct->setShortcut(QKeySequence("Ctrl+shift+s"));
    connect(saveAsStaffTeamAct, SIGNAL(triggered()), this, SLOT(saveAsStaffTeam()));

    newScheduleAct = new QAction("New Schedule", this);
    newScheduleAct->setStatusTip("Create a new duty schedule");
    newScheduleAct->setShortcut(QKeySequence("f2"));
    connect(newScheduleAct, SIGNAL(triggered()),this, SLOT(newSchedule()));

    saveScheduleAct = new QAction("Save Schedule", this);
    saveScheduleAct->setStatusTip("Save the current schedule to work on it later.");
    saveScheduleAct->setShortcut(QKeySequence("f3"));
    connect(saveScheduleAct, SIGNAL(triggered()),this, SLOT(saveSchedule()));
    saveScheduleAct->setDisabled(true);

    openScheduleAct = new QAction("Open Schedule", this);
    openScheduleAct->setStatusTip("Open and existing schedule.");
    connect(openScheduleAct, SIGNAL(triggered()),this, SLOT(loadSchedule()));
    
    refreshCountScheduleAct = new QAction("Refresh Counts", this);
    refreshCountScheduleAct->setStatusTip("Refresh counts if they are skewed.");
    connect(refreshCountScheduleAct, SIGNAL(triggered()), this, SLOT(refreshCountSchedule()));
    refreshCountScheduleAct->setEnabled(false);


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
    scheduleMenu->addAction(openScheduleAct);
    scheduleMenu->addAction(saveScheduleAct);
    scheduleMenu->addSeparator();
    scheduleMenu->addAction(refreshCountScheduleAct);


    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(aboutAct);

}

//MENU SLOTS

void MainWindow::newStaffTeam()
{
    m->reset();
    currentStaffTeamFile = "";

    windowState = STAFF_WIDGET;
}

void MainWindow::openStaffTeam() {

    // lists for loading
    StaffList staffList;
    QList<Exam::Ptr> finalList, midtermList;

    // get the file name, use currentStaffTeamFile if possible
    QString fileName("");
    if (currentStaffTeamFile.isEmpty()) {
        fileName = iohandle.getOpenFileName(this);
    }

    bool ok = iohandle.loadStaffTeam(fileName, staffList, finalList, midtermList);

    if (!ok) {
        // baaaad
        QString msg, error;
        iohandle.getErrorInfo(msg, error);
        QMessageBox::warning(this, msg, error);

        return;
    }

    m->reset();
    m->load(staffList, finalList, midtermList);
    sList = staffList;
    this->finalList = finalList;
    this->midtermList = midtermList;
    currentStaffTeamFile = fileName;

    windowState = STAFF_WIDGET;
}

void MainWindow::saveStaffTeam()
{
    if (currentStaffTeamFile.isEmpty()) {
        currentStaffTeamFile = iohandle.getSaveFileName(this);
    }

    saveStaffTeamName(currentStaffTeamFile);
}

void MainWindow::onSaveTimer() {
    // only save if the event was caught somewhere
    if (!saveNecessary)
        return;

    switch (windowState) {
    case STAFF_WIDGET: {
        if (!iohandle.getCurrentStaffFile().isEmpty()) {
            QString oldFile(iohandle.getCurrentStaffFile()); // store the old file

            saveStaffTeamName(iohandle.getCurrentStaffFile() + "~");

            iohandle.setCurrentStaffFile(oldFile); // replace the save file
            setSaveNecessary(false);
        } else {
            qDebug() << "no file name specified in auto-save";
        }

    } break;
    case SCHEDULE_WIDGET: {
        if (!iohandle.getCurrentScheduleFile().isEmpty()) {
            QString oldFile(iohandle.getCurrentScheduleFile()); // store the old file

            s->saveMidSchedule(iohandle.getCurrentScheduleFile() + "~");

            iohandle.setCurrentScheduleFile(oldFile); // replace the save file
            setSaveNecessary(false);
        } else {
            qDebug() << "no file name specified in auto-save";
        }
    } break;
    case SCHEDULE_WIZARD:
    default: {

    } break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (saveNecessary) {
        QString text("");
        switch (windowState) {
        case STAFF_WIDGET:
            text = "The staff team has been modified.";
            break;
        case SCHEDULE_WIDGET:
            text = "The schedule file has been modified.";
            break;
        default:
            // nothing to do..
            break;
        }

        if (!text.isEmpty()) {
            // if the team/schedule has been modified, then we will save it
            QMessageBox msgBox;
            msgBox.setText(text);
            msgBox.setInformativeText("Do you want to save your changes?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            switch (ret) {
            case QMessageBox::Save:
                onSaveTimer();
                break;
            default:
                // nothing to do then..
                break;
            }
        }
    }

    // force the iohandler to cleanup itself
    iohandle.cleanUpAutoSave();

    event->accept();
}

void MainWindow::setSaveNecessary(const bool nec) {
    saveNecessary = nec;
    saveStaffTeamAct->setEnabled(nec);
}

void MainWindow::onUpdateSaveState() {
    setSaveNecessary(true);
}

void MainWindow::saveAsStaffTeam() {
    currentStaffTeamFile = iohandle.getSaveFileName(this);
    saveStaffTeamName(currentStaffTeamFile);
}

void MainWindow::saveStaffTeamName(const QString &fileName)
{
    if (fileName.isEmpty())
        return ;


    StaffList _sList = m->getStaff();
    QList<Exam::Ptr> _fList, _mList;
    m->getExams(_fList, _mList);

    bool ok = iohandle.saveStaffTeam(fileName, _sList, _fList, _mList);

    if (!ok) {
        // badnews bears.
        QString msg, error;
        iohandle.getErrorInfo(msg, error);
        QMessageBox::warning(this, msg, error);

        return;
    }

    sList = _sList;
    finalList = _fList;
    midtermList = _mList;

    setSaveNecessary(false);
}

void MainWindow::saveSchedule()
{
    QString fileName;

    if (iohandle.getCurrentScheduleFile().isEmpty()) {
        fileName = iohandle.getSaveFileName(this, IOHandler::SCHEDULE);

        if (fileName.isEmpty())
            return;
    } else {
        fileName = iohandle.getCurrentScheduleFile();
    }

    s->saveMidSchedule(fileName);

    currentScheduleFile = fileName;

    setSaveNecessary(false);
}

void MainWindow::loadSchedule()
{
    QString fileName = iohandle.getOpenFileName(this, IOHandler::SCHEDULE);

    StaffList team = m->getStaff();
    QList<Exam::Ptr> finals, midterms;
    m->getExams(finals, midterms);

    if (team.count() == 0 || (finals.count() + midterms.count() == 0)) {
        if (iohandle.getCurrentStaffFile().isEmpty()) {
            QMessageBox msgBox2;
            msgBox2.setWindowTitle("Duty Schedule Tool");
            msgBox2.setText("Select the staff team to use for your new schedule.");
            msgBox2.setStandardButtons(QMessageBox::Ok);
            msgBox2.setDefaultButton(QMessageBox::Ok);
            msgBox2.exec();

            int x = 0;
            iohandle.clearErrorInfo();
            do {
                iohandle.getOpenFileName(this, IOHandler::STAFF);

                QString title(""), msg("");
                iohandle.getErrorInfo(msg, title);
                if (!msg.isEmpty() && !title.isEmpty())
                    QMessageBox::warning(this, title, msg);
            } while (!iohandle.checkFileName(iohandle.getCurrentStaffFile()) && x++ < 2);

            if (iohandle.getCurrentStaffFile().isEmpty()) {
                // no file opened properly
                qDebug() << "No Schedule file loaded.. no action required.";
                return;
            }
        }

        iohandle.loadStaffTeam(iohandle.getCurrentStaffFile(), team, finals, midterms);
    }

    if (centralWidget() == s)
        delete s;
    s = new ScheduleWidget( fileName, team, finals, midterms, this );

    setCentralWidget(s);

    openScheduleAct->setDisabled(true);
    newScheduleAct->setDisabled(true);
    saveScheduleAct->setEnabled(true);
    refreshCountScheduleAct->setEnabled(true);

    windowState = SCHEDULE_WIDGET;
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

    windowState = SCHEDULE_WIZARD;

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
        int x = 0;
        do {
            StaffTeamFilename = iohandle.getOpenFileName(this, IOHandler::STAFF);

            QString title(""), msg("");
            iohandle.getErrorInfo(msg, title);
            if (!msg.isEmpty() && !title.isEmpty())
                QMessageBox::warning(this, title, msg);
        } while (!iohandle.checkFileName(StaffTeamFilename) && x++ < 2);
    }

    QFile StaffTeamFile(StaffTeamFilename);

    if (!StaffTeamFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Duty Schedule Tool","Cannot read file.");
        return;
    }
    StaffTeamFile.close();

    m->reset();

    s = new ScheduleWidget(StaffTeamFilename, sw, this);

    setCentralWidget(s);

    newScheduleAct->setDisabled(true);
    newStaffTeamAct->setDisabled(true);
    openStaffTeamAct->setDisabled(true);
    saveStaffTeamAct->setDisabled(true);
    saveAsStaffTeamAct->setDisabled(true);
    saveScheduleAct->setEnabled(true);
    openScheduleAct->setDisabled(true);
    refreshCountScheduleAct->setEnabled(true);

    windowState = SCHEDULE_WIDGET;
}

void MainWindow::refreshCountSchedule() {
    s->refreshStats();
}


void MainWindow::about()
{
    QMessageBox::about(this, "Duty Schedule Tool", "This tool is intended to assist with the process of creating a duty schedule. \n\nFor quick help, hover your mouse over a button or field that you are unsure about and a description will appear in the status bar at the bottom of the application.");
}


