#include "schedulewizzard.h"



scheduleWizzard::scheduleWizzard(QWidget *parent) :
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


void scheduleWizzard::createDatesGroupBox()
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

void scheduleWizzard::createStaffNeededGroupBox()
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


    raMonSpinBox = new QSpinBox;
    raTueSpinBox = new QSpinBox;
    raWedSpinBox = new QSpinBox;
    raThuSpinBox = new QSpinBox;
    raFriSpinBox = new QSpinBox;
    raSatSpinBox = new QSpinBox;
    raSunSpinBox = new QSpinBox;

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


    layout->addWidget(donMonSpinBox,1,1);
    layout->addWidget(donTueSpinBox,1,2);
    layout->addWidget(donWedSpinBox,1,3);
    layout->addWidget(donThuSpinBox,1,4);
    layout->addWidget(donFriSpinBox,1,5);
    layout->addWidget(donSatSpinBox,1,6);
    layout->addWidget(donSunSpinBox,1,7);


    layout->addWidget(raMonSpinBox,2,1);
    layout->addWidget(raTueSpinBox,2,2);
    layout->addWidget(raWedSpinBox,2,3);
    layout->addWidget(raThuSpinBox,2,4);
    layout->addWidget(raFriSpinBox,2,5);
    layout->addWidget(raSatSpinBox,2,6);
    layout->addWidget(raSunSpinBox,2,7);

    staffNeededGroupBox->setLayout(layout);
}

QString scheduleWizzard::getValues()
{
    QString ret = "";

    if(startDateEdit->date().daysTo(endDateEdit->date()) < 1)
        return ret;

    ret += startDateEdit->date().toString("dd/MM/yyyy") + ",";
    ret += endDateEdit->date().toString("dd/MM/yyyy") + ",";

    ret += QString(examCheckBox->isChecked()?"Y":"N") + ",";


    ret += QString::number(donMonSpinBox->value()) + ",";
    ret += QString::number(donTueSpinBox->value()) + ",";
    ret += QString::number(donWedSpinBox->value()) + ",";
    ret += QString::number(donThuSpinBox->value()) + ",";
    ret += QString::number(donFriSpinBox->value()) + ",";
    ret += QString::number(donSatSpinBox->value()) + ",";
    ret += QString::number(donSunSpinBox->value()) + ",";


    ret += QString::number(raMonSpinBox->value()) + ",";
    ret += QString::number(raTueSpinBox->value()) + ",";
    ret += QString::number(raWedSpinBox->value()) + ",";
    ret += QString::number(raThuSpinBox->value()) + ",";
    ret += QString::number(raFriSpinBox->value()) + ",";
    ret += QString::number(raSatSpinBox->value()) + ",";
    ret += QString::number(raSunSpinBox->value());

    return ret;
}
