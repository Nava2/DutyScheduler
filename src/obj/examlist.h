#ifndef EXAMLIST_H
#define EXAMLIST_H

#include <QObject>
#include <QVariantMap>


#include "obj/exam.h"

class ExamList : public QObject {
    Q_OBJECT
public:
    ExamList(QObject *parent = nullptr);

    ExamList(const QList<Exam::Ptr> &exams, QObject *parent = nullptr);

    ExamList(const ExamList &other);

    ExamList &operator =(const ExamList &other);

    Exam::Ptr getExam(const QDate &date, const Exam::Period p) const;

    /*!
     * \brief getExams Returns a list of pointers, the same number of periods
     *      available. If the pointer is valid, there is already an exam
     * \param date
     * \return
     */
    QList<Exam::Ptr> getExams(const QDate &date) const;

    /*!
     * \brief addExam Adds an exam into the container
     * \param newExam
     */
    void add(const Exam::Ptr &newExam);

    /*!
     * \brief remove Removes an exam from the list
     * \param exam
     */
    void remove(const Exam::Ptr &exam);

    /*!
     * \brief all Gets a list containing all stored exams.
     * \return
     */
    QList<Exam::Ptr> all() const;

    /*!
     * \brief operator += convenience method for add(const Exam::Ptr &)
     * \param newExam
     * \return
     */
    ExamList &operator +=(const Exam::Ptr &newExam);

    /*!
     * \brief clear Removes all exams from internals.
     */
    void clear();

    ExamList &operator <<(const QVariantMap &json);
    void operator >>(QVariantMap &json) const;

private:
    QMultiMap<QDate, Exam::Ptr> _map;

};


#endif // EXAMLIST_H
