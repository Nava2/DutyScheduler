#include "maincontroller.h"

#include <QApplication>

using namespace ctrl;

MainController::MainController(int argc, char *argv[], QObject *parent) :
    QObject(parent), _app(argc, argv), _teamPath("")
{
    _mw = new MainWindow;
    _mw->setWindowTitle("Duty Scheduling Tool");

    _tw = _mw->teamWidget();

    _midterms = new ExamList(this);
    _finals = new ExamList(this);

    _staff = new StaffList(this);

    _io = new IOHandler(this);

    _ectrlFinal = new ctrl::ExamCalendar(_finals, false, _tw->calendar(false),
                                         _tw->checkboxes(false), this);
    _ectrlMidterm = new ctrl::ExamCalendar(_midterms, true, _tw->calendar(true),
                                           _tw->checkboxes(true), this);

}

MainController::~MainController() {
    delete _mw;
}

int MainController::exec() {
    _mw->show();
    return _app.exec();
}

ExamList *MainController::midterms() const {
    return _midterms;
}

ExamList *MainController::finals() const {
    return _finals;
}

StaffList *MainController::staff() const {
    return _staff;
}

void MainController::setTeamPath(const QString &path) {

}

QString MainController::teamPath() const {
    return _teamPath;
}
