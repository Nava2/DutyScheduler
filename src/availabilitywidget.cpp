#include "availabilitywidget.h"

#include <QDebug>
#include <QtGlobal>

#include <QGroupBox>
#include <QDate>
#include <QDateEdit>
#include <QList>
#include <QString>

AvailabilityWidget::AvailabilityWidget(QWidget *parent) :
    QGroupBox(tr("Dates Unavailable"), parent), rowNum(0)
{
    topLayout = new QGridLayout;

    scrollArea = new QScrollArea;
    scrollArea->setStatusTip("These are the dates that the selected staff member is unavailable. ex. A wedding, academic commitment, etc.");
//    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);

    scrollFrame = new QGroupBox;
    scrollFrame->setFlat(true);

    internalLayout = new QGridLayout(scrollFrame);
    internalLayout->setSpacing(10);

    scrollArea->setWidget(scrollFrame);

    rowNum = 2;
    for (int x = 0; x < 4; x++)
    {
        QDateEdit *de = new QDateEdit;
        QGroupBox *gb = new QGroupBox;
        arrayDateEdit.append(de);
        arrayGroupBox.append(gb);

        fillAvailabilitySubgroupBox(x, internalLayout, de, gb);
        scrollFrame->adjustSize();
    }

    topLayout->addWidget(scrollArea, 0, 0, 1, -1);

    addRowButton = new QPushButton("+");
    addRowButton->setToolTip("Click to add a row to availability options.");
    rmRowButton = new QPushButton("-");
    rmRowButton->setToolTip("Click to remove a row from availability options.");

    connect(addRowButton, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(rmRowButton, SIGNAL(clicked()), this, SLOT(removeRow()));

    topLayout->addWidget(addRowButton, 1, 1, 1, 1);
    topLayout->addWidget(rmRowButton, 1, 2, 1, 1);

    setLayout(topLayout);

    rmRowButton->setEnabled(rowNum > 2);
}

void AvailabilityWidget::fillAvailabilitySubgroupBox(const int &i, QGridLayout *parentLayout,
                                               QDateEdit *dateEdit,
                                               QGroupBox *groupBox) {
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setMinimumHeight(30);

    groupBox->setCheckable(true);
    groupBox->setChecked(false);
    groupBox->setMinimumHeight(50);
    groupBox->setTitle(QString::number(i+1));

    QGridLayout *subLayout = new QGridLayout;

    subLayout->addWidget(dateEdit);
    groupBox->setLayout(subLayout);

//    parentLayout->addWidget(groupBox, ((i>>1) & 0x01), (i & 0x01)); // left is even, right is odd
    parentLayout->addWidget(groupBox, i / 2, i % 2);
}

void AvailabilityWidget::setToAvail(const QList<QDate> &avail) {
    int num_slots = (avail.size() % 2 == 0) ? avail.size() : avail.size() + 1; // round up to even number

    adjustRowCount(num_slots);

    for (int i = 0; i < qMax(4, num_slots); i++) {
        if (i > num_slots || i >= avail.size()) {
            // num_slots < 4 :(
            arrayGroupBox[i]->setChecked(false);
            arrayDateEdit[i]->setDate(QDate::currentDate());
        } else {
            // num_slots > 4 !
            arrayGroupBox[i]->setChecked(true);
            arrayDateEdit[i]->setDate(avail[i]);
        }
    }
}

QList<QDate > AvailabilityWidget::getAvail() {
    QList<QDate > out;
    for ( int i = 0; i < arrayGroupBox.size(); i++ )
    {
        if (arrayGroupBox[i]->isChecked())
            out += arrayDateEdit[i]->date();
    }

    return out;
}

void AvailabilityWidget::reset() {
    // TODO Make this resort back to 4?
    adjustRowCount(4);

    for(int y = 0; y < arrayGroupBox.size(); y++)
    {
        arrayDateEdit[y]->setDate(QDate::currentDate());
        arrayGroupBox[y]->setChecked(false);
    }
}

void AvailabilityWidget::adjustRowCount(int count) {
    int num_slots = (count % 2 == 0) ? count : count + 1 ; // round up to even number

    // adjust rows
    while ( arrayGroupBox.size() < num_slots ) {
        addRow();
    }

    while ( arrayGroupBox.size() > qMax(num_slots, 4) ) {
        removeRow();
    }
}

void AvailabilityWidget::addRow() {
    qDebug() << "ADD AVAIL ROW";

//    internalLayout->setSizeConstraint(QLayout::SetNoConstraint);

    for (int x = 0; x < 2; x++)
    {
        QDateEdit *de = new QDateEdit;
        QGroupBox *gb = new QGroupBox;
        arrayDateEdit.append(de);
        arrayGroupBox.append(gb);

        fillAvailabilitySubgroupBox(rowNum*2+x, internalLayout, de, gb);
        scrollFrame->adjustSize();
    }

    rowNum++;

    rmRowButton->setEnabled(rowNum > 2);
}

void AvailabilityWidget::removeRow() {
    qDebug() << "RM AVAIL ROW";

    if (arrayGroupBox.size() <= 4) {
        qDebug() << "Cannot have less than 2 rows.";
        return;
    }

    for (int i = 0; i < 2; i++) {
        QDateEdit *de = arrayDateEdit.last();
        arrayDateEdit.removeLast();


        QGroupBox *gb = arrayGroupBox.last();
        arrayGroupBox.removeLast();
        internalLayout->removeWidget(gb);

        // delete after so the removeWidget call isn't messed up
        delete de;
        delete gb;
    }

    rowNum--;

    rmRowButton->setEnabled(rowNum > 2);
}
