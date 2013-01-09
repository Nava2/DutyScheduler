#ifndef MULTISELECTCALENDARWIDGET_H
#define MULTISELECTCALENDARWIDGET_H

#include <QObject>
#include <QEvent>
#include <QCalendarWidget>
#include <QTableView>

class MultiSelectCalendarFilter : public QObject {
    Q_OBJECT

public:
    explicit MultiSelectCalendarFilter(QCalendarWidget *const target, QObject *parent = nullptr);


signals:
    void selectCoord(const int row, const int col);

protected:
    enum STATE {
        INITIAL = 0,
        STARTED
    };

    bool eventFilter(QObject *obj, QEvent *event);

private:
    /*!
     * \brief getFirstLastIndex Gets the first and last calendar day's index
     *          within the QTableView.
     * \param first
     * \param last
     */
    void getFirstLastIndex(int &first, int &last);

    QCalendarWidget *_target;
    QTableView *_tbl;

    QModelIndex startIdx, endIdx;

    STATE _state;
};

class MultiSelectCalendarWidget : public QObject
{
    Q_OBJECT
public:
    explicit MultiSelectCalendarWidget(QCalendarWidget * const cal, QObject *parent = 0);

    virtual
    ~MultiSelectCalendarWidget();
    
    QCalendarWidget *calendar();

    QColor selectedColour() const;
    void selectedColour(const QColor &newColour);

signals:
    /*!
     * \brief ToggleSelected emitted when a date's selection is toggled
     * \param date The date changed
     * \param selected True if now selected
     */
    void ToggleSelected(const QDate &date, bool selected);
    
public slots:
    /*!
     * \brief onSelectCoord Selects the row/col passed.
     * \param row
     * \param col
     */
    void onSelectCoord(const int row, const int col);
    

private:
    QCalendarWidget *_cal;

    QColor _selectedColour;
};

#endif // MULTISELECTCALENDARWIDGET_H
