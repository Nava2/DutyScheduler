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
    enum ShiftType {
        NONE = 0x00,
        AM = 0x01, TOTAL = 0x83,
        DAY = 0x80, NIGHT = 0x02, WEEKEND = 0x06
    };
    Q_DECLARE_FLAGS(ShiftTypes, ShiftType)

    typedef QSharedPointer<Staff> Ptr;

    Staff();
    Staff(int, QString, QString, bool, bool, int);
    Staff(const QVariantMap &);
    Staff(const Staff &);
    ~Staff();

    void operator >>(QVariantMap &);
    void operator <<(const QVariantMap &);

    //getters
    int getId() const;
    QString getFirstName() const;
    QString getLastName() const;

    /*!
     * \brief getPosition
     * \return True if Don, false if RA
     */
    bool getPosition() const;

    /*!
     * \brief getGender
     * \return True if Male
     */
    bool getGender() const;

    /*!
     * \brief getNightClass
     * \return Integer where the i'th bit is high when night class
     */
    int getNightClass() const;

    bool isNightClass(const QDate &date) const;
    bool isNightClass(const int day) const;

    void setFinals(QString, const QList<Exam::Ptr> &examList);
    void setFinals(const QList<Exam::Ptr> &);
    void addFinal(const Exam::Ptr e);

    QList<Exam::Ptr> getFinals() const;
    QString getFinalsStr() const;

    void setMidterms(const QList<Exam::Ptr> &);
    void addMidterm(const Exam::Ptr e);
    QList<Exam::Ptr> getMidterms();
    QString getMidtermsStr();

    int getShifts(ShiftTypes type = TOTAL);
    int getWeekendShifts();
    int getAMShifts();
    
    void setShifts(int total, int weekend, int day, int am);


    //setters
    void update(QString, QString, bool, bool, int);
    void setId(int);
    void setName(const QString &first, const QString &last);

    /*!
     * \brief setPosition
     * \param don True if Don
     */
    void setPosition(bool don);

    /*!
     * \brief setGender
     * \param male True if male
     */
    void setGender(bool male);
    void setNightClass(int);
\

    void setAvailability(const QList<AvailableDate> &dtList);
    void setAvailability(const QString &);
    void getAvailability(QList<QDate> &);
    void getAvailability(QList<AvailableDate> &);
    QString getAvailabilityStr();
    void appendAvail(const AvailableDate &dt);
    void removeAvail(const AvailableDate &dt);

    void addShift(ShiftTypes type);
    void removeShift(ShiftTypes type);
    
    

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
    int numDayDuty;
    int numWeekendShifts;
    int numAMShifts;

    void genUID();

    bool examListContains(const Exam::Ptr e, const QList<Exam::Ptr> &list);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Staff::ShiftTypes)

#endif // STAFF_H
