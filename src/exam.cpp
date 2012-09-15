#include "exam.h"

#include <QDate>

Exam::Exam()
    : QDate()
{
    id = 99999;
    night = false;
}

Exam::Exam(const int i, const QDate &date, const bool n)
    : QDate(date)
{
    this->id = i;
    night = n;
}

Exam::Exam(const QVariantMap &json)
    : QDate(), id(0), night(0) {
    *this << json;
}

Exam::Exam(const Exam &old) {
    this->id = old.id;
    setYMD(old.year(), old.month(), old.day());
    night = old.night;
}

void Exam::setNight(bool n)
{
    night = n;
}

bool Exam::getNight()
{
    return night;
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

    QVariantList t_idList;
    foreach (QString uid, staffIds) {
        t_idList.append(QVariant(uid));
    }
    json["ids"] = t_idList;
}

bool Exam::operator ==(const Exam &ex) {
    return ((QDate)(*this) == (QDate)(ex)) && night == ex.night;
}

bool Exam::operator !=(const Exam &ex) {
    return !(*this == ex);
}
