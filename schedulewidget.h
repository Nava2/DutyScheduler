#ifndef SCHEDULEWIDGET_H
#define SCHEDULEWIDGET_H

#include <QWidget>
#include <QFont>
#include "staff.h"
#include "exam.h"
#include "sdate.h"
#include "myqlistwidget.h"
#include "myschedviewer.h"
#include "myqlabel.h"

QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QCheckBox;
class QDate;
class QDateEdit;
class QGroupBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QSpinBox;
class QRadioButton;
class QPushButton;
class QGridLayout;
class QHBoxLayout;
class QVBodLayout;
class QString;
class QFile;
QT_END_NAMESPACE


class scheduleWidget : public QWidget
{
    Q_OBJECT

public:
    scheduleWidget(QWidget *parent = 0);
    scheduleWidget(QString, QString, QWidget *parent = 0);
    ~scheduleWidget();

private slots:
    void dateClicked(QDate);
    void addStaff(QListWidgetItem*);//return value is success or not
    void removeStaff(QListWidgetItem*);
    void showMenu(QPoint p);//user right clicked an item in a list.
    void showStaffSchedule();
    void setAsAM();
    void setSpecialDuty();
    void updateStats(bool);
    void copySlot();
    void pasteSlot();
    void exportSchedule();

private:

    void loadStaffTeamData(QString);

    //schedule box
    void createScheduleGroupBox();
    void createScheduleControls();
    void createScheduleStats();
    void createCalendar();
    void createLists();
    void createAMGroupBox();
    void prepInterface();

    void updateNeeded();
    int dateToIndex(QDate);

    void setAsAM(int);
    void addStaff(int);

    QGroupBox *scheduleGroupBox;
    QGroupBox *datesGroupBox;
    QGroupBox *staffNeededGroupBox;
    QGroupBox *scheduleControls;
    QGroupBox *scheduleStatsGroupBox;
    QCalendarWidget *calendar;
    QGroupBox *AMGroupBox;

    QPushButton *exportButton;
    QPushButton *spDutyFlagButton;
    QPushButton *copyButton;
    QPushButton *pasteButton;


    QRadioButton *totalRadioButton;
    QRadioButton *weekendRadioButton;
    QList<MyQLabel*> *teamStatsLabels;
    //QList<int> * totalStaffShifts;//contains the number of shifts for each staff member.
    //QList<int> * weekendStaffShifts;
    QList<QLabel*> * shiftCountLabels;

    QLabel *donAverageLabel;
    QLabel *raAverageLabel;
    QLabel *dALabel;//fixed label
    QLabel *rALabel;//fixed label
    QLabel *dNLabel;//fixed label
    QLabel *rNLabel;//fixed label
    QLabel *donsNeededLabel;
    QLabel *rasNeededLabel;


    QGroupBox *OnDutyGroupBox;
    QGroupBox *OnDeckGroupBox;
    MyQListWidget *onDeckList;
    MyQListWidget *onDutyList;

    QFont *AMFont;
    QFont *DonFont;

    QList<staff*> *theTeam;
    QList<exam*> *theExams;

    QDate *startDate;
    QDate *endDate;

    int donsNeeded[7];
    int rasNeeded[7];

    bool examSchedule;

    QList<int> * nightClasses[7]; // each list contains the id's of the staff who can't work that night. Mon = 0
    QList<QListWidgetItem*> * onDeckItems; // this contains a CONSTANT source of list widget items, one for each staff.
    QList<QListWidgetItem*> * onDutyItems;
    QList<sDate*> * datesList;

    QAction *setAsAMAction;

    QList<int> * copyList;
    int copyAM;

};

#endif // SCHEDULEWIDGET_H
