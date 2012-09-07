#ifndef MYSCHEDVIEWER_H
#define MYSCHEDVIEWER_H

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QDialogButtonBox>
#include <QList>
#include <QLabel>
#include <QDate>
#include <QGroupBox>

#include "sdate.h"
#include "staff.h"

class SchedViewer : public QWidget
{
    Q_OBJECT
public:
    explicit SchedViewer(const QDate &startDate, const QDate &lastDate, QWidget *parent = NULL);
    ~SchedViewer();

    void setToStaff(Staff::Ptr pStaff, QList<SDate> &datesList);
    Staff::Ptr getStaff();

private:
    QCalendarWidget *calendarStaff;
    QGroupBox *box, *labelBox;
    QLabel *onLabel;
    QLabel *amLabel;
    QLabel *noWorkLabel;
    QLabel *nameLabel;
    QVBoxLayout *externLayout;
    QGridLayout *internLayout, *labelLayout;

    Staff::Ptr staffMember;
};

#endif // SCHEDVIEWER_H

