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


class AvailabilityWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit AvailabilityWidget(QWidget *parent = 0);


    void fillAvailabilitySubgroupBox(const int &i, QGridLayout *parentLayout,
                                                   QDateEdit *dateEdit,
                                                   QGroupBox *groupBox);

    void adjustRowCount(int count);

    void setToAvail(const QList<QDate > &avail);
    QList<QDate > getAvail();

    void reset();
    
signals:
    
public slots:
    void addRow();
    void removeRow();

private:
    QGridLayout *topLayout;
    QGridLayout *internalLayout;
    QScrollArea *scrollArea;
    QGroupBox *scrollFrame;


    int rowNum;
    QList<QGroupBox *> arrayGroupBox;
    QList<QDateEdit *> arrayDateEdit;
    QPushButton *addRowButton;
    QPushButton *rmRowButton;
    
};

#endif // AVAILABILITYWIDGET_H
