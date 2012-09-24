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
    enum _FileExtension { CSV, JSON, UNKWN, BAD };
    enum _IOType { SCHEDULE, STAFF, CSV_EXPORT };

    typedef enum _FileExtension FileExtension;
    typedef enum _IOType IOType;

    IOHandler();
    virtual
    ~IOHandler();

    bool loadStaffTeam(QString fileName, StaffList &staffList, QList<Exam::Ptr> &finalsList, QList<Exam::Ptr> &midtermsList);
    bool saveStaffTeam(const QString &fileName, const StaffList &staffList, const QList<Exam::Ptr> &finals, const QList<Exam::Ptr> &midterms);

    bool loadSchedule(const QString &fileName, const StaffList &team, QList<SDate> &dateList, QList<QList<QString> *> &nightClasses, QList<int > &donsNeeded, QList<int> &rasNeeded );
    bool saveSchedule(const QString &fileName, QList<SDate> &dateList, QList<QList<QString > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );

    void getErrorInfo(QString &title, QString &msg);
    void clearErrorInfo();

    void setCurrentStaffFile(const QString &staffFile);
    void setCurrentScheduleFile(const QString &scheduleFile);

    QString getCurrentStaffFile() const;
    QString getCurrentScheduleFile() const;

    bool checkFileName(const QString &fileName, FileExtension *ext = NULL);

    bool exportSchedule(const QString &filePath);

    QString getSaveFileName(QWidget *parent, const IOType = STAFF);
    QString getOpenFileName(QWidget *parent, const IOType = STAFF);

    bool cleanUpAutoSave();

private:
    QString errorMsg, errorTitle;

    QString currentStaffFile, currentScheduleFile;

    void setErrorInfo(const QString &msg, const QString &title);

    // json
    bool loadStaffTeamJson(QFile &file, StaffList &staffList, QList<Exam::Ptr> &finalList, QList<Exam::Ptr> &midtermList);
    bool saveStaffTeamJson(QFile &file, const StaffList &sList, const QList<Exam::Ptr> &finalList, const QList<Exam::Ptr> &midtermList);

    bool loadScheduleJson(QFile &file, QList<SDate> &dateList, QList<QList<QString > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleJson(QFile &file, QList<SDate> &dateList, QList<QList<QString > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );

    // csv
    bool loadStaffTeamFile(QFile &file, StaffList &staffList, QList<Exam::Ptr> &examList);
    bool saveStaffTeamFile(QFile &file, const StaffList &staffList, const QList<Exam::Ptr> &examList);

    bool loadScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<QString > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<QString > *> &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );


};

#endif // IOHANDLER_H
