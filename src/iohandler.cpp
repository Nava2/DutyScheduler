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

void IOHandler::getErrorInfo(QString &msg, QString &title) {
   msg = errorMsg;
   title = errorTitle;
}

void IOHandler::setErrorInfo(const QString &msg, const QString &title) {
    errorMsg = msg;
    errorTitle = title;
}

bool IOHandler::loadStaffTeam(const QString &fileName, QList<staff*> &staffList, QList<exam*> &examList)
{
    // initial check
    if (fileName.isEmpty()) {
        setErrorInfo("Open Staff Team", "Incorrect File must be valid file.");
        return false;
    }


    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        setErrorInfo("Duty Scheduler", "Cannot read file.");
        return false;
    }

    bool result = false;
    if(fileName.right(4) == ".txt")
    {
        result = loadStaffTeamFile(file, staffList, examList);
    } else if (fileName.right(5) == ".json") {
        result = loadStaffTeamJson(file, staffList, examList);
    } else {
        setErrorInfo("Open Staff Team","Incorrect File invalid extension.");
        result = false;
    }

    if (result) {
        currentStaffFile = file;
    }

    return result;
}


bool IOHandler::loadStaffTeamJson(QFile &file, QList<staff*> &staffList, QList<exam*> &examList) {

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
        staff *pStaff = new staff(val.toMap());
        staffList.append(pStaff);
    }

    // read exam objects
    QVariantList exams = v["exams"].toList();

    // repeat the same steps as the staff members
    foreach ( QVariant val, exams ) {
        exam *pExam = new exam(val.toMap());
        examList.append(pExam);
    }

    return true;
}

//FILE HANDLERS
bool IOHandler::loadStaffTeamFile(QFile &file, QList<staff *> &staffList, QList<exam *> &examList)
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

        staff *s;
        exam *e;

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

            s = new staff(id,first,last,pos,gen,night);
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

            e = new exam(id,date,e_night);

            examList.append(e);
        }
    }

    currentStaffFile = file;

    return true;
}

bool IOHandler::saveStaffTeam(const QString &fileName, const QList<staff *> &staffList, const QList<exam *> &examList) {
    if (fileName.isEmpty()) {
        setErrorInfo("Save Staff Team", "File name empty.");
        return false;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        setErrorInfo("Save Staff Team File","Cannot write file.");
        return false;
    }

    bool result = false;
    if(fileName.right(4) == ".txt" )
    {
        result = saveStaffTeamFile(file, staffList, examList);
    } else if ( fileName.right(5) == ".json") {
        result = saveStaffTeamJson(file, staffList, examList);
    } else {
        setErrorInfo("Save Staff Team", "Incorrect File, must have extention '.txt'.");
    }

    return result;
}

bool IOHandler::saveStaffTeamFile(QFile &file, const QList<staff *> &staffList, const QList<exam *> &examList)
{
    QTextStream ts(&file);

    ts << "[STAFF]" << endl;
    for (int x = 0; x < staffList.size(); x++)
    {
        staff *t_staff = staffList.at(x);

        ts << QString::number(x) << ","
           << t_staff->getFirstName() << ","
           << t_staff->getLastName() << ","
           << QString(t_staff->getPosition()?"D":"R") << ","
           << QString(t_staff->getGender()?"M":"F") << ","
           << QString::number(t_staff->getNightClass()) << ","
           << t_staff->getAvailability()
           << t_staff->getExams() << endl;
    }

    ts << "[EXAMS]" << endl;

    foreach (exam *ex, examList)
    {
        ts << QString::number(ex->getId()) << ","
           << ex->getDate() << ","
           << QString(ex->getNight()?"1":"0") << endl;
    }

    return true;
}

bool IOHandler::saveStaffTeamJson(QFile &file, const QList<staff *> &sList, const QList<exam *> &eList) {

    QVariantMap out;

    QVariantList o_sList;

    foreach (staff *pStaff, sList) {
        QVariantMap sMap;
        pStaff->toJson(sMap);

        o_sList.append(sMap);
    }

    out["team"] = o_sList;

    QVariantList o_sExams;

    foreach (exam *pExam, eList) {
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


