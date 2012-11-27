#ifndef SCHEDULEWIDGET_H
#define SCHEDULEWIDGET_H

#include <QWidget>
#include <QFont>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QSignalMapper>
#include "staff.h"
#include "exam.h"
#include "sdate.h"
#include "myqlistwidget.h"
#include "schedviewer.h"

#include "iohandler.h"
#include "schedulewizzard.h"


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


class ScheduleWidget : public QWidget
{
    Q_OBJECT

public:
    ScheduleWidget(QWidget *parent);
    ScheduleWidget(const QString &fileNameStaff, const ScheduleWizzard &swiz, QWidget *parent);
    void saveMidSchedule(const QString &);
    ScheduleWidget(const QString &fileNameSchedule, StaffList &team, QList<Exam::Ptr> &finals, QList<Exam::Ptr> &midterms, QWidget *parent);

    virtual
    ~ScheduleWidget();

private slots:
    void dateClicked(QDate);
    void addStaff(QListWidgetItem*);//return value is success or not
    void removeStaff(QListWidgetItem*);
    void showMenu(QPoint p);//user right clicked an item in a list.
    void showStaffSchedule(QTableWidgetItem*);
    void setAsAM();
    void setSpecialDuty();
    void updateStats();
    void copySlot();
    void pasteSlot();
    void exportSchedule();

    void showSchedule(const QString &id);

signals:
    void updateSaveState();


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
    int dateToIndex(const QDate &);

    void setAsAM(const QString &);
    void addStaff(const QString &);


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
    QLabel *currentDateLabel;
    QLabel *currentDateLabelFIXED;
    QLabel *donsNeededLabel;
    QLabel *rasNeededLabel;
    QLabel *donsNeededLabelFIXED;
    QLabel *rasNeededLabelFIXED;


    //tableWidget stuff
    QTableWidget * statsTable;
    QList<QTableWidgetItem*> * statsTableItems;
    //

    QTableWidget * averagesTable;
    QTableWidgetItem * raAverageItem;
    QTableWidgetItem * raAverageWeekendItem;
    QTableWidgetItem * donAverageItem;
    QTableWidgetItem * donAverageWeekendItem;
    QTableWidgetItem * amAverageItem;

    QGroupBox *OnDutyGroupBox;
    QGroupBox *OnDeckGroupBox;
    MyQListWidget *onDeckList;
    MyQListWidget *onDutyList;

    StaffList theTeam;
    QList<Exam::Ptr> theFinals, theMidterms;

    QDate startDate;
    QDate endDate;
    SchedViewer *schedViewWidget;

    QList<int > donsNeeded;
    QList<int > rasNeeded;

    IOHandler *iohandle;

    bool examSchedule;

    QList<QList<QString> > nightClasses; // each list contains the id's of the staff who can't work that night. Mon = 0
    QList<QListWidgetItem*> * onDeckItems; // this contains a CONSTANT source of list widget items, one for each staff.
    QList<QListWidgetItem*> * onDutyItems;
    QList<SDate> datesList;

    QColor defaultRABack, defaultDonBack;

    QAction *setAsAMAction;

    QSignalMapper *scheduleMapper;
    QList<QAction *> showScheduleActions;

    QList<QString > *copyList;
    QString copyAM;

};

#endif // SCHEDULEWIDGET_H
