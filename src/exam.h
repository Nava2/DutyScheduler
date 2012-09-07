#ifndef EXAM_H
#define EXAM_H

#include <QString>
#include <QVariantMap>
#include <QDate>

class Exam : public QDate
{
public:
    Exam();
    Exam(int, QString, bool);
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
