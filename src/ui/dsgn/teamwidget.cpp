#include "teamwidget.h"
#include "ui_teamwidget.h"

TeamWidget::TeamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeamWidget)
{
    ui->setupUi(this);
}

TeamWidget::~TeamWidget()
{
    delete ui;
}
