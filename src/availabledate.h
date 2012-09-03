#ifndef AVAILABLEDATE_H
#define AVAILABLEDATE_H

#include <QDate>
#include <QVariant>
#include <QVariantMap>

class AvailableDate
{
public:
    AvailableDate();

    QDate &date();

    QDate &endDate();

    void setDate(QDate date);

    void setEndDate(QDate endDate);

    void operator <<(QVariantMap &);
    void operator >>(QVariantMap &);


private:
    QDate dateVal;
    QDate endDateVal;

    bool isRange;


};

#endif // AVAILABLEDATE_H
