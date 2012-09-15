#ifndef EXAM_H
#define EXAM_H

#include <QString>
#include <QVariantMap>
#include <QDate>
#include <QSharedPointer>

class Exam : public QDate
{
public:
    typedef QSharedPointer<Exam> Ptr;

    Exam();
    Exam(const int, const QDate &, const bool, const bool);
    Exam(const Exam &);
    Exam(const QVariantMap &);

    void operator <<(const QVariantMap &);
    void operator >>(QVariantMap &);
    bool operator ==(const Exam &ex);
    bool operator !=(const Exam &ex);

    void setNight(bool);

    bool isNight();
    int getId();

    bool isMidterm() const;
    void setMidterm(const bool);

    void addStaff(const QString &id);
    void removeStaff(const QString &id);

private:
    int id;
    bool night;
    bool midterm;

    QList<QString > staffIds;
};

#endif // EXAM_H
