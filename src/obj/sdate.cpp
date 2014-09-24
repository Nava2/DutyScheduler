#include "sdate.h"

#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>

#include "staff.h"
#include "stafflist.h"

const QString SDate::AM_NOT_SET = "xxx";

SDate::SDate()
    : QDate(), spDuty(false), AM(AM_NOT_SET),
      defaultNeededD(true), defaultNeededR(true),
      _examDay(false),
      rasNeeded(0), donsNeeded(0), weekday(0) {
    _dayShiftMember[0] = _dayShiftMember[1] = "";
}

SDate::SDate(const QDate &date, const bool examDay, const int donsN, const int rasN)
    : QDate(date.year(), date.month(), date.day()), spDuty(false),
      AM(AM_NOT_SET), defaultNeededD(true), defaultNeededR(true),
      _examDay(examDay),
      rasNeeded(rasN), donsNeeded(donsN), weekday(date.dayOfWeek())
{
    _dayShiftMember[0] = _dayShiftMember[1] = "";
}

SDate::SDate(const QVariantMap &map) :
    QDate(), AM(AM_NOT_SET),
    defaultNeededD(true), defaultNeededR(true), _examDay(false)
{
    *this << map;
}

SDate::~SDate()
{
}

bool SDate::isSpecial() const
{
    return spDuty;
}

bool SDate::isExam() const {
    return _examDay;
}

void SDate::setSpecial(bool s)
{
    spDuty = s;
    donsOn.clear();
    rasOn.clear();
    AM = AM_NOT_SET;
}

QString SDate::dayShiftMember(int shiftNo) const {
    return _dayShiftMember[shiftNo];
}

void SDate::setDayShiftMember(int shiftNo, const QString &id) {
    _dayShiftMember[shiftNo] = id;
}

bool SDate::isOnDayDuty(const QString &id) const {
    return _examDay && (id == _dayShiftMember[0] || id == _dayShiftMember[1]);
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

    bool out = rasFull() && donsFull();
    if (isExam())
        out = out && !_dayShiftMember[0].isEmpty() && !_dayShiftMember[1].isEmpty();

    return out;
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

QList<QString > SDate::getCantWork() const {
    return cantWork;
}

QList<QString > SDate::getDons() const {
    return donsOn;
}

QList<QString > SDate::getRas() const {
    return rasOn;
}

bool SDate::staffCantWork(const QString &input)
{
    if (cantWork.contains(input))
        return true;

    return false;
}

bool SDate::isOn(const QString &uid, const bool checkDay) const
{
    bool result = (AM == uid || donsOn.contains(uid) || rasOn.contains(uid));

    if (checkDay) {
        result = result || isOnDayDuty(uid);
    }

    return result;
}

int SDate::getDonsLeft() const
{
    return donsNeeded - donsOn.count();
}

int SDate::getRasLeft() const
{
    return rasNeeded - rasOn.count();
}

int SDate::getRAsNeeded() const {
    return rasNeeded;
}

int SDate::getDonsNeeded() const {
    return donsNeeded;
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

QString SDate::getCantWorkStr() const {
    QString ret = "";

    if (cantWork.isEmpty())
        return ret;

    foreach(QString id, cantWork)
        ret += id + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getDonsStr() const {
    QString ret = "";

    if (donsOn.isEmpty())
        return ret;

    foreach(QString id, donsOn)
        ret += id + ",";

    ret.chop(1);

    return ret;
}

QString SDate::getRasStr() const {
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
    map["exam"] = _examDay;

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

    if (_examDay) {
        QStringList dayDuty;
        dayDuty.append(_dayShiftMember[0]);
        dayDuty.append(_dayShiftMember[1]);
        map["dayduty"] = dayDuty;
    }
}

// in
void SDate::operator <<(const QVariantMap &map) {
    // clear all lists:
    donsOn.clear();
    rasOn.clear();
    cantWork.clear();

    QDate d = map["date"].toDate();
    setDate(d.year(), d.month(), d.day());


    // load the object
    spDuty = map["special"].toBool();
    _examDay = map["exam"].toBool();

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

    if (_examDay) {
        QVariantList list = map["dayduty"].toList();
        _dayShiftMember[0] = list[0].toString();
        _dayShiftMember[1] = list[1].toString();
    }
}
