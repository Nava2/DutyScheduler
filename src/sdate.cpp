#include "sdate.h"

#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

#include "staff.h"
#include "stafflist.h"

const QString SDate::AM_NOT_SET = "xxx";

SDate::SDate()
    : QDate(), spDuty(false), AM(AM_NOT_SET),
      defaultNeededD(true), defaultNeededR(true),
      rasNeeded(0), donsNeeded(0), weekday(0) {
}

SDate::SDate(const QDate &d, int donsN, int rasN)
    : QDate(d.year(), d.month(), d.day()), spDuty(false),
      AM(AM_NOT_SET), defaultNeededD(true), defaultNeededR(true),
      rasNeeded(rasN), donsNeeded(donsN), weekday(d.dayOfWeek())
{
}

SDate::SDate(const QVariantMap &map) :
    QDate(), AM(AM_NOT_SET),
    defaultNeededD(true), defaultNeededR(true)
{
    *this << map;
}

SDate::~SDate()
{
}

bool SDate::isSpecial()
{
    return spDuty;
}

void SDate::setSpecial(bool s)
{
    spDuty = s;
    donsOn.clear();
    rasOn.clear();
    AM = AM_NOT_SET;
}

void SDate::setAM(const QString &ami) // set the AM
{
    AM = ami;
}

QString SDate::getAM() const // return the AM id
{
    return AM;
}

int SDate::getWeekday() const
{
    return weekday;
}

void SDate::addStaff(const QString &s, bool pos)
{
    if (cantWork.contains(s))
        return;

    if (pos)        // staff has position of DON
        donsOn.append(s);
    else            // staff is an RA
        rasOn.append(s);
}

void SDate::removeStaff(const QString &s, bool pos)
{
    if (pos)
    {
        if (s == AM)
        {
            AM = AM_NOT_SET;
        }
        else
        {
            donsOn.removeAll(s);
        }
    }
    else
    {
        rasOn.removeAll(s);
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
    if (AM == AM_NOT_SET)
        return false;

    return (rasFull() && donsFull());
}

void SDate::addCantWork(const QString &input)
{
    if (!cantWork.contains(input))
        cantWork.append(input);
}

void SDate::addCantWork(const QList<QString > &input)
{
    foreach (QString uid, input) {
        addCantWork(uid);
    }
}

bool SDate::staffCantWork(const QString &input)
{
    if (cantWork.contains(input))
        return true;

    return false;
}

bool SDate::isOn(const QString &id) const
{
    return (AM == id || donsOn.contains(id) || rasOn.contains(id));
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

bool SDate::canWork(const QString &id) const
{
    if (cantWork.contains(id))
        return false;

    return true;
}

// true if special duty
bool SDate::exportOn(const StaffList &team, Staff::Ptr &am, QList<Staff::Ptr> &dons, QList<Staff::Ptr> &ras) const {
    if (AM != AM_NOT_SET)
        am = team[AM];

    dons.clear();
    ras.clear();

    foreach (QString id, donsOn) {
        dons += team[id];
    }

    foreach (QString id, rasOn) {
        ras += team[id];
    }

    return spDuty;
}

QString SDate::exportOn() const
{
    //returns the list of staff with AM first, then dons, then ras.
   if(spDuty)
       return "special";

   QString ret = "";

   ret += AM + ",";

   foreach (QString x, donsOn)
       ret += x + ",";

   foreach (QString x, rasOn)
       ret += x + ",";

   if (ret.left(3) == "999")
       ret = "";

   return ret;

}

QString SDate::getCantWorkStr()
{
    QString ret = "";

    if (cantWork.isEmpty())
        return ret;

    foreach(QString id, cantWork)
        ret += id + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getDonsStr()
{
    QString ret = "";

    if (donsOn.isEmpty())
        return ret;

    foreach(QString id, donsOn)
        ret += id + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getRasStr()
{
    QString ret = "";

    if (rasOn.isEmpty())
        return ret;

    foreach (QString id, rasOn)
        ret += id + ",";

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
    foreach (QString id, cantWork) {
        cantWorkVar.append(id);
    }
    map["cantwork"] = cantWorkVar;

    if (spDuty) // don't need to store the other info
        return;

    map["am"] = AM;

    QVariantList donsVar;
    foreach (QString id, donsOn) {
        donsVar.append(id);
    }
    map["dons"] = donsVar;

    QVariantList rasVar;
    foreach (QString id, rasOn) {
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
        cantWork.append(id.toString());
    }

    QVariantMap needed = map["need"].toMap();
    donsNeeded = needed["dons"].toInt();
    rasNeeded = needed["ras"].toInt();

    if (spDuty) // others are irrelevant/not stored
        return;

    AM = map["am"].toString();

    QVariantList donsVar = map["dons"].toList();
    foreach (QVariant id, donsVar) {
        donsOn.append(id.toString());
    }

    QVariantList rasVar = map["ras"].toList();
    foreach (QVariant id, rasVar) {
        rasOn.append(id.toString());
    }
}
