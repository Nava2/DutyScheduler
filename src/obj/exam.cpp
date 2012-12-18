#include "exam.h"

#include <QDate>

Exam::Exam()
    : QDate(), id(9999), period(AFTERNOON), midterm(false)
{
}

Exam::Exam(const int i, const QDate &date, const bool midterm, const Exam::Period period)
    : QDate(date), period(period), midterm(midterm)
{
    this->id = i;
}

Exam::Exam(const QVariantMap &json)
    : QDate(), id(0), period(AFTERNOON), midterm(false) {
    *this << json;
}

Exam::Exam(const Exam &old)
    : QDate(old) {
    if (this != &old) {
        this->id = old.id;
        period = old.period;
        midterm = old.midterm;
    }
}
void Exam::setPeriod(const Exam::Period period) {
    this->period = period;
}

Exam::Period Exam::getPeriod() const {
    return period;
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

QList<QString > Exam::getStaff() {
    return staffIds;
}

// JSON IO
// in
void Exam::operator <<(const QVariantMap &json) {
    id = json["id"].toInt();
    QDate date = json["date"].toDate();
    setYMD(date.year(), date.month(), date.day());

    QVariantMap::const_iterator it = json.find("period");
    if (it == json.end()) {
        if (json["night"].toBool()) {
            period = NIGHT;
        } else {
            period = AFTERNOON;
        }
    } else {
        period = static_cast<Period>(it->toInt());
    }

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
    json["date"] = *(dynamic_cast<QDate*>(this));
    json["period"] = static_cast<int>(period);
    json["mid"] = midterm;

    QVariantList t_idList;
    foreach (QString uid, staffIds) {
        t_idList.append(QVariant(uid));
    }
    json["ids"] = t_idList;
}

bool Exam::operator ==(const Exam &ex) {
    return ((QDate)(*this) == (QDate)(ex))
            && period == ex.period
            && midterm == ex.midterm;
}

bool Exam::operator !=(const Exam &ex) {
    return !(*this == ex);
}
