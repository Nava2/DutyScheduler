#include "availabledate.h"

#include <QVariant>

AvailableDate::AvailableDate() :
    isRange(false)
{
}

QDate &AvailableDate::date() {
    return dateVal;
}

QDate &AvailableDate::endDate() {
    return endDateVal;
}

void AvailableDate::setDate(QDate date) {
    this->dateVal = date;
}

void AvailableDate::setEndDate(QDate endDate) {
    endDateVal = endDate;
    isRange = true;
}

void AvailableDate::operator <<(QVariantMap &map) {
    dateVal = map["date"].toDate();
    isRange = map["range"].toBool();

    if (isRange) {
        endDateVal = map["endDate"].toDate();
    }
}

void AvailableDate::operator >>(QVariantMap &out) {
    out["range"] = isRange;
    out["date"] = dateVal;

    if (isRange) {
        out["endDate"] = endDateVal;
    }
}
