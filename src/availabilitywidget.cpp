#include "availabilitywidget.h"

#include <QDebug>
#include <QtGlobal>

#include <QGroupBox>
#include <QDate>
#include <QDateEdit>
#include <QList>
#include <QString>

AvailabilityWidget::AvailabilityWidget(QWidget *parent) :
    QGroupBox(tr("Dates Unavailable"), parent), rowNum(0), dayCount(0)
{
    topLayout = new QGridLayout;

    scrollArea = new QScrollArea;
    scrollArea->setStatusTip("These are the dates that the selected staff member is unavailable. ex. A wedding, academic commitment, etc.");
//    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);

    scrollFrame = new QGroupBox;
    scrollFrame->setFlat(true);

    internalLayout = new QHBoxLayout(scrollFrame);
    internalLayout->setSpacing(10);

    scrollArea->setWidget(scrollFrame);

    rowNum = 2;
    for (int x = 0; x < 4; )
    {
        AvailRangeWidget *rw = new AvailRangeWidget(x, this);
        arrayRows.append(rw);

        internalLayout->addWidget(rw);
        scrollFrame->adjustSize();
    }

    topLayout->addWidget(scrollArea, 0, 0, 1, -1);

    countLabel = new QLabel("Count: 0");

    addRowButton = new QPushButton("+");
    addRowButton->setToolTip("Click to add a row to availability options.");
    rmRowButton = new QPushButton("-");
    rmRowButton->setToolTip("Click to remove a row from availability options.");

    connect(addRowButton, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(rmRowButton, SIGNAL(clicked()), this, SLOT(removeRow()));

    topLayout->addWidget(countLabel, 1, 0, 1, 1, Qt::AlignLeft);
    topLayout->addWidget(addRowButton, 1, 1, 1, 1);
    topLayout->addWidget(rmRowButton, 1, 2, 1, 1);

    setLayout(topLayout);

    rmRowButton->setEnabled(rowNum > 2);
}

void AvailabilityWidget::setToAvail(const QList<QDate> &avail) {
    int num_slots = (avail.size() % 2 == 0) ? avail.size() : avail.size() + 1; // round up to even number

    adjustRowCount(num_slots);

    for (int i = 0; i < qMax(4, num_slots); i+=2) {
        AvailRangeWidget *row = arrayRows[i];
        bool checked[] = { false, false };
        QDate dates[2];
        row->isChecked(checked[0], checked[1]);

        if (i > num_slots || i >= avail.size()) {
            // num_slots < 4 :(
            for (int j = 0; j < 2; j++) {
                dates[j] = QDate::currentDate();
            }
        } else {
            // num_slots > 4 !
            for (int j = 0; j < 2 && i < qMax(4, num_slots); j++,i++) {
                checked[j] = true;
                dates[j] = avail[i];
            }
        }

        row->setChecked(checked[0], checked[1]);
        row->setDates(dates[0], dates[1]);
    }

    updateCountLabel();
}

QList<QDate > AvailabilityWidget::getAvail() {
    QList<QDate > out;

    foreach (AvailRangeWidget *row, arrayRows)
    {
        bool checked[] = { false, false };
        QDate dates[2];
        row->isChecked(checked[0], checked[1]);
        row->getDates(dates[0], dates[1]);

        for (int j = 0; j < 2; j++)
            if (checked[j])
                out += dates[j];
    }

    return out;
}

void AvailabilityWidget::reset() {
    // TODO Make this resort back to 4?
    adjustRowCount(4);

    foreach (AvailRangeWidget *row, arrayRows)
    {
        row->setDates(QDate::currentDate(), QDate::currentDate());
        row->setChecked(false, false);
    }

    updateCountLabel();
}

void AvailabilityWidget::adjustRowCount(int count) {
    int num_slots = (count % 2 == 0) ? count : count + 1 ; // round up to even number
    num_slots /= 2;

    // adjust rows
    while ( arrayRows.size() < num_slots ) {
        addRow();
    }

    while ( arrayRows.size() > qMax(num_slots, 2) ) {
        removeRow();
    }

    updateCountLabel();
}

void AvailabilityWidget::updateCountLabel() {
    countLabel->setText("Count: " + QString::number(dayCount));
}

void AvailabilityWidget::onGroupBoxChecked(bool on) {
    QGroupBox *box = dynamic_cast<QGroupBox *>(sender());
    if (on && !box->isChecked()) {
        dayCount++;
    } else if (!on && box->isChecked()){
        dayCount--;
    }

    updateCountLabel();
}

void AvailabilityWidget::addRow() {
    AvailRangeWidget *rw = new AvailRangeWidget(rowNum*2);

    internalLayout->addWidget(rw);
    scrollFrame->adjustSize();

    rowNum++;

    rmRowButton->setEnabled(rowNum > 2);
}

void AvailabilityWidget::removeRow() {
    if (arrayRows.size() <= 2) {
        qDebug() << "Cannot have less than 2 rows.";
        return;
    }

    AvailRangeWidget *rw = arrayRows.last();
    arrayRows.removeLast();
    internalLayout->removeWidget(rw);

    if (!rw->range()) {
        // single days
        bool checked[2];
        rw->isChecked(checked[0], checked[1]);
        for (int x = 0; x<2; x++)
            if (checked[x])
                dayCount--;
    } else {
        // range:
        QList<QDate > dateList;
        rw->getDates(dateList);
        foreach(QDate x, dateList) {
            dayCount--;
        }
    }

    // delete after so the removeWidget call isn't messed up
    delete rw;

    rowNum--;

    rmRowButton->setEnabled(rowNum > 2);

    updateCountLabel();
}
