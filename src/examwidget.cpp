#include "examwidget.h"

#include <QGroupBox>
#include <QListWidget>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>

#include "exam.h"
#include "mainwidget.h"

ExamWidget::ExamWidget(QList<Exam::Ptr> &exams, QWidget *parent) :
    QWidget(parent), finals(exams)
{
    MainWidget *mw = dynamic_cast<MainWidget *>(parent);

    tabWidget = new QTabWidget;

    ScheduleGroupBox = new QGroupBox;
    examList = new QListWidget;
    dateEdit = new QDateEdit;
    nightCheck = new QCheckBox(tr("Night Exam"));
    addButton = new QPushButton(tr("ADD EXAM"));
    removeButton = new QPushButton(tr("REMOVE EXAM"));
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);

    tabWidget->addTab(ScheduleGroupBox, "Midterms");
    tabWidget->addTab(ScheduleGroupBox, "Finals");

    examList->setStatusTip("The list of exams for the selected staff member.");


    connect(addButton, SIGNAL(clicked()), this, SLOT(addFinal()));
    connect(this, SIGNAL(OnFinalAdded(Exam::Ptr)), mw, SLOT(addFinal(Exam::Ptr)));

    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFinal()));
    connect(this, SIGNAL(OnFinalRemoved(Exam::Ptr)), mw, SLOT(removeFinal(Exam::Ptr)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(examList,0,0,4,1);
    layout->addWidget(dateEdit,0,1);
    layout->addWidget(nightCheck,1,1);
    layout->addWidget(addButton,2,1);
    layout->addWidget(removeButton,3,1);

    ScheduleGroupBox->setLayout(layout);

    QGridLayout *up_layout = new QGridLayout;
    up_layout->addWidget(tabWidget);
    setLayout(up_layout);
}

ExamWidget::~ExamWidget() {
    delete ScheduleGroupBox;
}

void ExamWidget::reset() {
    examList->clear();
    dateEdit->setDate(QDate::currentDate());
    nightCheck->setChecked(false);
}

void ExamWidget::setFinals(const QList<Exam::Ptr> &exams) {
    foreach (Exam::Ptr ex, exams)
    {
        QString text = ex->toString("dd/MM/yyyy");
        if(ex->getNight()) {
            text += " (night)";
        }

        QListWidgetItem *i = new QListWidgetItem(text);
        i->setData(Qt::UserRole, ex->getId());
        examList->insertItem(0,i);
    }
}

void ExamWidget::getFinals(QList<Exam::Ptr > &out) {
    out.clear();
    for (int i = 0; i < examList->count(); i++) {
        QListWidgetItem *item = examList->item(i);

        int id = item->data(Qt::UserRole).toInt();
        out.append(finals[id]);
    }
}

void ExamWidget::addFinal() {
    Exam::Ptr e(new Exam(finals.count(),
                             dateEdit->date(),
                             nightCheck->isChecked()));

    QListWidgetItem *item = new QListWidgetItem();

    if (nightCheck->isChecked())
        item->setText(e->toString("dd/MM/yyyy") + " (night)");
    else
        item->setText(e->toString("dd/MM/yyyy"));

    item->setData(Qt::UserRole,e->getId());

    examList->insertItem(0,item);//adds the list item to the small exams list

    emit OnFinalAdded(e);
}

void ExamWidget::removeFinal() {
    QListWidgetItem *i;
    i = examList->takeItem(examList->currentRow());
    Exam::Ptr e = finals[i->data(Qt::UserRole).toInt()];
    delete i;

    emit OnFinalRemoved(e);
}
