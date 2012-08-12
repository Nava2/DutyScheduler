#include "exam.h"

exam::exam()
    : id(99999), date(""), night(false)
{ }

exam::exam(int _id, QString _date, bool _night)
    : id (_id), date(_date), night(_night)
{ }

void exam::setDate(QString d)
{
    date = d;
}

void exam::setNight(bool n)
{
    night = n;
}

QString exam::getDate()
{
    return date;
}

bool exam::getNight()
{
    return night;
}

int exam::getId()
{
    return id;
}
