#include <QApplication>
#include "mainwindow.h"



//#include "ui/dsgn/mainwindow.h"
//#include "ui/dsgn/teamwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Duty Schedule Tool");
    w.show();

    return a.exec();
}
