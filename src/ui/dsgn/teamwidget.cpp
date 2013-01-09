#include "teamwidget.h"
#include "ui_teamwidget.h"

#include "ui/multiselectcalendarwidget.h"

#include <QRegExpValidator>


TeamWidget::TeamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeamWidget)
{
    ui->setupUi(this);


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
    MultiSelectCalendarFilter *filter = new MultiSelectCalendarFilter(ui->cwTimeOff, _calTimeOff);
    view->viewport()->installEventFilter(filter);

    _calTimeOff->selectedColour(Qt::red);
    QDate date(ui->cwTimeOff->yearShown(), ui->cwTimeOff->monthShown(), 1);
    _calTimeOff->unselectedColour(ui->cwTimeOff->dateTextFormat(date).background().color());

    connect(filter, SIGNAL(selectCoord(int,int)), _calTimeOff, SLOT(onSelectCoord(int,int)));
}
