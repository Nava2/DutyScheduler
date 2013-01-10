#include "examlist.h"

ExamList::ExamList(const QList<Exam::Ptr> &exams, QObject *parent)
    : QObject(parent) {

    // insert the values into the map
    foreach (Exam::Ptr ptr, exams) {
        _map.insert(*ptr, ptr);
    }
}

ExamList::ExamList(const ExamList &other)
    : QObject(other.parent()) {
    if (&other != this) {
        _map = other._map;
    }
}

ExamList &ExamList::operator =(const ExamList &other) {
    if (&other != this) {
        setParent(other.parent());

        _map = other._map;
    }

    return *this;
}

Exam::Ptr ExamList::getExam(const QDate &date, const Exam::Period p) {
    QList<Exam::Ptr> tlist = _map.values(date);
    foreach (Exam::Ptr ptr, tlist) {
        if (ptr->getPeriod() == p) {
            return ptr;
        }
    }

    return Exam::Ptr();
}

QList<Exam::Ptr> ExamList::getExams(const QDate &date) {
    QList<Exam::Ptr> tList = _map.values(date);

    QList<Exam::Ptr> out;
    for (int i = 0; i < 3; ++i) {
        bool found = false;
        Exam::Period p = static_cast<Exam::Period>(i);

        foreach (Exam::Ptr ptr, tList) {
            if (ptr->getPeriod() == p) {
                out += ptr;
                tList.removeOne(ptr);

                found = true;
                break;
            }
        }

        if (!found)
            out += Exam::Ptr();
    }

    return out;
}

void ExamList::addExam(const Exam::Ptr &newExam) {
    QList<Exam::Ptr> old = _map.values(*newExam);

    foreach (Exam::Ptr p, old) {
        if (p->getPeriod() == newExam->getPeriod()) {
            _map.remove(*newExam, p);

            break;
        }
    }

    _map.insert(*newExam, newExam);
}

// IO:
////////////////

ExamList &ExamList::operator <<(const QVariantMap &json) {
    foreach (QString key, json.keys()) {
        QDate d = QDate::fromString(key);

        QList<QVariant> exams = json[key].toList();

        foreach (QVariant v, exams) {
            QVariantMap map = v.toMap();
            Exam::Ptr p(new Exam(map));

            _map.insert(d, p);
        }
    }
}

ExamList &ExamList::operator >>(QVariantMap &json) {
    foreach (QDate key, _map.keys()) {
        QList<Exam::Ptr> exams = _map.values(key);

        QVariantList vlist;
        foreach (Exam::Ptr p, exams) {
            QVariant v;
            *p >> v;
            vlist += v;
        }

        json[key.toString()] = vlist;
    }
}
