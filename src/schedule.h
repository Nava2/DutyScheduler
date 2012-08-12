#ifndef SCHEDULE_H
#define SCHEDULE_H


#include <QList>
#include <QString>
#include <QStringList>
#include <QDate>
#include "sdate.h"


class schedule
{

public:
    schedule();
    schedule(QString, QString);

private:
    QList<sDate*> * datesList;
    QDate startDate;
    QDate endDate;
    bool examSchedule;
    int rasNeeded[7];
    int donsNeeded[7];

    QString staffTeamFilename;

};

#endif // SCHEDULE_H
