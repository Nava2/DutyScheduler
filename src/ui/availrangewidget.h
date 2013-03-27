#ifndef AVAILRANGEWIDGET_H
#define AVAILRANGEWIDGET_H

#include <QGroupBox>
#include <QDateEdit>
#include <QPushButton>
#include <QDate>
#include <QGridLayout>

#include "availabilitywidget.h"
#include "obj/availabledate.h"

class AvailRangeWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit AvailRangeWidget(int index, QWidget *parent = 0);
    virtual
    ~AvailRangeWidget();
    
    bool range();

    void setDates(const QDate &left, const QDate &right, bool range = false);

    bool getDates(QDate &left, QDate &right);

    bool getDates(QList<QDate > &);

    bool getAvailDate(AvailableDate &d);
    void setAvailDate(const AvailableDate &);

    void isChecked(bool &left, bool &right);
    void setChecked(const bool &left, const bool &right);

private slots:
    void onRangeClick(bool);
    void onFirstChange(const QDate &date);
    void onLastChange(const QDate &date);
    void onLeftGroupToggle(bool);

signals:
    void onChangeRangeDate(const bool isRange, const QDate &newFirst, const QDate &newLast);

private:
    QGridLayout *gridLayout;

    QDateEdit *dateEditLeft;
    QGroupBox *groupBoxLeft;

    QDateEdit *dateEditRight;
    QGroupBox *groupBoxRight;

    QPushButton *rangeToggle;

    bool _range;
};

#endif // AVAILRANGEWIDGET_H
