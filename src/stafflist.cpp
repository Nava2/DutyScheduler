#include "stafflist.h"

#include <QList>
#include <QHash>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QVariant>

#include <QDebug>

#include "staff.h"

StaffList::StaffList(QObject *parent) :
    QObject(parent)
{
}

StaffList::~StaffList() {

}

StaffList::StaffList(const StaffList &list) :
    QObject()
{
    foreach (Staff::Ptr p, list._list) {
        _list.append(p);
    }

    foreach (QString key, list._hashList.keys()) {
        _hashList[key] = list._hashList[key];
    }
}

StaffList &StaffList::append(const Staff::Ptr &ptr) {
    if (_list.contains(ptr))
        return *this;

    _list.append(ptr);
    _hashList[ptr->uid()] = ptr;

    if (!ptr->isUIDSet()) {
        qDebug() << "WARNING: UID NOT SET IN STAFFLIST::APPEND";
    }

    return *this;
}

StaffList &StaffList::append(const QList<Staff::Ptr> &list) {
    foreach (Staff::Ptr ptr, list) {
        append(ptr);
    }

    return *this;
}

StaffList &StaffList::remove(const Staff::Ptr &ptr) {
    _list.removeAll(ptr);
    _hashList.remove(ptr->uid());

    return *this;
}

int StaffList::count() const {
    return _list.count();
}

int StaffList::size() const {
    return count();
}

void StaffList::nightClasses(QList<QList<QString> > &_nightClasses) const {
    _nightClasses.clear();
    for (int i = 0; i < 7; i++) {
        _nightClasses += QList<QString >();
    }

    foreach (Staff::Ptr pstaff, _list) {
        QString id = pstaff->uid();

        for (int i = 0; i < 7; i++) {
            if (pstaff->isNightClass(i))
                _nightClasses[i] += id;
        }
    }
}

void StaffList::clear() {
    _list.clear();
    _hashList.clear();
}


Staff::Ptr StaffList::at(const int index) const {
    // let underlying list handle errors :<
    return _list[index];
}

Staff::Ptr StaffList::operator [](const int index) const {
    return at(index);
}


Staff::Ptr StaffList::at(const QString &uid) const {
    return _hashList[uid];
}

bool StaffList::contains(const QString &uid) const {
    return _hashList.contains(uid);
}

bool StaffList::contains(const int id) const {
    return (id > 0) && (id < _list.size());
}

Staff::Ptr StaffList::operator [](const QString &uid) const {
    return at(uid);
}

StaffList &StaffList::operator <<(const QVariantMap &json) {
    QVariantList list = json["team"].toList();
    foreach (QVariant vStaff, list) {
        Staff::Ptr ptr(new Staff(vStaff.toMap()));
        append(ptr);
    }

    return *this;
}

StaffList &StaffList::operator >>(QVariantMap &json) {
    QVariantList list;
    foreach (Staff::Ptr ptr, _list) {
        QVariantMap v;
        *ptr >> v;
        list.append(v);
    }

    json["team"] = list;

    return *this;
}

StaffList &StaffList::operator =(const StaffList &list) {
    _list = list._list;
    _hashList = list._hashList;

    return *this;
}

StaffList::const_iterator StaffList::begin() const {
    return _list.begin();
}

StaffList::const_iterator StaffList::end() const {
    return _list.end();
}



