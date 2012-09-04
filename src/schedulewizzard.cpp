#include "schedulewizzard.h"



ScheduleWizzard::ScheduleWizzard(QWidget *parent) :
    QDialog(parent)
{

    createDatesGroupBox();
    createStaffNeededGroupBox();

    examCheckBox = new QCheckBox("Exam Period Schedule");

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                          | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


    QVBoxLayout *layout = new QVBoxLayout();
    QGridLayout *glayout = new QGridLayout();

    glayout->addWidget(datesGroupBox,0,0);
    glayout->addWidget(examCheckBox,1,0);
    glayout->addWidget(staffNeededGroupBox,0,1,2,2);

    layout->addLayout(glayout);
    layout->addWidget(buttonBox);

    setLayout(layout);
    setWindowTitle("Initial Schedule Settings");
}


void ScheduleWizzard::createDatesGroupBox()
{
    datesGroupBox = new QGroupBox("Dates of Schedule");

    startDateLabel = new QLabel("Start:");
    endDateLabel = new QLabel("End:");
    startDateEdit = new QDateEdit;
    endDateEdit = new QDateEdit;

    startDateEdit->setDate(QDate::currentDate());
    startDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(startDateEdit->date().addMonths(1));
    endDateEdit->setCalendarPopup(true);

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(startDateLabel,0,0);
    layout->addWidget(endDateLabel,1,0);
    layout->addWidget(startDateEdit,0,1);
    layout->addWidget(endDateEdit,1,1);

    datesGroupBox->setLayout(layout);

}

void ScheduleWizzard::createStaffNeededGroupBox()
{
    staffNeededGroupBox = new QGroupBox("Number of Staff Needed");


    monLabel = new QLabel("Mon");
    tueLabel = new QLabel("Tue");
    wedLabel = new QLabel("Wed");
    thuLabel = new QLabel("Thu");
    friLabel = new QLabel("FRI");
    satLabel = new QLabel("SAT");
    sunLabel = new QLabel("Sun");
    donLabel = new QLabel("Don-ON:");
    raAverageLabelFIXED = new QLabel("RA:");

    donMonSpinBox = new QSpinBox;
    donTueSpinBox = new QSpinBox;
    donWedSpinBox = new QSpinBox;
    donThuSpinBox = new QSpinBox;
    donFriSpinBox = new QSpinBox;
    donSatSpinBox = new QSpinBox;
    donSunSpinBox = new QSpinBox;

    donSpinBoxs.append(donMonSpinBox);
    donSpinBoxs.append(donTueSpinBox);
    donSpinBoxs.append(donWedSpinBox);
    donSpinBoxs.append(donThuSpinBox);
    donSpinBoxs.append(donFriSpinBox);
    donSpinBoxs.append(donSatSpinBox);
    donSpinBoxs.append(donSunSpinBox);

    raMonSpinBox = new QSpinBox;
    raTueSpinBox = new QSpinBox;
    raWedSpinBox = new QSpinBox;
    raThuSpinBox = new QSpinBox;
    raFriSpinBox = new QSpinBox;
    raSatSpinBox = new QSpinBox;
    raSunSpinBox = new QSpinBox;

    raSpinBoxs.append(raMonSpinBox);
    raSpinBoxs.append(raTueSpinBox);
    raSpinBoxs.append(raWedSpinBox);
    raSpinBoxs.append(raThuSpinBox);
    raSpinBoxs.append(raFriSpinBox);
    raSpinBoxs.append(raSatSpinBox);
    raSpinBoxs.append(raSunSpinBox);

    donMonSpinBox->setValue(0);
    donTueSpinBox->setValue(0);
    donWedSpinBox->setValue(0);
    donThuSpinBox->setValue(0);
    donFriSpinBox->setValue(1);
    donSatSpinBox->setValue(1);
    donSunSpinBox->setValue(0);
    raMonSpinBox->setValue(3);
    raTueSpinBox->setValue(3);
    raWedSpinBox->setValue(3);
    raThuSpinBox->setValue(4);
    raFriSpinBox->setValue(4);
    raSatSpinBox->setValue(4);
    raSunSpinBox->setValue(3);

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(monLabel,0,1);
    layout->addWidget(tueLabel,0,2);
    layout->addWidget(wedLabel,0,3);
    layout->addWidget(thuLabel,0,4);
    layout->addWidget(friLabel,0,5);
    layout->addWidget(satLabel,0,6);
    layout->addWidget(sunLabel,0,7);

    layout->addWidget(donLabel,1,0);
    layout->addWidget(raAverageLabelFIXED,2,0);


    int i = 1;
    foreach (QSpinBox *box, donSpinBoxs) {
        layout->addWidget(box, 1, i++);
    }

    i = 1;
    foreach (QSpinBox *box, raSpinBoxs) {
        layout->addWidget(box, 2, i++);
    }

    staffNeededGroupBox->setLayout(layout);
}

QString ScheduleWizzard::getValues()
{
    QString ret = "";

    if(startDateEdit->date().daysTo(endDateEdit->date()) < 1)
        return ret;

    ret += startDateEdit->date().toString("dd/MM/yyyy") + ",";
    ret += endDateEdit->date().toString("dd/MM/yyyy") + ",";

    ret += QString(examCheckBox->isChecked()?"Y":"N") + ",";

    foreach (QSpinBox *box, donSpinBoxs) {
        ret += QString::number(box->value()) + ",";
    }

    foreach (QSpinBox *box, raSpinBoxs) {
        ret += QString::number(box->value());
        if (box != raSunSpinBox)
            ret += ",";
    }

    return ret;
}

void ScheduleWizzard::getDates(QDate &start, QDate &end) const {
    start = startDateEdit->date();
    end = endDateEdit->date();
}

void ScheduleWizzard::getWeeklyValues(QList<int > &donOut, QList<int > &raOut) const {

    donOut.clear();
    raOut.clear();

    foreach (QSpinBox *box, donSpinBoxs) {
        donOut.append(box->value());
    }

    foreach (QSpinBox *box, raSpinBoxs) {
        raOut.append(box->value());
    }
}
