#include "teamwidget.h"
#include "ui_teamwidget.h"

#include "ui/multiselectcalendarwidget.h"

#include <QRegExpValidator>
#include <QSignalMapper>

#include <QDebug>


TeamWidget::TeamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeamWidget), _calTimeOff(nullptr), _countTimeOff(0)
{
    ui->setupUi(this);

    mapShortDayToDOW["sun"] = Qt::Sunday;
    mapShortDayToDOW["mon"] = Qt::Monday;
    mapShortDayToDOW["tues"] = Qt::Tuesday;
    mapShortDayToDOW["wed"] = Qt::Wednesday;
    mapShortDayToDOW["thurs"] = Qt::Thursday;
    mapShortDayToDOW["fri"] = Qt::Friday;
    mapShortDayToDOW["sat"] = Qt::Saturday;


    initPersonal();
    initTimeOff();
}

TeamWidget::~TeamWidget()
{
    delete ui;
}

void TeamWidget::initPersonal() {
    // only allow alphanumeric characters in the first and last name boxes
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("[a-zA-Z\\s]{1,}"), this);
    ui->leFirstName->setValidator(validator);
    ui->leLastName->setValidator(validator);
}

void TeamWidget::initTimeOff() {
    _calTimeOff = new MultiSelectCalendarWidget(ui->cwTimeOff, this);

    QTableView *view = ui->cwTimeOff->findChild<QTableView *>();
    MultiSelectCalendarFilter *filter = new MultiSelectCalendarFilter(ui->cwTimeOff, view);
    view->viewport()->installEventFilter(filter);

    _calTimeOff->selectedColour(Qt::green);

    connect(filter, SIGNAL(selectCoord(int,int)), _calTimeOff, SLOT(onSelectCoord(int,int)));

    connect(_calTimeOff, SIGNAL(ToggleSelected(QDate,bool)), this, SLOT(updateTimeOffCount(QDate,bool)));

    // weekday reoccur:
    QList<QAbstractButton *> cbs = ui->btnGrpReoccur->buttons();

    QSignalMapper *cbMapper = new QSignalMapper(this);
    foreach (QAbstractButton *cb, cbs) {
        connect(cb, SIGNAL(clicked()), cbMapper, SLOT(map()));
        qDebug() << cb->text().toLower() << "->" << mapShortDayToDOW[cb->text().toLower()];
        cbMapper->setMapping(cb, static_cast<int>(mapShortDayToDOW[cb->text().toLower()]));
    }
    connect(cbMapper, SIGNAL(mapped(int)), this, SLOT(toggleWeekday(int)));
}

void TeamWidget::initExams() {
//    ExamListModel *midtermModel = new ExamListModel(exams, this);

//    ctrlExamCalMid = new ctrl::ExamCalendar(AAAA, true, ui->cwMidterms, BBBB, this);
}

/////////// ACCESSORS:
/////////////////////////////////////

QCalendarWidget *TeamWidget::calendar(const bool isMidterm) const {
    return (isMidterm ? ui->cwMidterms : ui->cwFinal);
}


QList<QCheckBox *> TeamWidget::checkboxes(const bool isMidterm) const {
    QList<QCheckBox *> out;
    if (isMidterm) {
        out += ui->cbMidMorn;
        out += ui->cbMidAft;
        out += ui->cbMidEve;
    } else {
        out += ui->cbFinMorn;
        out += ui->cbFinAft;
        out += ui->cbFinEve;
    }

    return out;
}

/////////// SLOTS
/////////////////////////////////////

void TeamWidget::updateTimeOffCount(const QDate &date, bool selected) {
    date.year();

    if (selected) {
        ++_countTimeOff;
    } else {
        --_countTimeOff;
    }

    ui->lblTOCount->setText(QString::number(_countTimeOff));
}

void TeamWidget::toggleWeekday(const int dayOfWeek) {
    Qt::DayOfWeek dw = static_cast<Qt::DayOfWeek>(dayOfWeek);

    QSignalMapper *mapper = dynamic_cast<QSignalMapper *>(sender());
    QAbstractButton *cb = dynamic_cast<QAbstractButton *>(mapper->mapping(dayOfWeek));

    _calTimeOff->setDayOfWeekActive(dw, cb->isChecked());
}
