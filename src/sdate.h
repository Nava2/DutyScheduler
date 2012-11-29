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

    bool isSpecial() const; // is this special duty?
    bool isOn(const QString &) const;// is this staff on duty already?
    bool isFull() const;
    bool isExam() const;
    bool rasFull() const; // do we have enough ras?
    bool donsFull() const;// do we have enough dons? including RA.
    bool canWork(const QString &) const;//can someone work?

    void setSpecial(bool);//set the special duty flag


    QString getAM() const;//return the AM's ID
    int getWeekday() const;

    /*!
     * \brief getRasLeft
     * \return Number of RAs left to schedule
     */
    int getRasLeft() const;

    /*!
     * \brief getRAsNeeded
     * \return Get the total RAs needed
     */
    int getRAsNeeded() const;

    /*!
     * \brief setRasNeeded Set the number of RAs for a day
     */
    void setRasNeeded(const int);

    /*!
     * \brief getDonsNeeded
     * \return Number of Dons left to schedule
     */
    int getDonsLeft() const;

    /*!
     * \brief getDonsNeeded
     * \return Number of Dons needed in total
     */
    int getDonsNeeded() const;

    void setDonsNeeded(const int);

    bool isDefaultNeeded() const;

    bool isWeekend() const;

    QString getCantWorkStr() const;
    QList<QString > getCantWork() const ;

    QString getDonsStr() const ;
    QList<QString > getDons() const;

    QString getRasStr() const;
    QList<QString > getRas() const;

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
    bool examDay;
    int rasNeeded;
    int donsNeeded;
    int weekday;
};

#endif // SDATE_H
