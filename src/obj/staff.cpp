#include "staff.h"

#include <QVariantMap>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QCryptographicHash>

Staff::Staff()
{
    id = 127;
    firstName = "Art";
    lastName = "Vandelay";
    position = false;
    gender = false;
    nightClass = 0;
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;

    _uid = "xxx";
    UIDSet = false;
}

Staff::Staff(int i, QString first, QString last, bool pos, bool gen, int night)
{
    id = i;
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;

    genUID();
}

Staff::Staff(const QVariantMap &json) :
    numShifts(0), numWeekendShifts(0), numAMShifts(0)
{
    *this << json;
}

Staff::Staff(const Staff &old) {
    if (this != &old) {
        this->id = old.id;
        firstName = old.firstName;
        lastName = old.lastName;
        position = old.position;
        gender = old.gender;

        finals = old.finals;
        numShifts = old.numShifts;
        numWeekendShifts = old.numWeekendShifts;
        numAMShifts = old.numAMShifts;

        genUID();
    }
}

void Staff::genUID() {
    QString id = firstName + lastName + QString::number((int)(rand() * 1000));
    QByteArray tuid = QCryptographicHash::hash(id.toUtf8(), QCryptographicHash::Sha1);
    tuid = tuid.toHex();
    _uid = QString(tuid.left(3) + tuid.right(3));

    UIDSet = true;
}

Staff::~Staff()
{

}

// JSON mapping:
void Staff::operator << (const QVariantMap &json) {
    this->id = json["id"].toInt();
    _uid = json["uid"].toString();
    UIDSet = !_uid.isEmpty();

    midterms.clear();
    finals.clear();

    QVariantMap name = json["name"].toMap();
    firstName = name["f"].toString();
    lastName = name["l"].toString();

    position = static_cast<bool>(json["pos"].toInt());
    gender = static_cast<bool>(json["gndr"].toInt());
    nightClass = json["night"].toInt();

    foreach (QVariant qv, json["avail"].toList()) {
        AvailableDate d;
        d << qv.toMap();
        availList.append(d);
    }

    QVariantMap shiftMap = json["shifts"].toMap();
    numWeekendShifts = shiftMap["wkend"].toInt();
    if (position)
       numAMShifts = shiftMap["am"].toInt();
    numShifts = shiftMap["ttl"].toInt();

    if (!UIDSet)
        genUID();
}

void Staff::operator >>(QVariantMap &json) {
    json["id"] = id;
    json["uid"] = uid();

    QVariantMap name;
    name["f"] = firstName;
    name["l"] = lastName;
    json["name"] = name;

    json["pos"] = static_cast<int>(position);
    json["gndr"] = static_cast<int>(gender);
    json["night"] = nightClass;

    QVariantList list;
    foreach (AvailableDate d, availList) {
        QVariantMap v;
        d >> v;
        list.append(QVariant(v));
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
}


int Staff::getId() const
{   return id;  }

QString Staff::getFirstName() const
{   return firstName;   }

QString Staff::getLastName() const
{    return lastName;   }

bool Staff::getPosition() const
{    return position;   }

bool Staff::getGender() const
{    return gender;     }

int Staff::getNightClass() const
{   return nightClass;  }

bool Staff::isNightClass(const QDate &date) const {
    int day = date.dayOfWeek() - 1;

    return isNightClass(day);
}

bool Staff::isNightClass(const int day) const {
    return ((nightClass & (0x1 << day)) > 0);
}

void Staff::setId(int i)
{   id = i;             }

void Staff::setName(const QString &first, const QString &last) {
    firstName = first;
    lastName = last;
}

void Staff::setPosition(bool male)
{   position = male;     }

void Staff::setGender(bool male)
{   gender = male;       }

void Staff::setNightClass(int night)
{   nightClass = night; }

bool Staff::examListContains(const Exam::Ptr e, const QList<Exam::Ptr> &list) {
    foreach (Exam::Ptr ptr, list) {
        if (*ptr == *e)
            return true;
    }

    return false;
}

// kept for compatibility
void Staff::setFinals(QString ex, const QList<Exam::Ptr> &examList)
{
    finals.clear();
    QStringList list = ex.split(',', QString::SkipEmptyParts);
    foreach (QString ex, list) {
        ex.remove("(", Qt::CaseInsensitive);
        ex.remove(")", Qt::CaseInsensitive);

        int id = ex.toInt();
        finals.append(examList[id]);
        examList[id]->addStaff(uid());
    }
}

void Staff::setMidterms(const QList<Exam::Ptr> &exams) {
    midterms = exams;

    foreach (Exam::Ptr ptr, midterms) {
        ptr->addStaff(uid());
    }
}

void Staff::addMidterm(const Exam::Ptr e) {
    if (!examListContains(e, midterms)) {
        midterms.append(e);
        e->addStaff(uid());
    }
}

QList<Exam::Ptr> Staff::getMidterms() {
    return midterms;
}

void Staff::setFinals(const QList<Exam::Ptr> &exams) {
    finals = exams;

    foreach (Exam::Ptr ptr, finals) {
        ptr->addStaff(uid());
    }
}

void Staff::addFinal(const Exam::Ptr e) {
    if (!examListContains(e, finals)) {
        finals.append(e);
        e->addStaff(uid());
    }
}

QList<Exam::Ptr> Staff::getFinals() const
{
    return finals;
}

QString Staff::getFinalsStr() const {
    QString out;
    foreach (Exam::Ptr ex, finals) {
        out += "(" + QString::number(ex->getId()) + "),";
    }

    return out;
}

void Staff::setAvailability(const QList<AvailableDate > &dtList)
{
    availList.clear();

    foreach(AvailableDate dt, dtList) {
        availList.append(dt);
    }
}

void Staff::setAvailability(const QString &str) {
    availList.clear();

    QStringList dates = str.split(",", QString::SkipEmptyParts);
    foreach (QString dt, dates) {
        QDate first = QDate::fromString(dt, "dd/MM/yyyy");
        AvailableDate d;
        d.setDate(first);
        availList.append(d);
    }
}

void Staff::appendAvail(const AvailableDate &dt) {
    availList.append(dt);
}

void Staff::removeAvail(const AvailableDate &dt) {
    availList.removeAll(dt);
}

void Staff::getAvailability(QList<QDate> &out)
{
    out.clear();

    foreach (AvailableDate ad, availList) {
        // get the dates from the AvailableDate, let it handle the range notion
        foreach (QDate qd, ad.getDates()) {
            out.append(qd);
        }
    }
}

void Staff::getAvailability(QList<AvailableDate> &out) {
    out.clear();

    foreach (AvailableDate ad, availList)
        out += ad;
}

QString Staff::getAvailabilityStr() {
    QString out("");

    foreach (AvailableDate ad, availList) {
        // get the dates from the AvailableDate, let it handle the range notion
        foreach (QDate qd, ad.getDates()) {
            out += qd.toString("dd/MM/yyyy") + ",";
        }
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

bool Staff::isUIDSet() const {
    return UIDSet;
}
QString Staff::uid() const {
    return _uid;
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

