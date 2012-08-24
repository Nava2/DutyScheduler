#include "exam.h"

exam::exam()
{
    id = 99999;
    date = "";
    night = false;
}

exam::exam(int i, QString d, bool n)
{
    id = i;
    date = d;
    night = n;
}

exam::exam(const QVariantMap &json) {
    id = json["id"].toInt();
    date = json["date"].toString();
    night = json["night"].toBool();
}

void exam::toJson(QVariantMap &json) {
    json["id"] = id;
    json["date"] = date;
    json["night"] = night;
}

void exam::setDate(QString d)
{
    date = d;
}

void exam::setNight(bool n)
{
    night = n;
}

QString exam::getDate()
{
    return date;
}

bool exam::getNight()
{
    return night;
}

int exam::getId()
{
    return id;
}
