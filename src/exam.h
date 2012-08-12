#ifndef EXAM_H
#define EXAM_H

#include <QString>

class exam
{
public:
    exam();
    exam(int, QString, bool);

    void setDate(QString);
    void setNight(bool);

    QString getDate();
    bool getNight();
    int getId();

private:
    int id;
    QString date;
    bool night;
};

#endif // EXAM_H
