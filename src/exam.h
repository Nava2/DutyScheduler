#ifndef EXAM_H
#define EXAM_H

#include <QString>
#include <QVariantMap>

class exam
{
public:
    exam();
    exam(int, QString, bool);

    exam(const QVariantMap &);
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
