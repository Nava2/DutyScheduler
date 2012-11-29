#include "availabilitywidget.h"

#include <QDebug>
#include <QtGlobal>

#include <QGroupBox>
#include <QDate>
#include <QDateEdit>
#include <QList>
#include <QString>
#include <QVBoxLayout>

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

    internalLayout = new QVBoxLayout(scrollFrame);
    internalLayout->setSpacing(10);

    scrollArea->setWidget(scrollFrame);

    rowNum = 2;
    for (int x = 0; x < 4; x += 2 )
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

AvailabilityWidget::~AvailabilityWidget() {
    foreach(AvailRangeWidget *r, arrayRows) {
        delete r;
    }
    arrayRows.clear();

    delete internalLayout;
    delete scrollFrame;
    delete scrollArea;

    delete addRowButton;
    delete rmRowButton;
    delete countLabel;


    delete topLayout;
}

void AvailabilityWidget::setToAvail(const QList<AvailableDate> &avail) {
    // TODO Not loading properly
    QList<AvailableDate > range, single;
    sortRanges(avail, range, single);

    int num_slots = range.size() * 2 + single.size(); // round up to even number
//    num_slots /= 2;
    adjustRowCount(num_slots);

    int rowInd = 0;
    foreach (AvailableDate d, range) {
        AvailRangeWidget *row = arrayRows[rowInd++];

        row->setAvailDate(d);
    }

    qDebug() << "Rows" << arrayRows.count();

    AvailRangeWidget *row = nullptr;
    AvailableDate date;
    for ( int i = 0; i < single.size(); i++ ) {
        if (i % 2 == 0) {
            row = arrayRows[rowInd++];
            date.setDate(single[i].date());
            date.setEndDate(QDate(0, 0, 0));
        } else {
            date.setEndDate(single[i].date());

            row->setAvailDate(date);

            date = AvailableDate();
        }
    }

    if (date != AvailableDate() && row) {
        row->setAvailDate(date);
    }

    updateCountLabel();
}

QList<AvailableDate > AvailabilityWidget::getAvail() {
    QList<AvailableDate > out;

    foreach (AvailRangeWidget *row, arrayRows)
    {
        AvailableDate d;
        if (!row->getAvailDate(d)) {
            break;
        }

        if (d.isRange()) {
            out += d;
        } else {
            AvailableDate a(d.date(), false);
            out += a;

            if (d.endDate() != QDate(0, 0, 0)) {
                AvailableDate b(d.endDate(), false);
                out += b;
            }
        }
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

void AvailabilityWidget::sortRanges(QList<AvailableDate> in, QList<AvailableDate> &ranges, QList<AvailableDate > &single) {
    ranges.clear();
    single.clear();

    QList<AvailableDate>::iterator it;
    for (it = in.begin(); it != in.end(); ) {
        AvailableDate d = *it;
        if (d.isRange()) {
            ranges.append(d);

            it = in.erase(it);
        } else {
            it++;
        }
    }

    foreach (AvailableDate d, in) {
        single.append(d);
    }

    qDebug() << "Total" << ranges.count() + single.count() << ":: Single"
             << single.count() << ":: Ranges" << ranges.count();
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
    AvailRangeWidget *rw = new AvailRangeWidget(rowNum*2, this);

    internalLayout->addWidget(rw);
    arrayRows += rw;

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
        dayCount -= dateList.size();
    }

    // delete after so the removeWidget call isn't messed up
    delete rw;

    rowNum--;

    rmRowButton->setEnabled(rowNum > 2);

    updateCountLabel();
}
