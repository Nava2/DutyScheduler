#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QTextStream>
#include <QMenu>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QStatusBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QTimer>

#include <iostream>
#include "mainwidget.h"
#include "schedulewidget.h"
#include "schedulewizzard.h"
#include "exam.h"
#include "staff.h"
#include "sdate.h"
#include "iohandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    virtual
    ~MainWindow();

private slots:

    void newSchedule();

    void about();
    void newStaffTeam();
    void openStaffTeam();
    void saveStaffTeam();
    void saveAsStaffTeam();
    void saveStaffTeamName(const QString &fileName);

    void saveSchedule();
    void loadSchedule();

    void onSaveTimer();
    void onUpdateSaveState();



private:

    // file management
    IOHandler iohandle;

    StaffList sList;
    QList<Exam::Ptr> finalList, midtermList;
    QString currentStaffTeamFile,
        currentScheduleFile;

    // widgets
    MainWidget *m;
    ScheduleWidget *s;
    ScheduleWizzard *w;

    enum WindowState {
        STAFF_WIDGET, SCHEDULE_WIDGET, SCHEDULE_WIZARD
    } windowState;

    //menus and actions
    void createActions();
    void createMenu();
    QMenu *staffTeamMenu;
    QMenu *scheduleMenu;
    QMenu *helpMenu;
    QAction *newStaffTeamAct;
    QAction *openStaffTeamAct;
    QAction *saveStaffTeamAct;
    QAction *saveAsStaffTeamAct;
    QAction *newScheduleAct;
    QAction *saveScheduleAct;
    QAction *openScheduleAct;
    QAction *aboutAct;

    bool saveNecessary;
    QTimer *saveTimer;






};

#endif // MAINWINDOW_H
