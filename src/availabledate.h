#ifndef AVAILABLEDATE_H
#define AVAILABLEDATE_H

#include <QDate>
#include <QVariant>
#include <QVariantMap>

class AvailableDate
{
public:
    AvailableDate();
    AvailableDate(const QDate &date, const bool = false);
    virtual
    ~AvailableDate();

    QDate date() const;

    QDate endDate() const;

    void setDate(QDate date);

    void setEndDate(QDate endDate);

    QList<QDate > getDates() const;

    bool isRange() const;
    void setRange(const bool);

    AvailableDate &operator =(const AvailableDate &copy);

    void operator <<(const QVariantMap &);
    void operator >>(QVariantMap &) const;

    bool operator ==(const AvailableDate &date) const;
    bool operator !=(const AvailableDate &date) const;


private:
    QDate dateVal;
    QDate endDateVal;

    bool _isRange;


};

#endif // AVAILABLEDATE_H
