#include "availabledate.h"

#include <QVariant>

AvailableDate::AvailableDate() :
    _isRange(false)
{
}

QDate AvailableDate::date() const {
    return dateVal;
}

QDate AvailableDate::endDate() const {
    return endDateVal;
}

void AvailableDate::setDate(QDate date) {
    this->dateVal = date;
}

void AvailableDate::setEndDate(QDate endDate) {
    endDateVal = endDate;
    _isRange = true;
}

QList<QDate > AvailableDate::getDates() const {
    QList<QDate > out;
    if (!isRange()) {
        out.append(dateVal);
        return out;
    }

    // it's a range:
    QDate counter = dateVal,
            endVal = endDateVal.addDays(1);
    while (counter != endVal) {
        out.append(counter);
        counter = counter.addDays(1);
    }

    return out;
}

bool AvailableDate::isRange() const {
    return _isRange;
}

void AvailableDate::setRange(const bool range) {
    _isRange = range;
}

void AvailableDate::operator <<(const QVariantMap &map) {
    dateVal = map["date"].toDate();
    _isRange = map["range"].toBool();
    endDateVal = map["endDate"].toDate();
}

void AvailableDate::operator >>(QVariantMap &out) const {
    out["range"] = _isRange;
    out["date"] = dateVal;
    out["endDate"] = endDateVal;
}

inline
bool AvailableDate::operator ==(const AvailableDate &date) const {
    bool result = dateVal == date.dateVal;
    result &= isRange() == date.isRange();
    if (result && isRange()) {
        result &= date.endDateVal == endDateVal;
    }

    return result;
}

inline
bool AvailableDate::operator !=(const AvailableDate &date) const {
    return !(*this == date);
}
