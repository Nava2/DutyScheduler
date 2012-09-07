#include "exam.h"

#include <QDate>

Exam::Exam()
    : QDate()
{
    id = 99999;
    night = false;
}

Exam::Exam(int i, QString d, bool n)
{
    this->id = i;
    QDate date = fromString(d, "dd/MM/yyyy");
    setYMD(date.year(), date.month(), date.day());
    night = n;
}

Exam::Exam(const QVariantMap &json)
    : id(0), night(0) {
    *this << json;
}

Exam::Exam(const Exam &old) {
    this->id = old.id;
    setYMD(old.year(), old.month(), old.day());
    night = old.night;
}

void Exam::setNight(bool n)
{
    night = n;
}

bool Exam::getNight()
{
    return night;
}

int Exam::getId()
{
    return id;
}

// JSON IO
// in
void Exam::operator <<(const QVariantMap &json) {
    id = json["id"].toInt();
    QDate date = json["date"].toDate();
    setYMD(date.year(), date.month(), date.day());
    night = json["night"].toBool();
}

// out
void Exam::operator >>(QVariantMap &json) {
    json["id"] = id;
    json["date"] = (QDate)(*this);
    json["night"] = night;
}
