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

    void setNight(bool);

    bool getNight();
    int getId();

private:
    int id;
    bool night;
};

#endif // EXAM_H
