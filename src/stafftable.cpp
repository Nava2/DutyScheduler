#include "stafftable.h"
#include <stdio.h>

StaffTable::StaffTable(QWidget *parent) :
    QTableWidget(parent)
{
    setSortingEnabled(true); // sort automagically
    setStatusTip("The list of staff members of this team.");
    connect(this, SIGNAL(itemSelectionChanged()), parent, SLOT(updateSelections()));
    setColumnCount(2);
    setRowCount(0);

    labels << "Name" << "Role";
    setHorizontalHeaderLabels(labels); // must be called after setting num of cols
}

void StaffTable::appendStaffMember(staff *member) {

    QTableWidgetItem *item_name = new QTableWidgetItem, // this item represents the staff in the visible list widget
            *item_role = new QTableWidgetItem;

    int rows = rowCount();
    setRowCount(rows+1);

    item_name->setText(tr("%1 %2").arg(member->getFirstName(), member->getLastName())); // set the text of the list item
    item_name->setData(Qt::UserRole, member->getId());

    item_role->setText(tr(member->getPosition() ? "Don" : "RA"));//attach the staff's id number to the list item for later use.
    item_role->setData(Qt::UserRole, member->getId());

    setItem(rows, 0, item_name);
    setItem(rows, 1, item_role);
}

void StaffTable::removeMember() {
    QList<QTableWidgetItem *> items = selectedItems();

    if (items.size() == 0) {
        printf("No selected items in staff table.\n");
    }

    removeRow(items[0]->row());

    // free the memory associated
    for (QList<QTableWidgetItem *>::iterator it = items.begin(); it != items.end(); ) {
        delete *it;
        it = items.erase(it);
    }
}

QList<QTableWidgetItem *> StaffTable::getRow(int row) {
    QList<QTableWidgetItem *> out;

    if (row < 0)
        row = selectedItems()[0]->row();

    out << item(row, 0) << item(row, 1);
    return out;
}

void StaffTable::updateRow(staff *mem, int row) {
    QList<QTableWidgetItem *> items = getRow(row);

    items[0]->setText(tr("%1 %2").arg(mem->getFirstName(), mem->getLastName())); // set the text of the list item

    items[1]->setText(tr(mem->getPosition() ? "Don" : "RA"));//attach the staff's id number to the list item for later use.
}

void StaffTable::clear() {
    QTableWidget::clear(); // super's version

    setRowCount(0);
    setHorizontalHeaderLabels(labels);
}
