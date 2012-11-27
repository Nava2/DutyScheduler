#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>

#include "staff.h"
#include "exam.h"
#include "sdate.h"
#include "stafflist.h"


class IOHandler
{
public:
    enum _FileExtension { CSV, JSON, UNKWN, BAD };
    enum _IOType { SCHEDULE, STAFF, CSV_EXPORT };

    enum _Position { AM = 2, DON = 0, RA = 1 };

    static QString BLANK_ENTRY;


    typedef enum _FileExtension FileExtension;
    typedef enum _IOType IOType;

    IOHandler();
    virtual
    ~IOHandler();

    bool loadStaffTeam(QString fileName, StaffList &staffList, QList<Exam::Ptr> &finalsList, QList<Exam::Ptr> &midtermsList);
    bool saveStaffTeam(const QString &fileName, const StaffList &staffList, const QList<Exam::Ptr> &finals, const QList<Exam::Ptr> &midterms);

    bool loadSchedule(const QString &fileName, const StaffList &team, QList<SDate> &dateList, QList<QList<QString> > &nightClasses, QList<int > &donsNeeded, QList<int> &rasNeeded );
    bool saveSchedule(const QString &fileName, const QList<SDate> &dateList, const QList<int> &donsNeeded, const QList<int> &rasNeeded );

    void getErrorInfo(QString &title, QString &msg);
    void clearErrorInfo();

    void setCurrentStaffFile(const QString &staffFile);
    void setCurrentScheduleFile(const QString &scheduleFile);

    QString getCurrentStaffFile() const;
    QString getCurrentScheduleFile() const;

    bool checkFileName(const QString &fileName, FileExtension *ext = NULL);

    bool exportSchedule(const QString &filePath,
                        const QList<SDate> &datesList,
                        const StaffList &theTeam,
                        const QMap<QString, QList<int > > &tableMap,
                        const QList<QString > &idList);

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

    bool loadScheduleJson(QFile &file, QList<SDate> &dateList, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleJson(QFile &file, const QList<SDate> &dateList, const QList<int> &donsNeeded, const QList<int> &rasNeeded );

    // csv
    bool loadStaffTeamFile(QFile &file, StaffList &staffList, QList<Exam::Ptr> &examList);
    bool saveStaffTeamFile(QFile &file, const StaffList &staffList, const QList<Exam::Ptr> &examList);

    bool loadScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<QString > > &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded );
    bool saveScheduleFile(QFile &file, const QList<SDate> &dateList, const QList<QList<QString> > &nightClasses, const QList<int> &donsNeeded, const QList<int> &rasNeeded );

    // export methods:
    void export_writeWeekArrays(QTextStream &ts,
                                const QList<int > &maxNeededForWeek,
                                QList<QList<QStringList> > &weekDayLists,
                                const QStringList &writtenDates) const;

};

#endif // IOHANDLER_H
