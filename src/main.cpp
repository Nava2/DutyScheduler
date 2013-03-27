#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Residence Life Scheduler");
    QCoreApplication::setApplicationVersion("0.0.1");

    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Duty Schedule Tool");
    w.show();
    
    return a.exec();
}
