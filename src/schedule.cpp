#include "schedule.h"

schedule::schedule()
{
}

schedule::schedule(QString inp, QString file)
{

    QStringList input = inp.split(",");

    startDate = QDate::fromString(input.at(0),"dd/MM/yyyy");
    endDate = QDate::fromString(input.at(1),"dd/MM/yyyy");

    staffTeamFilename = file;

    if(input.at(2) == "Y")
        examSchedule = true;
    else
        examSchedule = false;


    for(int x = 0; x<7; x++)
    {
        donsNeeded[x] = input.at(x+3).toInt();
        rasNeeded[x] = input.at(x+10).toInt();
    }


    datesList = new QList<sDate*>;
    sDate *day;
    QDate counter;
    counter.setDate(startDate.year(),startDate.month(),startDate.day());

    while(counter.toString("ddMMyyyy") != endDate.addDays(1).toString("ddMMyyyy"))
    {
        day = new sDate(counter,donsNeeded[counter.dayOfWeek()-1],rasNeeded[counter.dayOfWeek()-1]);

        datesList->append(day);

        counter = counter.addDays(1);
    }

}
