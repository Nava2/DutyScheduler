#ifndef EXAMLIST_H
#define EXAMLIST_H

#include <QObject>
#include <QVariantMap>


#include "obj/exam.h"

class ExamList : public QObject {
    Q_OBJECT
public:
    ExamList(const QList<Exam::Ptr> &exams, QObject *parent = nullptr);

    ExamList(const ExamList &other);

    ExamList &operator =(const ExamList &other);

    Exam::Ptr getExam(const QDate &date, const Exam::Period p);

    /*!
     * \brief getExams Returns a list of pointers, the same number of periods
     *      available. If the pointer is valid, there is already an exam
     * \param date
     * \return
     */
    QList<Exam::Ptr> getExams(const QDate &date);

    /*!
     * \brief addExam Adds an exam into the container
     * \param newExam
     */
    void addExam(const Exam::Ptr &newExam);

    ExamList &operator <<(const QVariantMap &json);
    ExamList &operator >>(QVariantMap &json);

private:
    QMultiMap<QDate, Exam::Ptr> _map;

};


#endif // EXAMLIST_H
