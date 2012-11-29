#ifndef STAFFLIST_H
#define STAFFLIST_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QSharedPointer>
#include <QVariantMap>

#include "staff.h"

class StaffList : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<StaffList> Ptr;

    typedef QList<Staff::Ptr>::const_iterator const_iterator;

    explicit StaffList(QObject *parent = 0);
    StaffList(const StaffList &list);

    virtual
    ~StaffList();

    StaffList &append(const Staff::Ptr &);
    StaffList &append(const QList<Staff::Ptr> &list);

    StaffList &remove(const Staff::Ptr &);

    int count() const;
    int countRA() const;
    int countDon() const;

    int size() const;

    void nightClasses(QList<QList<QString> > &_nightClasses) const;

    void clear();

    Staff::Ptr at(const int index) const;
    Staff::Ptr at(const QString &uid) const;
    Staff::Ptr operator [](const int index) const;
    Staff::Ptr operator [](const QString &uid) const;

    bool contains(const QString &uid) const;
    bool contains(const int id) const;
    
    StaffList &operator <<(const QVariantMap &json);
    StaffList &operator >>(QVariantMap &json);

    StaffList &operator =(const StaffList &list);

    const_iterator begin() const;
    const_iterator end() const;


signals:
    
public slots:

private:
    QList<Staff::Ptr> _list;
    QHash<QString, Staff::Ptr> _hashList;

    int _raCount, _donCount;
    
};

#endif // STAFFLIST_H
