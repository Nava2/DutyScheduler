#ifndef STAFF_H
#define STAFF_H

#include <QString>
#include "exam.h"

class staff
{

public:
    staff();
    staff(int, QString, QString, bool, bool, int);
    ~staff();

    //getters
    int getId();
    QString getFirstName();
    QString getLastName();
    bool getPosition();
    bool getGender();
    int getNightClass();
    QString getExams();
    QString getAvailability();
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
    void setAvailability(QString);
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
    QString availability;// "dd/MM/yyyy,dd/MM/yyyy," etc
    int numShifts;
    int numWeekendShifts;
    int numAMShifts;

};

#endif // STAFF_H
