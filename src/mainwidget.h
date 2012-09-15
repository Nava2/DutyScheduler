#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QtGui>

#include "staff.h"
#include "availabilitywidget.h"
#include "stafflist.h"
#include "examwidget.h"

QT_BEGIN_NAMESPACE
class QCalendarWidget;
class QCheckBox;
class QDate;
class QDateEdit;
class QFile;
class QGroupBox;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QMessageBox;
class QPushButton;
class QRadioButton;
class QStatusBar;
class QStringList;
class QSpinBox;
class QVBodLayout;
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    virtual
    ~MainWidget();
    void reset();
    StaffList::Ptr getStaff();
    QList<Exam::Ptr> * getExams();
    QList<QString > getUIDs();
    QString getTeam();
    void load(StaffList::Ptr staffList, QList<Exam::Ptr> *examList);


private slots:

    void updateStaffMember();
    void addStaffMember();
    void removeStaffMember();
    void clearSelections();
    void updateSelections(QListWidgetItem*);
    void addExam(const Exam::Ptr e);
    void removeExam(const Exam::Ptr e);



private:
    //DATA
    QList<Exam::Ptr> *finalExams, *midtermExams;
    StaffList::Ptr theTeam;

    //GUI
    void createStaffElements();

    void createPositionGroupBox();
    void createGenderGroupBox();
    void createNightClassGroupBox();
    void createExamScheduleGroupBox();
    void createStaffControls();
    void createNameGroupBox();

    QGroupBox *staffGroupBox;
    QListWidget *staffTeamList;

    QGroupBox *controlsGroupBox;
    QPushButton *addStaffButton;
    QPushButton *updateStaffButton;
    QPushButton *removeStaffButton;
    QPushButton *clearButton;

    QGroupBox *nameGroupBox;
    QLabel *firstNameLabel;
    QLabel *lastNameLabel;
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;

    QGroupBox *positionGroupBox;
    QRadioButton *donRadio;
    QRadioButton *raRadio;

    QGroupBox *genderGroupBox;
    QRadioButton *maleRadio;
    QRadioButton *femaleRadio;

    QGroupBox *nightClassGroupBox;
    QCheckBox *nightClassCheckBoxes[7];

    AvailabilityWidget *availWidget;

    ExamWidget *examWidget;

};

#endif // MAINWIDGET_H
