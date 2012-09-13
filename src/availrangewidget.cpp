#include "availrangewidget.h"

#include <QGroupBox>
#include <QDateEdit>
#include <QPushButton>
#include <QDate>
#include <QGridLayout>

#include "availabilitywidget.h"

AvailRangeWidget::AvailRangeWidget(int index, QWidget *parent) :
    QGroupBox(parent), _range(false)
{
    parent = dynamic_cast<AvailabilityWidget *>(parent);

    //left:
    dateEditLeft = new QDateEdit;
    dateEditLeft->setCalendarPopup(true);
    dateEditLeft->setDate(QDate::currentDate());

    groupBoxLeft = new QGroupBox;
    groupBoxLeft->setCheckable(true);
    groupBoxLeft->setChecked(false);
    groupBoxLeft->setTitle(QString::number(++index));
    connect(groupBoxLeft, SIGNAL(toggled(bool)), this, SLOT(onLeftGroupToggle(bool)));
    connect(groupBoxLeft, SIGNAL(toggled(bool)), parent, SLOT(onGroupBoxChecked(bool)));

    QHBoxLayout *lay = new QHBoxLayout;
    lay->addWidget(dateEditLeft);
    groupBoxLeft->setLayout(lay);

    // RIGHT:
    dateEditRight = new QDateEdit;
    dateEditRight->setCalendarPopup(true);
    dateEditRight->setDate(QDate::currentDate());

    groupBoxRight = new QGroupBox;
    groupBoxRight->setCheckable(true);
    groupBoxRight->setChecked(false);
    groupBoxRight->setTitle(QString::number(++index));
    connect(groupBoxRight, SIGNAL(toggled(bool)), parent, SLOT(onGroupBoxChecked(bool)));

    lay = new QHBoxLayout;
    lay->addWidget(dateEditRight);
    groupBoxRight->setLayout(lay);

    // toggle range:
    rangeToggle = new QPushButton("-->");
    rangeToggle->setCheckable(true);
    rangeToggle->setEnabled(false);
    connect(rangeToggle, SIGNAL(clicked(bool)), this, SLOT(onRangeClick(bool)));

    gridLayout = new QGridLayout;
    gridLayout->addWidget(groupBoxLeft, 0, 0, 3, 1);
    gridLayout->addWidget(groupBoxRight, 0, 2, 3, 1);
    gridLayout->addWidget(rangeToggle, 1, 1, 1, 1);

    setLayout(gridLayout);
}

void AvailRangeWidget::onRangeClick(bool click) {
    _range = click;
    if (_range) {
        groupBoxRight->setChecked(groupBoxLeft->isChecked());

        if (dateEditLeft->date() > dateEditRight->date()) {
            dateEditRight->setDate(dateEditLeft->date().addDays(1));
        }

    }

    emit onChangeRangeDate(range(), dateEditLeft->date(), dateEditRight->date());
}

void AvailRangeWidget::onFirstChange(const QDate &date) {
    emit onChangeRangeDate(range(), date, dateEditRight->date());
}

void AvailRangeWidget::onLastChange(const QDate &date) {
    emit onChangeRangeDate(range(), dateEditLeft->date(), date);
}

void AvailRangeWidget::onLeftGroupToggle(bool check) {
    rangeToggle->setEnabled(check);

    if (!check) {
        rangeToggle->setChecked(false);
        onRangeClick(false);
    }
}

bool AvailRangeWidget::range() {
    return _range;
}

bool AvailRangeWidget::getDates(QDate &left, QDate &right) {
    left = dateEditLeft->date();
    right = dateEditRight->date();

    return range();
}

bool AvailRangeWidget::getDates(QList<QDate> &outDates) {
    QDate left = dateEditLeft->date();
    QDate right = dateEditRight->date();

    outDates.clear();
    outDates.append(left);

    if (range()) {
        // get all the dates inbetween:
        left = left.addDays(1);
        while (left != right) {
            outDates.append(left);
            left = left.addDays(1);
        }
    }

    outDates.append(right);

    return range();
}

AvailableDate AvailRangeWidget::getAvailDate() {
    AvailableDate d;
    d.setDate(dateEditLeft->date());
    d.setEndDate(dateEditRight->date());
    d.setRange(rangeToggle->isDown());

    return d;
}

void AvailRangeWidget::setAvailDate(const AvailableDate &d) {
    dateEditLeft->setDate(d.date());

    _range = d.isRange();
    onRangeClick(this->range());

    if (d.isRange()) {
        dateEditRight->setDate(d.endDate());
    }
}

void AvailRangeWidget::setDates(const QDate &left, const QDate &right, bool range) {
    dateEditLeft->setDate(left);
    dateEditRight->setDate(right);

    _range = range;
    onRangeClick(this->range());
}

void AvailRangeWidget::isChecked(bool &left, bool &right) {
    left = groupBoxLeft->isChecked();
    right = groupBoxRight->isChecked();
}

void AvailRangeWidget::setChecked(const bool &left, const bool &right) {
    groupBoxLeft->setChecked(left);
    groupBoxRight->setChecked(right);
}
