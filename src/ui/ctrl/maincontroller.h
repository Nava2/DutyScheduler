#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>

#include "obj/stafflist.h"
#include "obj/exam.h"

namespace ctrl {

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    
signals:
    
public slots:

private:

    
};

}

#endif // MAINCONTROLLER_H
