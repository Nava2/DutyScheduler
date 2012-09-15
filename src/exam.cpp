#include "exam.h"

#include <QDate>

Exam::Exam()
    : QDate(), id(9999), night(false), midterm(false)
{
}

Exam::Exam(const int i, const QDate &date, const bool midterm, const bool night)
    : QDate(date), night(night), midterm(midterm)
{
    this->id = i;
}

Exam::Exam(const QVariantMap &json)
    : QDate(), id(0), night(false), midterm(false) {
    *this << json;
}

Exam::Exam(const Exam &old)
    : QDate(old) {
    this->id = old.id;
    night = old.night;
    midterm = old.midterm;
}

void Exam::setNight(bool n)
{
    night = n;
}

bool Exam::isNight()
{
    return night;
}

bool Exam::isMidterm() const {
    return midterm;
}

void Exam::setMidterm(const bool midterm) {
    this->midterm = midterm;
}

int Exam::getId()
{
    return id;
}

void Exam::addStaff(const QString &id) {
    if (!staffIds.contains(id))
        staffIds.append(id);
}

void Exam::removeStaff(const QString &id) {
    staffIds.removeAll(id);
}

// JSON IO
// in
void Exam::operator <<(const QVariantMap &json) {
    id = json["id"].toInt();
    QDate date = json["date"].toDate();
    setYMD(date.year(), date.month(), date.day());
    night = json["night"].toBool();
    midterm = json["mid"].toBool();

    QVariantList list = json["ids"].toList();
    staffIds.clear();
    foreach (QVariant uid, list) {
        staffIds.append(uid.toString());
    }
}

// out
void Exam::operator >>(QVariantMap &json) {
    json["id"] = id;
    json["date"] = (QDate)(*this);
    json["night"] = night;
    json["mid"] = midterm;

    QVariantList t_idList;
    foreach (QString uid, staffIds) {
        t_idList.append(QVariant(uid));
    }
    json["ids"] = t_idList;
}

bool Exam::operator ==(const Exam &ex) {
    return ((QDate)(*this) == (QDate)(ex))
            && night == ex.night
            && midterm == ex.midterm;
}

bool Exam::operator !=(const Exam &ex) {
    return !(*this == ex);
}
