#include "iohandler.h"

#include "staff.h"
#include "exam.h"
#include "json.h"

#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QVariant>
#include <QVariantList>
#include <QTextStream>

IOHandler::IOHandler() :
    errorMsg(""), errorTitle("")
{
}

void IOHandler::getErrorInfo(QString &title, QString &msg) {
   msg = errorMsg;
   title = errorTitle;
}

void IOHandler::clearErrorInfo() {
   errorMsg = "";
   errorTitle = "";
}

void IOHandler::setErrorInfo(const QString &msg, const QString &title) {
    errorMsg = msg;
    errorTitle = title;
}

bool IOHandler::checkFileName(const QString &fileName, FileExtension *ext) {
    bool result = false;

    if (ext) {
        *ext = BAD;
    }

    // initial check
    if (fileName.isEmpty()) {
        setErrorInfo("Check File Name", "Incorrect File must be valid file.");
        return false;
    }

    if(fileName.right(4) == ".txt")
    {
        result = true;
        if (ext) {
            *ext = CSV;
        }
    } else if (fileName.right(5) == ".json") {
        result = true;

        if (ext) {
            *ext = JSON;
        }
    } else {
        setErrorInfo("Check File Name", "Incorrect File - invalid extension.");

    }

    return result;
}

bool IOHandler::loadStaffTeam(const QString &fileName, QList<Staff*> &staffList, QList<Exam*> &examList)
{
    FileExtension ext;
    bool result = IOHandler::checkFileName(fileName, &ext);

    if (result) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            setErrorInfo("Duty Scheduler", "Cannot read file.");
            return false;
        }

        // act based on extension
        switch (ext) {
        case JSON: {
            result = loadStaffTeamJson(file, staffList, examList);
        } break;
        case CSV: {
            result = loadStaffTeamFile(file, staffList, examList);
        } break;
        case BAD:
        default: {
            return false;
        } break;
        }

        currentStaffFile = fileName;
    } else {
        currentStaffFile = "";
    }

    return result;
}


bool IOHandler::loadStaffTeamJson(QFile &file, QList<Staff*> &staffList, QList<Exam*> &examList) {

    // read the whole file into the QByteArray then put it into a string for
    // parsing
    QByteArray data = file.readAll();
    QString str_data(data);

    // parse using json.h
    bool ok = false;
    QVariantMap v = QtJson::Json::parse(str_data, ok).toMap();

    if (!ok) {
        // no idea why it would, unless someone muddled on their own
        setErrorInfo("Duty Scheduler", "Could not parse Json file");
        return false;
    }

    // read the teams
    QVariantList team = v["team"].toList();

    // for each member, create a new object initialized to the values in the
    // maps' storage
    foreach( QVariant val, team ) {
        Staff *pStaff = new Staff(val.toMap());
        staffList.append(pStaff);
    }

    // read exam objects
    QVariantList exams = v["exams"].toList();

    // repeat the same steps as the staff members
    foreach ( QVariant val, exams ) {
        Exam *pExam = new Exam(val.toMap());
        examList.append(pExam);
    }

    return true;
}

