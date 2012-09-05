#ifndef STAFF_H
#define STAFF_H

#include <QString>
#include <QVariant>
#include <QDate>

#include "exam.h"

class Staff
{

public:
    Staff();
    Staff(int, QString, QString, bool, bool, int);
    Staff(const QVariantMap &);
    ~Staff();

    void operator >>(QVariantMap &);
    void operator <<(const QVariantMap &);

    //getters
    int getId();
    QString getFirstName();
    QString getLastName();
    bool getPosition();
    bool getGender();
    int getNightClass();
    QString getExams();

    int getShifts();
    int getWeekendShifts();
    int getAMShifts();


    //setters
    void update(QString, QString, bool, bool, int);
    void setId(int);
    void setFirstName(QString);
    void setLastName(QString);
    void setPosition(bool);
    void setGender(bool);
    void setNightClass(int);
    void setExams(QString);

    void setAvailability(const QList<QDate> &dtList);
    void setAvailability(const QString &);
    QList<QDate> getAvailability();
    QString getAvailabilityStr();
    void appendAvail(const QDate &dt);
    void removeAvail(const QDate &dt);

    void addShift(bool, bool);
    void removeShift(bool, bool);



private:
    int id;
    QString firstName;
    QString lastName;
    bool position;
    bool gender;
    int nightClass;
    QString exams; // "(id),(id2),(id5),"
//     QString availability;// "dd/MM/yyyy,dd/MM/yyyy," etc
    QList<QDate > availList;
    int numShifts;
    int numWeekendShifts;
    int numAMShifts;

};

#endif // STAFF_H
