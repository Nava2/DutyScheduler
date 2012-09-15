#ifndef SCHEDULEWIZZARD_H
#define SCHEDULEWIZZARD_H

#include <QDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QDateEdit>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QDate>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
class QDate;
class QDateEdit;
class QGroupBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QGridLayout;
class QDialogButtonBox;
class QCheckBox;
QT_END_NAMESPACE

class ScheduleWizzard : public QDialog
{
    Q_OBJECT
public:
    explicit ScheduleWizzard(QWidget *parent = 0);
    virtual
    ~ScheduleWizzard();

    QString getValues();

    bool isExamSchedule() const {
        if (examCheckBox)
            return examCheckBox->isChecked();
        return false;
    }

    void getWeeklyValues(QList<int > &donOut, QList<int > &raOut) const ;
    void getDates(QDate &start, QDate &end) const;

signals:
    
public slots:


private:
    void createDatesGroupBox();
    void createStaffNeededGroupBox();

    QGroupBox *datesGroupBox;
    QGroupBox *staffNeededGroupBox;

    QLabel *startDateLabel;
    QDateEdit *startDateEdit;
    QLabel *endDateLabel;
    QDateEdit *endDateEdit;

    QLabel *sunLabel;
    QLabel *monLabel;
    QLabel *tueLabel;
    QLabel *wedLabel;
    QLabel *thuLabel;
    QLabel *friLabel;
    QLabel *satLabel;
    QLabel *donLabel;
    QLabel *raAverageLabelFIXED;
    QSpinBox *donSunSpinBox;
    QSpinBox *donMonSpinBox;
    QSpinBox *donTueSpinBox;
    QSpinBox *donWedSpinBox;
    QSpinBox *donThuSpinBox;
    QSpinBox *donFriSpinBox;
    QSpinBox *donSatSpinBox;

    QList<QSpinBox *> donSpinBoxs;

    QSpinBox *raSunSpinBox;
    QSpinBox *raMonSpinBox;
    QSpinBox *raTueSpinBox;
    QSpinBox *raWedSpinBox;
    QSpinBox *raThuSpinBox;
    QSpinBox *raFriSpinBox;
    QSpinBox *raSatSpinBox;

    QList<QSpinBox *> raSpinBoxs;

    QCheckBox *examCheckBox;

    QDialogButtonBox *buttonBox;
    
};

#endif // SCHEDULEWIZZARD_H
