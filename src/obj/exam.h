#ifndef EXAM_H
#define EXAM_H

#include <QString>
#include <QVariantMap>
#include <QDate>
#include <QSharedPointer>

class Exam : public QDate
{
public:
    enum Period {
        MORNING = 0, AFTERNOON = 1, NIGHT = 2
    };

    typedef QSharedPointer<Exam> Ptr;

    Exam();
    Exam(const int, const QDate &, const Period period);
    Exam(const Exam &);
    Exam(const QVariantMap &);

    void operator <<(const QVariantMap &);
    void operator >>(QVariantMap &);
    bool operator ==(const Exam &ex);
    bool operator !=(const Exam &ex);

    void setPeriod(const Period period);
    Period getPeriod() const;

    int getId();

    void setMidterm(const bool mterm);

    void addStaff(const QString &id);
    void removeStaff(const QString &id);

    QList<QString > getStaff();

private:
    int id;
    Period _period;

    QList<QString > staffIds;
};

#endif // EXAM_H
