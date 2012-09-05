#ifndef SDATE_H
#define SDATE_H

#include <QDate>
#include <QList>
#include <QString>
#include <QVariantMap>

class SDate : public QDate
{

public:
    SDate();
    SDate(const QDate &, int, int);
    SDate(const QVariantMap &map);
    ~SDate();

    bool isSpecial(); // is this special duty?
    bool isOn(int) const;// is this staff on duty already?
    bool isFull() const;
    bool rasFull() const; // do we have enough ras?
    bool donsFull() const;// do we have enough dons? including RA.
    bool canWork(int) const;//can someone work?

    void setSpecial(bool);//set the special duty flag


    int getAM() const;//return the AM's ID
    int getWeekday() const;
    int getRasNeeded() const;
    void setRasNeeded(const int);

    int getDonsNeeded() const;
    void setDonsNeeded(const int);

    bool isDefaultNeeded() const;

    bool isWeekend() const;

    QString getCantWorkStr();
    QList<int > getCantWork() {
        return cantWork;
    }

    QString getDonsStr();
    QList<int > getDons() {
        return donsOn;
    }

    QString getRasStr();
    QList<int > getRas() {
        return rasOn;
    }

    QString exportOn();

    void setAM(int);//set the AM with their id
    void addStaff(int,bool); // add a staff to be on duty
    void removeStaff(int, bool);

    void addCantWork(int);
    bool staffCantWork(int);

    void operator >>(QVariantMap &);
    void operator <<(const QVariantMap &);


private:
    QDate theDate;
    bool spDuty;
    int AM;
    QList<int> donsOn;
    QList<int> rasOn;
    QList<int> cantWork;
    bool defaultNeededD, defaultNeededR;
    int rasNeeded;
    int donsNeeded;
    int weekday;
};

#endif // SDATE_H
