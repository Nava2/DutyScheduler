#ifndef EXAM_H
#define EXAM_H

#include <QString>
#include <QVariantMap>

class Exam
{
public:
    Exam();
    Exam(int, QString, bool);

    Exam(const QVariantMap &);
    void toJson(QVariantMap &);

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
