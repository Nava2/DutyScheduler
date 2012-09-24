#ifndef SDATE_H
#define SDATE_H

#include <QDate>
#include <QList>
#include <QString>
#include <QVariantMap>

#include "staff.h"
#include "stafflist.h"

class SDate : public QDate
{

public:
    SDate();
    SDate(const QDate &, int, int);
    SDate(const QVariantMap &map);
    ~SDate();

    bool isSpecial(); // is this special duty?
    bool isOn(const QString &) const;// is this staff on duty already?
    bool isFull() const;
    bool rasFull() const; // do we have enough ras?
    bool donsFull() const;// do we have enough dons? including RA.
    bool canWork(const QString &) const;//can someone work?

    void setSpecial(bool);//set the special duty flag


    QString getAM() const;//return the AM's ID
    int getWeekday() const;
    int getRasNeeded() const;
    void setRasNeeded(const int);

    int getDonsNeeded() const;
    void setDonsNeeded(const int);

    bool isDefaultNeeded() const;

    bool isWeekend() const;

    QString getCantWorkStr();
    QList<QString > getCantWork() {
        return cantWork;
    }

    QString getDonsStr();
    QList<QString > getDons() {
        return donsOn;
    }

    QString getRasStr();
    QList<QString > getRas() {
        return rasOn;
    }

    QString exportOn() const;

    bool exportOn(const StaffList &team,
                  Staff::Ptr &am,
                  QList<Staff::Ptr> &dons, QList<Staff::Ptr> &ras) const;

    void setAM(const QString &);//set the AM with their id
    void addStaff(const QString &s,bool); // add a staff to be on duty
    void removeStaff(const QString &s, bool);

    void addCantWork(const QString &);
    void addCantWork(const QList<QString > &);
    bool staffCantWork(const QString &);

    void operator >>(QVariantMap &);
    void operator <<(const QVariantMap &);

    static
    const QString AM_NOT_SET;

private:
    bool spDuty;
    QString AM;
    QList<QString > donsOn;
    QList<QString > rasOn;
    QList<QString > cantWork;
    bool defaultNeededD, defaultNeededR;
    int rasNeeded;
    int donsNeeded;
    int weekday;
};

#endif // SDATE_H
