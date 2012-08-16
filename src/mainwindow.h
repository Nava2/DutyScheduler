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

    /**
      Takes in an extension of .EXTENSION form (period included), checks it against the file.
      It will assume the extension is correct if none is present.
      */
    static bool
    CheckFileExtension(QWidget *parent, QString ext, QString &fileName) {

        if (fileName.isEmpty()) {
            QMessageBox::warning(parent, "Warning",
                                 "Empty filename.");
            return false;
        }

        int index = fileName.lastIndexOf('.');

        if (index == -1) {
            // not found, assume it was correct
            fileName += ext;
        } else {
            QString sub = fileName.right(fileName.length() - index);
            if (sub != ext) {
                QMessageBox::warning(parent, "Warning",
                                     "Filename must be of type \"" + ext
                                     + "\" (found: \"" + sub + "\")");
                return false;
            }
        }

        return true;
    }

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
