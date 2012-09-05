#ifndef MYSCHEDVIEWER_H
#define MYSCHEDVIEWER_H

#include <QDialog>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QTextCharFormat>
#include <QDialogButtonBox>
#include <QList>
#include <QLabel>
#include <QDate>
#include "sdate.h"

class mySchedViewer : public QWidget
{
    Q_OBJECT
public:
    explicit mySchedViewer(QString name, int staffID, QList<SDate> *datesList, QWidget *parent = NULL);
    ~mySchedViewer();

private:
    QCalendarWidget *calendarStaff;
    QDialogButtonBox *buttonBox;
    QLabel *onLabel;
    QLabel *amLabel;
    QVBoxLayout *layout;
};

#endif // MYSCHEDVIEWER_H

