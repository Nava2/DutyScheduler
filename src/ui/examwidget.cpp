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

    tabWidget = new QTabWidget(this);

    // FINALS
    finalScheduleGroupBox = new QGroupBox(this);
    finalList = new QListWidget(this);
    finalDateEdit = new QDateEdit(this);
    finalPeriodCB = new QComboBox(this);

    finalPeriodCB->addItem("Morning", QVariant(Exam::MORNING));
    finalPeriodCB->addItem("Afternoon", QVariant(Exam::AFTERNOON));
    finalPeriodCB->addItem("Night", QVariant(Exam::NIGHT));

    addFinalButton = new QPushButton(tr("ADD EXAM"), this);
    removeFinalButton = new QPushButton(tr("REMOVE EXAM"), this);
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
    layout->addWidget(finalPeriodCB,1,1);
    layout->addWidget(addFinalButton,2,1);
    layout->addWidget(removeFinalButton,3,1);

    finalScheduleGroupBox->setLayout(layout);

    // MIDTERMS
    midtermScheduleGroupBox = new QGroupBox(this);
    midtermList = new QListWidget(this);
    midtermDateEdit = new QDateEdit(this);
    midtermPeriodCB = new QComboBox(this);

    midtermPeriodCB->addItem("Morning", QVariant(Exam::MORNING));
    midtermPeriodCB->addItem("Afternoon", QVariant(Exam::AFTERNOON));
    midtermPeriodCB->addItem("Night", QVariant(Exam::NIGHT));

    addMidtermButton = new QPushButton(tr("ADD EXAM"), this);
    removeMidtermButton = new QPushButton(tr("REMOVE EXAM"), this);
    midtermDateEdit->setDate(QDate::currentDate());
    midtermDateEdit->setCalendarPopup(true);

    midtermList->setStatusTip("The list of exams for the selected staff member.");

    connect(addMidtermButton, SIGNAL(clicked()), this, SLOT(addMidterm()));
    connect(this, SIGNAL(OnMidtermAdded(Exam::Ptr)), mw, SLOT(addMidterm(Exam::Ptr)));

    connect(removeMidtermButton, SIGNAL(clicked()), this, SLOT(removeMidterm()));
    connect(this, SIGNAL(OnMidtermRemoved(Exam::Ptr)), mw, SLOT(removeMidterm(Exam::Ptr)));

    layout = new QGridLayout(this);
    layout->addWidget(midtermList,0,0,4,1);
    layout->addWidget(midtermDateEdit,0,1);
    layout->addWidget(midtermPeriodCB,1,1);
    layout->addWidget(addMidtermButton,2,1);
    layout->addWidget(removeMidtermButton,3,1);

    midtermScheduleGroupBox->setLayout(layout);

    tabWidget->addTab(midtermScheduleGroupBox, "Midterms");
    tabWidget->addTab(finalScheduleGroupBox, "Finals");

    QGridLayout *up_layout = new QGridLayout(this);
    up_layout->addWidget(tabWidget);
    setLayout(up_layout);
}

ExamWidget::~ExamWidget() {
    delete finalScheduleGroupBox;
}

void ExamWidget::reset() {
    finalList->clear();
    finalDateEdit->setDate(QDate::currentDate());
    finalPeriodCB->setCurrentIndex(0);

    midtermList->clear();
    midtermDateEdit->setDate(QDate::currentDate());
    midtermPeriodCB->setCurrentIndex(0);
}

void ExamWidget::getExams(QList<Exam::Ptr > &fOut, QList<Exam::Ptr> &mOut) {
    getFinals(fOut);
    getMidterms(mOut);
}

void ExamWidget::setExams(const QList<Exam::Ptr> &fIn, const QList<Exam::Ptr> &mIn) {
    reset();

    setFinals(fIn);
    setMidterms(mIn);
}

void ExamWidget::setFinals(const QList<Exam::Ptr> &in) {
    foreach (Exam::Ptr ex, in)
    {
        QString text = ex->toString("dd/MM/yyyy") + " (%1)";
        switch(ex->getPeriod()) {
        case Exam::NIGHT:
            text = text.arg("Night");
            break;
        case Exam::AFTERNOON:
            text = text.arg("Afternoon");
            break;
        case Exam::MORNING:
            text = text.arg("Morning");
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
        QString text = ex->toString("dd/MM/yyyy") + " (%1)";
        switch(ex->getPeriod()) {
        case Exam::NIGHT:
            text = text.arg("Night");
            break;
        case Exam::AFTERNOON:
            text = text.arg("Afternoon");
            break;
        case Exam::MORNING:
            text = text.arg("Morning");
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
    int period = finalPeriodCB->itemData(finalPeriodCB->currentIndex()).toInt();

    Exam::Ptr e(new Exam(finals.count(),
                         finalDateEdit->date(),
                         static_cast<Exam::Period>(period)));

    QString text = e->toString("dd/MM/yyyy") + " (%1)";
    switch(e->getPeriod()) {
    case Exam::NIGHT:
        text = text.arg("Night");
        break;
    case Exam::AFTERNOON:
        text = text.arg("Afternoon");
        break;
    case Exam::MORNING:
        text = text.arg("Morning");
    }

    bool examInList = false;
    foreach (Exam::Ptr ptr, finals) {
        if (*ptr == *e) {
            for (int i = 0; i < finalList->count(); i++ ) {
                if (finalList->item(i)->text() == text) {
                    return;
                }
            }

            examInList = true;
            e = ptr; // use the stored ptr, let the other go out of scope
        }
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(text);
    item->setData(Qt::UserRole, e->getId());

    finalList->insertItem(0,item);//adds the list item to the small exams list

    if (!examInList)
        emit OnFinalAdded(e);
}

void ExamWidget::addMidterm() {
    int period = midtermPeriodCB->itemData(midtermPeriodCB->currentIndex()).toInt();

    Exam::Ptr e(new Exam(midterms.count(),
                         midtermDateEdit->date(),
                         static_cast<Exam::Period>(period)));

    QString text = e->toString("dd/MM/yyyy") + " (%1)";
    switch(e->getPeriod()) {
    case Exam::NIGHT:
        text = text.arg("Night");
        break;
    case Exam::AFTERNOON:
        text = text.arg("Afternoon");
        break;
    case Exam::MORNING:
        text = text.arg("Morning");
    }

    // don't re-add it if already there..
    bool examInList = false;
    foreach (Exam::Ptr ptr, midterms) {
        if (*ptr == *e) {
            for (int i = 0; i < midtermList->count(); i++ ) {
                if (midtermList->item(i)->text() == text) {
                    return;
                }
            }

            examInList = true;
            e = ptr; // use the stored ptr, let the other go out of scope
        }
    }

    QListWidgetItem *item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, e->getId());

    midtermList->insertItem(0,item);//adds the list item to the small exams list

    if (!examInList)
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

//    qDebug() << "eid =" << i->data(Qt::UserRole).toInt() << " :: count =" << midterms.size();

    Exam::Ptr e = midterms[i->data(Qt::UserRole).toInt()];
    delete i;

    emit OnMidtermRemoved(e);
}
