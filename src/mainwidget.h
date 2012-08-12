#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include "staff.h"
#include "stafftable.h"

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
class QList<char>;
class QMessageBox;
class QPushButton;
class QRadioButton;
class QStatusBar;
class QStringList;
class QSpinBox;
class QVBodLayout;
QT_END_NAMESPACE

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    mainWidget(QWidget *parent = 0);
    ~mainWidget();
    void reset();
    QList<staff*> * getStaff();
    QList<exam*> * getExams();
    QString getTeam();
    void load(QList<staff*> * staffList, QList<exam*> * examList);


private slots:

    void updateStaffMember();
    void addStaffMember();
    void removeStaffMember();
    void clearSelections();
    void updateSelections();
    void addExam();
    void removeExam();

private:



    //DATA
    QList<exam*> *theExams;
    QList<staff*> *theTeam;

    //GUI
    void createStaffElements();

    void createPositionGroupBox();
    void createGenderGroupBox();
    void createNightClassGroupBox();
    void createAvailabilityGroupBox();
    void createExamScheduleGroupBox();
    void createStaffControls();
    void createNameGroupBox();

    QGroupBox *staffGroupBox;
    StaffTable *staffTeamTable;

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


    QGroupBox *availabilityGroupBox;
    QGroupBox *arrayGroupBox[4];
    QDateEdit *arrayDateEdit[4];

    QGroupBox *examScheduleGroupBox;
    QListWidget *examsList;
    QDateEdit *examDateEdit;
    QCheckBox *examNightCheck;
    QPushButton *addExamButton;
    QPushButton *removeExamButton;

};

#endif // MAINWIDGET_H
