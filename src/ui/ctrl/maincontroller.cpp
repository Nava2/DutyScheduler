#include "maincontroller.h"

using namespace ctrl;

MainController::MainController(QObject *parent) :
    QObject(parent), _teamPath("")
{
    _midterms = new ExamList(this);
    _finals = new ExamList(this);

    _staff = new StaffList(this);

    _io = new IOHandler(this);
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
