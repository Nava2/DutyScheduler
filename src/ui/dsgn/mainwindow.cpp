#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "teamwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), tw(new TeamWidget(this))
{
    ui->setupUi(this);

    QGridLayout *lay = new QGridLayout(ui->tabTeam);
    lay->addWidget(tw, 0, 0, 1, 1);
    ui->wTabTeam->setLayout(lay);
}

MainWindow::~MainWindow()
{
    delete ui;
}
