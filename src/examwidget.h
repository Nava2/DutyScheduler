#ifndef EXAMWIDGET_H
#define EXAMWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QListWidget>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>

#include "exam.h"

class ExamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExamWidget(QList<Exam::Ptr> &finals, QList<Exam::Ptr> &midterms, QWidget *parent = 0);
    virtual
    ~ExamWidget();

    void reset();

    void getExams(QList<Exam::Ptr > &fOut, QList<Exam::Ptr> &mOut);
    void setExams(const QList<Exam::Ptr > &fIn, const QList<Exam::Ptr> &mIn);

    void setFinals(const QList<Exam::Ptr> &in);
    void getFinals(QList<Exam::Ptr> &out);

    void setMidterms(const QList<Exam::Ptr> &in);
    void getMidterms(QList<Exam::Ptr> &out);
    
signals:
    void OnFinalAdded(const Exam::Ptr);
    void OnFinalRemoved(const Exam::Ptr);

    void OnMidtermAdded(const Exam::Ptr);
    void OnMidtermRemoved(const Exam::Ptr);

public slots:
    void addFinal();
    void removeFinal();

    void addMidterm();
    void removeMidterm();

private:
    QGroupBox *finalScheduleGroupBox, *midtermScheduleGroupBox;
    QListWidget *finalList, *midtermList;
    QDateEdit *finalDateEdit, *midtermDateEdit;
    QCheckBox *finalNightCheck, *midtermNightCheck;
    QPushButton *addFinalButton, *addMidtermButton;
    QPushButton *removeFinalButton, *removeMidtermButton;

    QTabWidget *tabWidget;

    QList<Exam::Ptr > &midterms, &finals;
    
};

#endif // EXAMWIDGET_H
