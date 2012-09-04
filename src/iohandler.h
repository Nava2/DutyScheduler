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
    enum _FileExtension { CSV, JSON, BAD };
    typedef enum _FileExtension FileExtension;

    IOHandler();
    bool loadStaffTeam(const QString &fileName, QList<Staff*> &staffList, QList<Exam*> &examList);
    bool saveStaffTeam(const QString &fileName, const QList<Staff *> &staffList, const QList<Exam *> &examList);


    void getErrorInfo(QString &title, QString &msg);
    void clearErrorInfo();
    QString getCurrentStaffFile() const { return currentStaffFile; }
    bool checkFileName(const QString &fileName, FileExtension *ext = NULL);

private:
    QString errorMsg, errorTitle;

    QString currentStaffFile;

    void setErrorInfo(const QString &msg, const QString &title);

    // json
    bool loadStaffTeamJson(QFile &file, QList<Staff*> &staffList, QList<Exam*> &examList);
    bool saveStaffTeamJson(QFile &file, const QList<Staff *> &sList, const QList<Exam *> &eList);

    // csv
    bool loadStaffTeamFile(QFile &file, QList<Staff *> &staffList, QList<Exam *> &examList);
    bool saveStaffTeamFile(QFile &file, const QList<Staff *> &staffList, const QList<Exam *> &examList);


};

#endif // IOHANDLER_H
