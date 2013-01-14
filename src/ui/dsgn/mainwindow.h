#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "teamwidget.h"
#include "obj/staff.h"
#include "obj/stafflist.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    TeamWidget *teamWidget() const;

    void setToStaffMember(const Staff::Ptr &ptr);
    Staff::Ptr getStaffMember();
    
private:
    Ui::MainWindow *ui;
    TeamWidget *_tw;
};

#endif // MAINWINDOW_H
