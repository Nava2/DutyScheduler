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
#include <iostream>
#include "mainwidget.h"
#include "schedulewidget.h"
#include "schedulewizzard.h"
#include "exam.h"
#include "staff.h"
#include "sdate.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void newSchedule();
    void openSchedule();
    void about();
    void newStaffTeam();
    void openStaffTeam();
    void saveStaffTeam();
    void saveAsStaffTeam();


private:

    // file management
    void loadStaffTeamFile(const QString &fileName);
    void saveStaffTeamFile(const QString &fileName);

    void loadStaffTeamJson(const QString &fileName);
    void saveStaffTeamJson(const QString &);

    QList<staff*> * sList;
    QList<exam*> * eList;
    QString currentStaffTeamFile;
    QString usingStaffTeamFile;

    // widgets
    mainWidget *m;
    scheduleWidget *s;
    scheduleWizzard *w;

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
    QAction *openScheduleAct;
    QAction *aboutAct;






};

#endif // MAINWINDOW_H
