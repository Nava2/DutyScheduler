#ifndef STAFF_H
#define STAFF_H

#include <QString>
#include <QVariant>
#include <QDate>
#include <QSharedPointer>

#include "exam.h"
#include "availabledate.h"

class Staff
{

public:
    typedef QSharedPointer<Staff> Ptr;

    Staff();
    Staff(int, QString, QString, bool, bool, int);
    Staff(const QVariantMap &);
    Staff(const Staff &);
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
    bool isNightClass(const QDate &date) const;
    bool isNightClass(const int day) const;

    void setFinals(QString, const QList<Exam::Ptr> &examList);
    void setFinals(const QList<Exam::Ptr> &);
    void addFinal(const Exam::Ptr e);
    QList<Exam::Ptr> getFinals();
    QString getFinalsStr();

    void setMidterms(const QList<Exam::Ptr> &);
    void addMidterm(const Exam::Ptr e);
    QList<Exam::Ptr> getMidterms();
    QString getMidtermsStr();

    int getShifts();
    int getWeekendShifts();
    int getAMShifts();


    //setters
    void update(QString, QString, bool, bool, int);
    void setId(int);
    void setName(const QString &first, const QString &last);
    void setPosition(bool);
    void setGender(bool);
    void setNightClass(int);
\

    void setAvailability(const QList<AvailableDate> &dtList);
    void setAvailability(const QString &);
    void getAvailability(QList<QDate> &);
    void getAvailability(QList<AvailableDate> &);
    QString getAvailabilityStr();
    void appendAvail(const AvailableDate &dt);
    void removeAvail(const AvailableDate &dt);

    void addShift(bool, bool = false);
    void removeShift(bool, bool);

    bool isUIDSet() const;
    QString uid() const;

private:
    int id;
    bool UIDSet;
    QString _uid;
    QString firstName;
    QString lastName;
    bool position;
    bool gender;
    int nightClass;
    QList<Exam::Ptr> finals, midterms; // "(id),(id2),(id5),"
//     QString availability;// "dd/MM/yyyy,dd/MM/yyyy," etc
    QList<AvailableDate > availList;
    int numShifts;
    int numWeekendShifts;
    int numAMShifts;

    void genUID();

    bool examListContains(const Exam::Ptr e, const QList<Exam::Ptr> &list);

};

#endif // STAFF_H
