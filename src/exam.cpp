#include "exam.h"

Exam::Exam()
{
    id = 99999;
    date = "";
    night = false;
}

Exam::Exam(int i, QString d, bool n)
{
    id = i;
    date = d;
    night = n;
}

Exam::Exam(const QVariantMap &json) {
    id = json["id"].toInt();
    date = json["date"].toString();
    night = json["night"].toBool();
}

void Exam::toJson(QVariantMap &json) {
    json["id"] = id;
    json["date"] = date;
    json["night"] = night;
}

void Exam::setDate(QString d)
{
    date = d;
}

void Exam::setNight(bool n)
{
    night = n;
}

QString Exam::getDate()
{
    return date;
}

bool Exam::getNight()
{
    return night;
}

int Exam::getId()
{
    return id;
}
