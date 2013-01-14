#include <QApplication>

#include "ctrl/maincontroller.h"

int main(int argc, char *argv[])
{
    ctrl::MainController ctrl(argc, argv);
    return ctrl.exec();
}