//FILE HANDLERS
bool IOHandler::loadStaffTeamFile(QFile &file, QList<Staff *> &staffList, QList<Exam *> &examList)
{
    QTextStream ts(&file);

    QString currentLine = "";
    QStringList current_Line;


    //IMPORT STAFF MEMBERS
    int id = 999;
    QString first = "";
    QString last = "";
    bool pos = false;
    bool gen = false;
    int night = 0;
    QString avail = "";
    QString exams = "";

    //IMPORT EXAMS
    QString date = "";
    bool e_night = false;


    bool ExamsFlag = false;//this flag tells us where we are in the text file.

    while(!ts.atEnd())
    {
        currentLine = ts.readLine();

        if (ts.atEnd())
            break;

        if (currentLine == "[STAFF]")
            continue;

        if (currentLine == "[EXAMS]")
        {
            ExamsFlag = true;
            continue;
        }

        //split the input line into an array of strings
        current_Line = currentLine.split(",", QString::SkipEmptyParts);

        Staff *s;
        Exam *e;

        if(!ExamsFlag)//looking at staff data
        {
            avail = "";
            exams = "";

            if(current_Line.count() < 6)
            {
                setErrorInfo("ERROR", "ERROR: BAD INPUT FILE. PLEASE RESTART THE PROGRAM.");
                return false;
            }
            id = current_Line.at(0).toInt();
            first = current_Line.at(1);
            last = current_Line.at(2);

            if (current_Line.at(3) == "D")
                pos = true;
            else
                pos = false;

            if (current_Line.at(4) == "M")
                gen = true;
            else
                gen = false;

            night = current_Line.at(5).toInt();

            int y = 6;
            bool flag = true;

            while(flag)
            {

                if (y >= current_Line.size())
                {
                    flag = false;
                }
                else if(current_Line.at(y).startsWith("("))
                {
                    exams += current_Line.at(y) + ",";
                }
                else
                {
                    avail += current_Line.at(y) + ",";
                }

                y++;
            }

            s = new Staff(id,first,last,pos,gen,night);
            s->setAvailability(avail);
            s->setExams(exams);

            staffList.append(s);
        }
        else // we are now looking at exam data
        {
            if(current_Line.count() != 3)
            {
                setErrorInfo("ERROR", "ERROR: BAD INPUT FILE. PLEASE RESTART THE PROGRAM.");
                return false;
            }
            id = current_Line.at(0).toInt();
            date = current_Line.at(1);

            if (current_Line.at(2) == "1")
                e_night = true;
            else
                e_night = false;

            e = new Exam(id,date,e_night);

            examList.append(e);
        }
    }

    currentStaffFile = file.fileName();

    return true;
}

bool IOHandler::saveStaffTeam(const QString &fileName, const QList<Staff *> &staffList, const QList<Exam *> &examList) {

    FileExtension ext;
    bool result = IOHandler::checkFileName(fileName, &ext);

    if (result) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            setErrorInfo("Duty Scheduler", "Cannot read file.");
            return false;
        }

        // act based on extension
        switch (ext) {
        case JSON: {
            result = saveStaffTeamJson(file, staffList, examList);
        } break;
        case CSV: {
            result = saveStaffTeamFile(file, staffList, examList);
        } break;
        case BAD:
        default: {
            return false;
        } break;
        }

        currentStaffFile = fileName;
    } else {
        currentStaffFile = "";
    }

    return result;
}

bool IOHandler::saveStaffTeamFile(QFile &file, const QList<Staff *> &staffList, const QList<Exam *> &examList)
{
    QTextStream ts(&file);

    ts << "[STAFF]" << endl;
    for (int x = 0; x < staffList.size(); x++)
    {
        Staff *t_staff = staffList.at(x);

        ts << QString::number(x) << ","
           << t_staff->getFirstName() << ","
           << t_staff->getLastName() << ","
           << QString(t_staff->getPosition()?"D":"R") << ","
           << QString(t_staff->getGender()?"M":"F") << ","
           << QString::number(t_staff->getNightClass()) << ","
           << t_staff->getAvailabilityStr()
           << t_staff->getExams() << endl;
    }

    ts << "[EXAMS]" << endl;

    foreach (Exam *ex, examList)
    {
        ts << QString::number(ex->getId()) << ","
           << ex->getDate() << ","
           << QString(ex->getNight()?"1":"0") << endl;
    }

    return true;
}

bool IOHandler::saveStaffTeamJson(QFile &file, const QList<Staff *> &sList, const QList<Exam *> &eList) {

    QVariantMap out;

    QVariantList o_sList;

    foreach (Staff *pStaff, sList) {
        QVariantMap sMap;
        pStaff->toJson(sMap);

        o_sList.append(sMap);
    }

    out["team"] = o_sList;

    QVariantList o_sExams;

    foreach (Exam *pExam, eList) {
        QVariantMap eMap;
        pExam->toJson(eMap);

        o_sExams.append(eMap);
    }

    out["exams"] = o_sExams;

    bool ok = false;
    QByteArray ba = QtJson::Json::serialize(out, ok);

    if (ok) {
        file.write(ba);
    }

    return ok;
}


