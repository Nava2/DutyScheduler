#include "sdate.h"


sDate::sDate()
{
    rasNeeded = 0;
    donsNeeded = 0;
    weekday = 0;
    AM = 999;
}

sDate::sDate(QDate d, int donsN, int rasN)
{
    theDate = d;
    spDuty = false;
    rasNeeded = rasN;
    donsNeeded = donsN;
    AM = 999;
    donsOn = new QList<int>;
    rasOn = new QList<int>;
    cantWork = new QList<int>;

    weekday = theDate.dayOfWeek();
}

sDate::~sDate()
{
    delete donsOn;
    delete rasOn;
    delete cantWork;
}

QDate sDate::getDate()
{   return theDate; }

bool sDate::isSpecial()
{   return spDuty;  }

void sDate::setSpecial(bool s)
{
    spDuty = s;
    donsOn->clear();
    rasOn->clear();
    AM = 999;
}

void sDate::setAM(int ami) // set the AM
{   AM = ami;       }

int sDate::getAM() // return the AM id
{    return AM;     }

int sDate::getWeekday()
{   return weekday; }

void sDate::addStaff(int s, bool pos)
{
    if (cantWork->contains(s))
        return;

    if (pos)        // staff has position of DON
        donsOn->append(s);
    else            // staff is an RA
        rasOn->append(s);
}

void sDate::removeStaff(int s, bool pos)
{
    if (pos)
    {
        if (s == AM)
        {
            AM = 999;
        }
        else
        {
            donsOn->removeAt(donsOn->indexOf(s));
        }
    }
    else
    {
        rasOn->removeAt(rasOn->indexOf(s));
    }
}

bool sDate::rasFull()
{
    if (rasOn->size() == rasNeeded)
        return true;

    return false;
}

bool sDate::donsFull()
{
    if (donsOn->size() == donsNeeded)
        return true;

    return false;
}

bool sDate::isFull()
{
    if(AM == 999)
        return false;

    return (rasFull() && donsFull());
}

void sDate::addCantWork(int input)
{
    cantWork->append(input);
}

bool sDate::staffCantWork(int input)
{
    if(cantWork->contains(input))
        return true;

    return false;
}

bool sDate::isOn(int id)
{
    if(AM == id || donsOn->contains(id) || rasOn->contains(id))
        return true;
    else
        return false;

}

int sDate::getDonsNeeded()
{
    return donsNeeded - donsOn->count();
}

int sDate::getRasNeeded()
{
    return rasNeeded - rasOn->count();
}

bool sDate::canWork(int id)
{
    if (cantWork->contains(id))
        return false;

    return true;
}

QString sDate::exportOn()
{
    //returns the list of staff with AM first, then dons, then ras.
   if(spDuty)
       return "special";

   QString ret = "";

   ret += QString::number(AM) + ",";

   for(int x = 0; x < donsOn->count(); x++)
       ret += QString::number(donsOn->at(x)) + ",";

   for(int z = 0; z < rasOn->count(); z++)
       ret += QString::number(rasOn->at(z)) + ",";

   if (ret.left(3) == "999")
       ret = "";

   return ret;

}

QString sDate::getCantWork()
{
    QString ret = "";

    if (cantWork->isEmpty())
        return ret;

    for (int x = 0; x < cantWork->count(); x++)
        ret += QString::number(cantWork->at(x)) + ",";

    ret.chop(1);

    return ret;
}

QString sDate::getDons()
{
    QString ret = "";

    if (donsOn->isEmpty())
        return ret;

    for (int x = 0; x < donsOn->count(); x++)
        ret += QString::number(donsOn->at(x)) + ",";

    ret.chop(1);

    return ret;
}

QString sDate::getRas()
{
    QString ret = "";

    if (rasOn->isEmpty())
        return ret;

    for (int x = 0; x < rasOn->count(); x++)
        ret += QString::number(rasOn->at(x)) + ",";

    ret.chop(1);

    return ret;
}
