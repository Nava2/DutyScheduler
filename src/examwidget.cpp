#include "examwidget.h"

#include <QGroupBox>
#include <QListWidget>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>

#include "exam.h"
#include "mainwidget.h"

ExamWidget::ExamWidget(QList<Exam::Ptr> &finals, QList<Exam::Ptr> &midterms, QWidget *parent) :
    QWidget(parent), midterms(midterms), finals(finals)
{
    MainWidget *mw = dynamic_cast<MainWidget *>(parent);

    tabWidget = new QTabWidget;

    // FINALS
    finalScheduleGroupBox = new QGroupBox;
    finalList = new QListWidget;
    finalDateEdit = new QDateEdit;
    finalNightCheck = new QCheckBox(tr("Night Exam"));
    addFinalButton = new QPushButton(tr("ADD EXAM"));
    removeFinalButton = new QPushButton(tr("REMOVE EXAM"));
    finalDateEdit->setDate(QDate::currentDate());
    finalDateEdit->setCalendarPopup(true);

    tabWidget->addTab(finalScheduleGroupBox, "Midterms");
    tabWidget->addTab(finalScheduleGroupBox, "Finals");

    finalList->setStatusTip("The list of exams for the selected staff member.");


    connect(addFinalButton, SIGNAL(clicked()), this, SLOT(addFinal()));
    connect(this, SIGNAL(OnFinalAdded(Exam::Ptr)), mw, SLOT(addFinal(Exam::Ptr)));

    connect(removeFinalButton, SIGNAL(clicked()), this, SLOT(removeFinal()));
    connect(this, SIGNAL(OnFinalRemoved(Exam::Ptr)), mw, SLOT(removeFinal(Exam::Ptr)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(finalList,0,0,4,1);
    layout->addWidget(finalDateEdit,0,1);
    layout->addWidget(finalNightCheck,1,1);
    layout->addWidget(addFinalButton,2,1);
    layout->addWidget(removeFinalButton,3,1);

    finalScheduleGroupBox->setLayout(layout);

    // MIDTERMS
    midtermScheduleGroupBox = new QGroupBox;
    midtermList = new QListWidget;
    midtermDateEdit = new QDateEdit;
    midtermNightCheck = new QCheckBox(tr("Night Exam"));
    addMidtermButton = new QPushButton(tr("ADD EXAM"));
    removeMidtermButton = new QPushButton(tr("REMOVE EXAM"));
    midtermDateEdit->setDate(QDate::currentDate());
    midtermDateEdit->setCalendarPopup(true);

    midtermList->setStatusTip("The list of exams for the selected staff member.");

    connect(addMidtermButton, SIGNAL(clicked()), this, SLOT(addMidterm()));
    connect(this, SIGNAL(OnMidtermAdded(Exam::Ptr)), mw, SLOT(addMidterm(Exam::Ptr)));

    connect(removeMidtermButton, SIGNAL(clicked()), this, SLOT(removeMidterm()));
    connect(this, SIGNAL(OnMidtermRemoved(Exam::Ptr)), mw, SLOT(removeMidterm(Exam::Ptr)));

    layout = new QGridLayout;
    layout->addWidget(midtermList,0,0,4,1);
    layout->addWidget(midtermDateEdit,0,1);
    layout->addWidget(midtermNightCheck,1,1);
    layout->addWidget(addMidtermButton,2,1);
    layout->addWidget(removeMidtermButton,3,1);

    midtermScheduleGroupBox->setLayout(layout);

    tabWidget->addTab(midtermScheduleGroupBox, "Midterms");
    tabWidget->addTab(finalScheduleGroupBox, "Finals");

    QGridLayout *up_layout = new QGridLayout;
    up_layout->addWidget(tabWidget);
    setLayout(up_layout);
}

ExamWidget::~ExamWidget() {
    delete finalScheduleGroupBox;
}

void ExamWidget::reset() {
    finalList->clear();
    finalDateEdit->setDate(QDate::currentDate());
    finalNightCheck->setChecked(false);

    midtermList->clear();
    midtermDateEdit->setDate(QDate::currentDate());
    midtermNightCheck->setChecked(false);
}

void ExamWidget::setFinals(const QList<Exam::Ptr> &in) {
    foreach (Exam::Ptr ex, in)
    {
        QString text = ex->toString("dd/MM/yyyy");
        if(ex->isNight()) {
            text += " (night)";
        }

        QListWidgetItem *i = new QListWidgetItem(text);
        i->setData(Qt::UserRole, ex->getId());
        finalList->insertItem(0,i);
    }
}

void ExamWidget::getFinals(QList<Exam::Ptr > &out) {
    out.clear();
    for (int i = 0; i < finalList->count(); i++) {
        QListWidgetItem *item = finalList->item(i);

        int id = item->data(Qt::UserRole).toInt();
        out.append(finals[id]);
    }
}

void ExamWidget::setMidterms(const QList<Exam::Ptr> &in) {
    foreach (Exam::Ptr ex, in) {
        QString text = ex->toString("dd/MM/yyyy");
        if(ex->isNight()) {
            text += " (night)";
        }

        QListWidgetItem *i = new QListWidgetItem(text);
        i->setData(Qt::UserRole, ex->getId());
        midtermList->insertItem(0,i);
    }
}

void ExamWidget::getMidterms(QList<Exam::Ptr > &out) {
    out.clear();
    for (int i = 0; i < midtermList->count(); i++) {
        QListWidgetItem *item = midtermList->item(i);

        int id = item->data(Qt::UserRole).toInt();
        out.append(midterms[id]);
    }
}

void ExamWidget::addFinal() {
    Exam::Ptr e(new Exam(finals.count(),
                         finalDateEdit->date(),
                         false,
                         finalNightCheck->isChecked()));

    QListWidgetItem *item = new QListWidgetItem();

    if (finalNightCheck->isChecked())
        item->setText(e->toString("dd/MM/yyyy") + " (night)");
    else
        item->setText(e->toString("dd/MM/yyyy"));

    item->setData(Qt::UserRole,e->getId());

    finalList->insertItem(0,item);//adds the list item to the small exams list

    emit OnFinalAdded(e);
}

void ExamWidget::addMidterm() {
    Exam::Ptr e(new Exam(midterms.count(),
                         midtermDateEdit->date(),
                         true,
                         midtermNightCheck->isChecked()));

    QListWidgetItem *item = new QListWidgetItem();

    if (midtermNightCheck->isChecked())
        item->setText(e->toString("dd/MM/yyyy") + " (night)");
    else
        item->setText(e->toString("dd/MM/yyyy"));

    item->setData(Qt::UserRole,e->getId());

    midtermList->insertItem(0,item);//adds the list item to the small exams list

    emit OnMidtermAdded(e);
}

void ExamWidget::removeFinal() {
    QListWidgetItem *i;
    i = finalList->takeItem(finalList->currentRow());
    Exam::Ptr e = finals[i->data(Qt::UserRole).toInt()];
    delete i;

    emit OnFinalRemoved(e);
}

void ExamWidget::removeMidterm() {
    QListWidgetItem *i;
    i = midtermList->takeItem(midtermList->currentRow());
    Exam::Ptr e = midterms[i->data(Qt::UserRole).toInt()];
    delete i;

    emit OnMidtermRemoved(e);
}
