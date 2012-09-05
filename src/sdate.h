#ifndef SDATE_H
#define SDATE_H

#include <QDate>
#include <QList>
#include <QString>
#include <QVariantMap>

class SDate : public QObject
{
    Q_OBJECT

public:
    SDate();
    SDate(QDate, int, int);
    SDate(const QVariantMap &map);
    ~SDate();

    QDate getDate();//return the date of this sDate
    bool isSpecial(); // is this special duty?
    bool isOn(int);// is this staff on duty already?
    bool isFull();
    bool rasFull(); // do we have enough ras?
    bool donsFull();// do we have enough dons? including RA.
    bool canWork(int);//can someone work?

    void setSpecial(bool);//set the special duty flag


    int getAM();//return the AM's ID
    int getWeekday();
    int getRasNeeded();
    int getDonsNeeded();
    QString getCantWork();
    QString getDons();
    QString getRas();

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
    QList<int> *donsOn;
    QList<int> *rasOn;
    QList<int> *cantWork;
    int rasNeeded;
    int donsNeeded;
    int weekday;
};

#endif // SDATE_H
