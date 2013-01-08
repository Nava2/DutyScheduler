#include "teamwidget.h"
#include "ui_teamwidget.h"

#include <QRegExpValidator>

TeamWidget::TeamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeamWidget)
{
    ui->setupUi(this);


    initPersonal();

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
