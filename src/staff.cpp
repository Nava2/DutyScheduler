#include "staff.h"

#include <QVariantMap>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

Staff::Staff()
{
    id = 127;
    firstName = "Art";
    lastName = "Vandelay";
    position = false;
    gender = false;
    nightClass = 0;
    exams = "";
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;
}

Staff::Staff(int i, QString first, QString last, bool pos, bool gen, int night)
{
    id = i;
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    exams = "";
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;
}

Staff::Staff(const QVariantMap &json) :
    numShifts(0), numWeekendShifts(0), numAMShifts(0)
{
    *this << json;
}

Staff::Staff(const Staff &old) {
    this->id = old.id;
    firstName = old.firstName;
    lastName = old.lastName;
    position = old.position;
    gender = old.gender;

    exams = old.exams;
    numShifts = old.numShifts;
    numWeekendShifts = old.numWeekendShifts;
    numAMShifts = old.numAMShifts;
}

Staff::~Staff()
{

}

// JSON mapping:
void Staff::operator << (const QVariantMap &json) {
    id = json["id"].toInt();

    QVariantMap name = json["name"].toMap();
    firstName = name["f"].toString();
    lastName = name["l"].toString();

    position = json["pos"].toBool();
    gender = json["gndr"].toBool();
    nightClass = json["night"].toInt();
    exams = json["exams"].toString();

    foreach (QVariant qv, json["avail"].toList()) {
        availList.append(qv.toDate());
    }


    QVariantMap shiftMap = json["shifts"].toMap();
    numWeekendShifts = shiftMap["wkend"].toInt();
    if (position)
       numAMShifts = shiftMap["am"].toInt();
    numShifts = shiftMap["ttl"].toInt();
}

void Staff::operator >>(QVariantMap &json) {
    json["id"] = id;

    QVariantMap name;
    name["f"] = firstName;
    name["l"] = lastName;
    json["name"] = name;

    json["pos"] = position;
    json["gndr"] = gender;
    json["night"] = nightClass;
    json["exams"] = exams;

    QVariantList list;
    foreach (QDate d, availList) {
        list.append(QVariant(d));
    }

    json["avail"] = list;

    QVariantMap shiftMap;
    shiftMap["wkend"] = numWeekendShifts;
    if (position)
        shiftMap["am"] = numAMShifts;
    shiftMap["ttl"] = numShifts;

    json["shifts"] = shiftMap;
}

void Staff::update(QString first, QString last, bool pos, bool gen, int night)
{
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    exams = "";

}


int Staff::getId()
{   return id;  }

QString Staff::getFirstName()
{   return firstName;   }

QString Staff::getLastName()
{    return lastName;   }

bool Staff::getPosition()
{    return position;   }

bool Staff::getGender()
{    return gender;     }

int Staff::getNightClass()
{   return nightClass;  }

void Staff::setId(int i)
{   id = i;             }

void Staff::setFirstName(QString name)
{   firstName = name;   }

void Staff::setLastName(QString name)
{   lastName = name;    }

void Staff::setPosition(bool pos)
{   position = pos;     }

void Staff::setGender(bool gen)
{   gender = gen;       }

void Staff::setNightClass(int night)
{   nightClass = night; }


void Staff::setExams(QString ex)
{
    exams = ex;
}

QString Staff::getExams()
{
    return exams;
}

void Staff::setAvailability(const QList<QDate> &dtList)
{
    availList.clear();

    foreach(QDate dt, dtList) {
        availList.append(dt);
    }
}

void Staff::setAvailability(const QString &str) {
    availList.clear();

    QStringList dates = str.split(",", QString::SkipEmptyParts);
    foreach (QString dt, dates) {
        availList.append(QDate::fromString(dt, "dd/MM/yyyy"));
    }
}

void Staff::appendAvail(const QDate &dt) {
    availList.append(dt);
}

void Staff::removeAvail(const QDate &dt) {
    availList.removeAll(dt);
}

QList<QDate> Staff::getAvailability()
{
    return availList;
}

QString Staff::getAvailabilityStr() {
    QString out("");
    foreach (QDate dt, availList) {
        out += dt.toString("dd/MM/yyyy") + ",";
    }

    return out;
}

void Staff::addShift(bool weekend, bool isAM)
{
    if (isAM)
        numAMShifts++;

    if (weekend)
        numWeekendShifts++;

    numShifts++;
}

void Staff::removeShift(bool weekend, bool isAM)
{
    if (isAM)
        numAMShifts--;

    if (weekend)
        numWeekendShifts--;

    numShifts--;
}

int Staff::getShifts()
{
    return numShifts;
}

int Staff::getWeekendShifts()
{
    return numWeekendShifts;
}

int Staff::getAMShifts()
{
    return numAMShifts;
}

