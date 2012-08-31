#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QString>
#include <QList>
#include <QFile>

#include "staff.h"
#include "exam.h"


class IOHandler
{
public:
    IOHandler();
    bool loadStaffTeam(const QString &fileName, QList<staff*> &staffList, QList<exam*> &examList);
    bool saveStaffTeam(const QString &fileName, const QList<staff *> &staffList, const QList<exam *> &examList);


    void getErrorInfo(QString &msg, QString &title);

private:
    QString errorMsg, errorTitle;

    QString currentStaffFile;

    void setErrorInfo(const QString &msg, const QString &title);

    // json
    bool loadStaffTeamJson(QFile &file, QList<staff*> &staffList, QList<exam*> &examList);
    bool saveStaffTeamJson(QFile &file, const QList<staff *> &sList, const QList<exam *> &eList);

    // csv
    bool loadStaffTeamFile(QFile &file, QList<staff *> &staffList, QList<exam *> &examList);
    bool saveStaffTeamFile(QFile &file, const QList<staff *> &staffList, const QList<exam *> &examList);


};

#endif // IOHANDLER_H
