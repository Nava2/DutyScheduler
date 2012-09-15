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
    Exam(const int, const QDate &, const bool);
    Exam(const Exam &);
    Exam(const QVariantMap &);

    void operator <<(const QVariantMap &);
    void operator >>(QVariantMap &);
    bool operator ==(const Exam &ex);
    bool operator !=(const Exam &ex);

    void setNight(bool);

    bool getNight();
    int getId();

    void addStaff(const QString &id);
    void removeStaff(const QString &id);

private:
    int id;
    bool night;

    QList<QString > staffIds;
};

#endif // EXAM_H
