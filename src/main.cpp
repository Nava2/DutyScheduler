#include <QApplication>

#include "ctrl/maincontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Residence Life Scheduler");
    QCoreApplication::setApplicationVersion("0.0.1");

    ctrl::MainController ctrl(argc, argv);
    return ctrl.exec();
}
