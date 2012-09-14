#ifndef AVAILABILITYWIDGET_H
#define AVAILABILITYWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QGroupBox>
#include <QDate>
#include <QDateEdit>
#include <QList>
#include <QString>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include "availrangewidget.h"
#include "availabledate.h"

class AvailRangeWidget;

class AvailabilityWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit AvailabilityWidget(QWidget *parent = 0);


    void buildSubgroupBox(const int &i, QGridLayout *parentLayout,
                                                   QDateEdit *dateEdit,
                                                   QGroupBox *groupBox);

    void adjustRowCount(int count);

    void setToAvail(const QList<AvailableDate> &avail);
    QList<AvailableDate > getAvail();

    void reset();
    
signals:
    
public slots:
    void addRow();
    void removeRow();
    void onGroupBoxChecked(bool on);

private:
    void updateCountLabel();

    void sortRanges(const QList<AvailableDate> in, QList<AvailableDate> &range, QList<AvailableDate> &single);

    QGridLayout *topLayout;
    QVBoxLayout *internalLayout;
    QScrollArea *scrollArea;
    QGroupBox *scrollFrame;


    int rowNum;
    int dayCount;
    QList<AvailRangeWidget *> arrayRows;
    QPushButton *addRowButton;
    QPushButton *rmRowButton;
    QLabel *countLabel;
    
};

#endif // AVAILABILITYWIDGET_H
