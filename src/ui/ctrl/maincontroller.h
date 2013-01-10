#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>

#include "obj/stafflist.h"
#include "obj/exam.h"
#include "obj/examlist.h"

#include "iohandler.h"

namespace ctrl {

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);

    ExamList *midterms() const;
    ExamList *finals() const;
    StaffList *staff() const;

    void setTeamPath(const QString &path);
    QString teamPath() const ;


private:
    ExamList *_midterms, *_finals;

    StaffList *_staff;

    IOHandler *_io;
    
    QString _teamPath;
};

}

#endif // MAINCONTROLLER_H
