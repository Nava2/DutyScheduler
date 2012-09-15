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
    explicit ExamWidget(QList<Exam::Ptr> &midterms, QList<Exam::Ptr> &finals, QWidget *parent = 0);
    virtual
    ~ExamWidget();

    void reset();

    void setFinals(const QList<Exam::Ptr> &out);
    void getFinals(QList<Exam::Ptr> &out);
    
signals:
    void OnFinalAdded(const Exam::Ptr);
    void OnFinalRemoved(const Exam::Ptr);

public slots:
    void addFinal();
    void removeFinal();

private:
    QGroupBox *ScheduleGroupBox;
    QListWidget *examList;
    QDateEdit *dateEdit;
    QCheckBox *nightCheck;
    QPushButton *addButton;
    QPushButton *removeButton;

    QTabWidget *tabWidget;

    QList<Exam::Ptr > &finals, &midterms;
    
};

#endif // EXAMWIDGET_H
