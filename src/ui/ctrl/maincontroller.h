#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QApplication>

#include "ctrl/examcalendar.h"

#include "obj/stafflist.h"
#include "obj/exam.h"
#include "obj/examlist.h"

#include "ui/dsgn/mainwindow.h"
#include "ui/dsgn/teamwidget.h"

#include "iohandler.h"

namespace ctrl {

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(int argc, char *argv[], QObject *parent = 0);
    ~MainController();

    ExamList *midterms() const;
    ExamList *finals() const;
    StaffList *staff() const;

    void setTeamPath(const QString &path);
    QString teamPath() const ;

    int exec();


private:
    QApplication _app;

    // CONTROLLERS:
    ctrl::ExamCalendar *_ectrlMidterm, *_ectrlFinal;

    // UI:
    MainWindow *_mw;
    TeamWidget *_tw;

    // MODELS:
    ExamList *_midterms, *_finals;

    StaffList *_staff;

    IOHandler *_io;
    
    QString _teamPath;
};

}

#endif // MAINCONTROLLER_H
