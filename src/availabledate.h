#ifndef AVAILABLEDATE_H
#define AVAILABLEDATE_H

#include <QDate>
#include <QVariant>
#include <QVariantMap>

class AvailableDate
{
public:
    AvailableDate();

    QDate date() const;

    QDate endDate() const;

    void setDate(QDate date);

    void setEndDate(QDate endDate);

    QList<QDate > getDates() const;

    bool isRange() const;
    void setRange(const bool);

    void operator <<(const QVariantMap &);
    void operator >>(QVariantMap &) const;

    inline
    bool operator ==(const AvailableDate &date) const;
    inline
    bool operator !=(const AvailableDate &date) const;


private:
    QDate dateVal;
    QDate endDateVal;

    bool _isRange;


};

#endif // AVAILABLEDATE_H
