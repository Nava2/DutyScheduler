#ifndef STAFFTABLE_H
#define STAFFTABLE_H

#include <QTableWidget>
#include "staff.h"

class StaffTable : public QTableWidget
{
    Q_OBJECT
private:
    QStringList labels;
public:
    explicit StaffTable(QWidget *parent = 0);

    /* Appends a staff member into the table */
    void appendStaffMember(staff *);

    void removeMember();

    QList<QTableWidgetItem *> getRow(int row = -1);

    void updateRow(staff *mem, int row = -1);

    void clear();
    
signals:
    
public slots:
    
};

#endif // STAFFTABLE_H
