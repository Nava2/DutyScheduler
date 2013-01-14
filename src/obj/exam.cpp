#include "exam.h"

#include <QDate>

Exam::Exam()
    : QDate(), id(9999), _period(AFTERNOON)
{
}

Exam::Exam(const int i, const QDate &date, const Exam::Period period)
    : QDate(date), _period(period)
{
    this->id = i;
}

Exam::Exam(const QVariantMap &json)
    : QDate(), id(0), _period(AFTERNOON) {
    *this << json;
}

Exam::Exam(const Exam &old)
    : QDate(old) {
    if (this != &old) {
        this->id = old.id;
        _period = old._period;
    }
}
void Exam::setPeriod(const Exam::Period period) {
    this->_period = period;
}

Exam::Period Exam::getPeriod() const {
    return _period;
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
            _period = NIGHT;
        } else {
            _period = AFTERNOON;
        }
    } else {
        _period = static_cast<Period>(it->toInt());
    }

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
    json["period"] = static_cast<int>(_period);

    QVariantList t_idList;
    foreach (QString uid, staffIds) {
        t_idList.append(QVariant(uid));
    }
    json["ids"] = t_idList;
}

bool Exam::operator ==(const Exam &ex) {
    return ((QDate)(*this) == (QDate)(ex))
            && _period == ex._period;
}

bool Exam::operator !=(const Exam &ex) {
    return !(*this == ex);
}
