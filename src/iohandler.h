#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QString>
#include <QList>
#include <QFile>

#include "staff.h"
#include "exam.h"
#include "sdate.h"
#include "stafflist.h"


class IOHandler
{
public:
    enum _FileExtension { CSV, JSON, BAD };
    typedef enum _FileExtension FileExtension;

    IOHandler();
    bool loadStaffTeam(const QString &fileName, StaffList &staffList, QList<Exam::Ptr> &examList);
    bool saveStaffTeam(const QString &fileName, const StaffList &staffList, const QList<Exam::Ptr> &examList);

    bool loadSchedule(const QString &fileName, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveSchedule(const QString &fileName, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );

    void getErrorInfo(QString &title, QString &msg);
    void clearErrorInfo();
    QString getCurrentStaffFile() const { return currentStaffFile; }
    bool checkFileName(const QString &fileName, FileExtension *ext = NULL);

private:
    QString errorMsg, errorTitle;

    QString currentStaffFile, currentScheduleFile;

    void setErrorInfo(const QString &msg, const QString &title);

    // json
    bool loadStaffTeamJson(QFile &file, StaffList &staffList, QList<Exam::Ptr> &examList);
    bool saveStaffTeamJson(QFile &file, const StaffList &sList, const QList<Exam::Ptr> &eList);

    bool loadScheduleJson(QFile &file, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleJson(QFile &file, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );

    // csv
    bool loadStaffTeamFile(QFile &file, StaffList &staffList, QList<Exam::Ptr> &examList);
    bool saveStaffTeamFile(QFile &file, const StaffList &staffList, const QList<Exam::Ptr> &examList);

    bool loadScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<int > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );


};

#endif // IOHANDLER_H
