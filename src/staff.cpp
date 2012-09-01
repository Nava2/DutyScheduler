#include "staff.h"

#include <QVariantMap>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

staff::staff()
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

staff::staff(int i, QString first, QString last, bool pos, bool gen, int night)
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

staff::staff(const QVariantMap &json) {

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
}

staff::~staff()
{

}

void staff::toJson(QVariantMap &json) {
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
}

void staff::update(QString first, QString last, bool pos, bool gen, int night)
{
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    exams = "";

}


int staff::getId()
{   return id;  }

QString staff::getFirstName()
{   return firstName;   }

QString staff::getLastName()
{    return lastName;   }

bool staff::getPosition()
{    return position;   }

bool staff::getGender()
{    return gender;     }

int staff::getNightClass()
{   return nightClass;  }

void staff::setId(int i)
{   id = i;             }

void staff::setFirstName(QString name)
{   firstName = name;   }

void staff::setLastName(QString name)
{   lastName = name;    }

void staff::setPosition(bool pos)
{   position = pos;     }

void staff::setGender(bool gen)
{   gender = gen;       }

void staff::setNightClass(int night)
{   nightClass = night; }


void staff::setExams(QString ex)
{
    exams = ex;
}

QString staff::getExams()
{
    return exams;
}

void staff::setAvailability(const QList<QDate> &dtList)
{
    availList.clear();

    foreach(QDate dt, dtList) {
        availList.append(dt);
    }
}

void staff::setAvailability(const QString &str) {
    availList.clear();

    QStringList dates = str.split(",", QString::SkipEmptyParts);
    foreach (QString dt, dates) {
        availList.append(QDate::fromString(dt, "dd/MM/yyyy"));
    }
}

void staff::appendAvail(const QDate &dt) {
    availList.append(dt);
}

void staff::removeAvail(const QDate &dt) {
    availList.removeAll(dt);
}

QList<QDate> staff::getAvailability()
{
    return availList;
}

QString staff::getAvailabilityStr() {
    QString out("");
    foreach (QDate dt, availList) {
        out += dt.toString("dd/MM/yyyy") + ",";
    }

    return out;
}

void staff::addShift(bool weekend, bool isAM)
{
    if (isAM)
        numAMShifts++;

    if (weekend)
        numWeekendShifts++;

    numShifts++;
}

void staff::removeShift(bool weekend, bool isAM)
{
    if (isAM)
        numAMShifts--;

    if (weekend)
        numWeekendShifts--;

    numShifts--;
}

int staff::getShifts()
{
    return numShifts;
}

int staff::getWeekendShifts()
{
    return numWeekendShifts;
}

int staff::getAMShifts()
{
    return numAMShifts;
}

