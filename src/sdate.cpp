#include "sdate.h"

#include <QVariant>
#include <QVariantMap>
#include <QVariantList>


SDate::SDate()
    : QDate()
{
    rasNeeded = 0;
    donsNeeded = 0;
    weekday = 0;
    AM = 999;
}

SDate::SDate(const QDate &d, int donsN, int rasN)
    : QDate(d.year(), d.month(), d.day())
{
    spDuty = false;
    rasNeeded = rasN;
    donsNeeded = donsN;
    AM = 999;

    weekday = theDate.dayOfWeek();
}

SDate::SDate(const QVariantMap &map) :
    QDate(), AM(0)
{
    *this << map;
}

SDate::~SDate()
{
}

bool SDate::isSpecial()
{   return spDuty;  }

void SDate::setSpecial(bool s)
{
    spDuty = s;
    donsOn.clear();
    rasOn.clear();
    AM = 999;
}

void SDate::setAM(int ami) // set the AM
{   AM = ami;       }

int SDate::getAM() const // return the AM id
{    return AM;     }

int SDate::getWeekday() const
{   return weekday; }

void SDate::addStaff(int s, bool pos)
{
    if (cantWork.contains(s))
        return;

    if (pos)        // staff has position of DON
        donsOn.append(s);
    else            // staff is an RA
        rasOn.append(s);
}

void SDate::removeStaff(int s, bool pos)
{
    if (pos)
    {
        if (s == AM)
        {
            AM = 999;
        }
        else
        {
            donsOn.removeAt(donsOn.indexOf(s));
        }
    }
    else
    {
        rasOn.removeAt(rasOn.indexOf(s));
    }
}

bool SDate::rasFull() const
{
    if (rasOn.size() == rasNeeded)
        return true;

    return false;
}

bool SDate::donsFull() const
{
    if (donsOn.size() == donsNeeded)
        return true;

    return false;
}

bool SDate::isFull() const
{
    if(AM == 999)
        return false;

    return (rasFull() && donsFull());
}

void SDate::addCantWork(int input)
{
    cantWork.append(input);
}

bool SDate::staffCantWork(int input)
{
    if(cantWork.contains(input))
        return true;

    return false;
}

bool SDate::isOn(int id) const
{
    if(AM == id || donsOn.contains(id) || rasOn.contains(id))
        return true;
    else
        return false;

}

int SDate::getDonsNeeded() const
{
    return donsNeeded - donsOn.count();
}

int SDate::getRasNeeded() const
{
    return rasNeeded - rasOn.count();
}

void SDate::setRasNeeded(const int ras) {
    defaultNeededR &= rasNeeded == ras;
    rasNeeded = ras;
}

void SDate::setDonsNeeded(const int dons) {
    defaultNeededD &= dons == donsNeeded;
    donsNeeded = dons;
}

bool SDate::isDefaultNeeded() const {
    return defaultNeededD && defaultNeededR;
}

bool SDate::isWeekend() const {
    return dayOfWeek() == 5 || dayOfWeek() == 6;
}

bool SDate::canWork(int id) const
{
    if (cantWork.contains(id))
        return false;

    return true;
}

QString SDate::exportOn()
{
    //returns the list of staff with AM first, then dons, then ras.
   if(spDuty)
       return "special";

   QString ret = "";

   ret += QString::number(AM) + ",";

   foreach (int x, donsOn)
       ret += QString::number(x) + ",";

   foreach (int x, rasOn)
       ret += QString::number(x) + ",";

   if (ret.left(3) == "999")
       ret = "";

   return ret;

}

QString SDate::getCantWorkStr()
{
    QString ret = "";

    if (cantWork.isEmpty())
        return ret;

    foreach(int id, cantWork)
        ret += QString::number(id) + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getDonsStr()
{
    QString ret = "";

    if (donsOn.isEmpty())
        return ret;

    foreach(int id, donsOn)
        ret += QString::number(id) + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getRasStr()
{
    QString ret = "";

    if (rasOn.isEmpty())
        return ret;

    foreach (int id, rasOn)
        ret += QString::number(id) + ",";

    ret.chop(1);

    return ret;
}

// JSON
// out
void SDate::operator >>(QVariantMap &map) {
    map["date"] = *dynamic_cast<QDate *>(this);

    map["special"] = spDuty;

    QVariantMap needed;
    needed["dons"] = donsNeeded;
    needed["ras"] = rasNeeded;
    map["need"] = needed;

    QVariantList cantWorkVar;
    foreach (int id, cantWork) {
        cantWorkVar.append(id);
    }
    map["cantwork"] = cantWorkVar;

    if (spDuty) // don't need to store the other info
        return;

    map["am"] = AM;

    QVariantList donsVar;
    foreach (int id, donsOn) {
        donsVar.append(id);
    }
    map["dons"] = donsVar;

    QVariantList rasVar;
    foreach (int id, rasOn) {
        rasVar.append(id);
    }
    map["ras"] = rasVar;


}

// in
void SDate::operator <<(const QVariantMap &map) {
    // clear all lists:
    donsOn.clear();
    rasOn.clear();
    cantWork.clear();

    QDate d = map["date"].toDate();
    setYMD(d.year(), d.month(), d.day());

    // load the object
    spDuty = map["special"].toBool();

    QVariantList cantWorkVar = map["cantwork"].toList();
    foreach (QVariant id, cantWorkVar) {
        cantWork.append(id.toInt());
    }

    QVariantMap needed = map["need"].toMap();
    donsNeeded = needed["dons"].toInt();
    rasNeeded = needed["ras"].toInt();

    if (spDuty) // others are irrelevant/not stored
        return;

    AM = map["am"].toInt();

    QVariantList donsVar = map["dons"].toList();
    foreach (QVariant id, donsVar) {
        donsOn.append(id.toInt());
    }

    QVariantList rasVar = map["ras"].toList();
    foreach (QVariant id, rasVar) {
        rasOn.append(id.toInt());
    }
}
