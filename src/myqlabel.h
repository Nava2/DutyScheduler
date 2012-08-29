#ifndef MYQLABEL_H
#define MYQLABEL_H

#include <QLabel>

class MyQLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyQLabel(QWidget *parent = 0);

signals:
    void clicked();

private:
    void mouseReleaseEvent ( QMouseEvent * event );

};

#endif // MYQLABEL_H



