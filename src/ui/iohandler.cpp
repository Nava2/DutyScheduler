#include "iohandler.h"

#include "staff.h"
#include "exam.h"
#include "sdate.h"

#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QVariant>
#include <QVariantList>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QFileDialog>

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include "stafflist.h"

QString IOHandler::BLANK_ENTRY = "\"\"";

IOHandler::IOHandler() :
    errorMsg(""), errorTitle("")
{
}

IOHandler::~IOHandler() {
    cleanUpAutoSave();
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

void IOHandler::setCurrentStaffFile(const QString &staffFile) {
    currentStaffFile = staffFile;
}

void IOHandler::setCurrentScheduleFile(const QString &scheduleFile) {
    currentScheduleFile = scheduleFile;
}

QString IOHandler::getCurrentStaffFile() const {
    return currentStaffFile;
}

QString IOHandler::getCurrentScheduleFile() const {
    return currentScheduleFile;
}

bool IOHandler::cleanUpAutoSave() {
#ifdef QT_COMPILER_INITIALIZER_LISTS
    QList<QString > files { currentStaffFile, currentScheduleFile };
#else
    QList<QString > files;
    files.append(currentStaffFile); files.append(currentScheduleFile);
#endif

    bool successful = true;

    foreach (QString file, files) {
        QFileInfo fiAuto(file + "~"),
                fiManual(file);
        if (!(fiAuto.isFile() && fiManual.isFile())) {
            // one of them doesn't exist, so bleh
            continue;
        }

        if (fiAuto.lastModified() > fiManual.lastModified()) {
            // the auto version is greater than the manual
            // lets leave it because it means its fresher
            continue;
        } else {
            // the manual version > auto
            // delete the auto
            successful &= QFile::remove(fiAuto.path());
        }
    }

    qDebug() << "IOHandler::cleanUpAutoSave Successful:" << successful;

    return successful;
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

    int period_pos = fileName.lastIndexOf(".")+1;

    if (period_pos == 0) {
        *ext = UNKWN;
        result = false;
    } else if(fileName.mid(period_pos, 3) == "txt") {
        result = true;
        if (ext) {
            *ext = CSV;
        }
    } else if (fileName.mid(period_pos, 4) == "json") {
        result = true;

        if (ext) {
            *ext = JSON;
        }
    } else {
        setErrorInfo("Check File Name", "Incorrect File - invalid extension.");

    }

    return result;
}

bool IOHandler::loadStaffTeam(QString filePath, StaffList &staffList, QList<Exam::Ptr> &finalList, QList<Exam::Ptr> &midtermList)
{

    FileExtension ext;
    bool result = IOHandler::checkFileName(filePath, &ext);

    if (!result && ext == UNKWN) {
        // the extension wasn't found, so we default to json

        ext = JSON;
        result = true;
        filePath += ".json";
    }

    if (result) {
        QFile file(filePath);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            setErrorInfo("Duty Scheduler", "Cannot read file.");
            return false;
        }

        // clear all lists:
        staffList.clear();
        finalList.clear();
        midtermList.clear();

        // act based on extension
        switch (ext) {
        case JSON: {
            result = loadStaffTeamJson(file, staffList, finalList, midtermList);

            // remap IDs
            foreach (Exam::Ptr e, midtermList) {
                foreach (QString id, e->getStaff()) {
                    if (staffList[id])
                        staffList[id]->addMidterm(e);
                }
            }
        } break;
        case CSV: {
            result = loadStaffTeamFile(file, staffList, finalList);
            qDebug() << "CSV is outdated.";
        } break;
        case BAD:
        default: {
            return false;
        } break;
        }

        foreach (Exam::Ptr e, finalList) {
            foreach (QString id, e->getStaff()) {
                if (staffList[id])
                    staffList[id]->addFinal(e);
            }
        }

        currentStaffFile = filePath;
    } else {
        currentStaffFile = "";
    }

    return result;
}


bool IOHandler::loadStaffTeamJson(QFile &file, StaffList &staffList, QList<Exam::Ptr> &finals, QList<Exam::Ptr> &midterms) {

    QJson::Parser parser; // define parser

    // read the whole file into the QByteArray then put it into a string for
    // parsing
    QByteArray data = file.readAll();

    // parse using json.h
    bool ok = false;
    QVariantMap v = parser.parse(data, &ok).toMap();

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
        Staff::Ptr pStaff(new Staff(val.toMap()));
        staffList.append(pStaff);
    }

    // read exam objects
    QVariantMap examMap = v["exams"].toMap();
    QVariantList v_finals = examMap["finals"].toList();
    foreach (QVariant qv, v_finals) {
        Exam::Ptr pexam (new Exam(qv.toMap()));
        finals.append(pexam);
    }

    QVariantList v_midterms = examMap["mid"].toList();
    foreach (QVariant qv, v_midterms) {
        Exam::Ptr pexam (new Exam(qv.toMap()));
        midterms.append(pexam);
    }

    return true;
}

//FILE HANDLERS
bool IOHandler::loadStaffTeamFile(QFile &file, StaffList &staffList, QList<Exam::Ptr> &examList)
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
    Exam::Period e_time = Exam::MORNING;


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

        Staff::Ptr s;
        Exam::Ptr e;

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

            s = Staff::Ptr(new Staff(id, first, last,
                                     pos, gen, night));
            s->setAvailability(avail);
            s->setFinals(exams, examList);

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
            QDate date = QDate::fromString(current_Line.at(1), "dd/MM/yyyy");

            if (current_Line.at(2) == "1")
                e_time = Exam::NIGHT;
            else
                e_time = Exam::NIGHT;

            e = Exam::Ptr(new Exam(id, date, false, e_time));

            examList.append(e);
        }
    }

    currentStaffFile = file.fileName();

    return true;
}

bool IOHandler::saveStaffTeam(const QString &fileName,
                              const StaffList &staffList,
                              const QList<Exam::Ptr> &finals,
                              const QList<Exam::Ptr> &midterms) {

    FileExtension ext;
    bool result = IOHandler::checkFileName(fileName, &ext);

    if (result) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            setErrorInfo("Duty Scheduler", "Cannot read file.");
            return false;
        }

        // act based on extension
        switch (ext) {
        case JSON: {
            result = saveStaffTeamJson(file, staffList, finals, midterms);
        } break;
        case CSV: {
            result = saveStaffTeamFile(file, staffList, finals);
            qWarning() << "WARNING: CSV DEPRECIATED.";
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

bool IOHandler::saveStaffTeamFile(QFile &file,
                                  const StaffList &staffList,
                                  const QList<Exam::Ptr> &examList)
{
    QTextStream ts(&file);

    ts << "[STAFF]" << endl;
    for (int x = 0; x < staffList.size(); x++)
    {
        Staff::Ptr t_staff = staffList[x];

        ts << QString::number(x) << ","
           << t_staff->getFirstName() << ","
           << t_staff->getLastName() << ","
           << QString(t_staff->getPosition()?"D":"R") << ","
           << QString(t_staff->getGender()?"M":"F") << ","
           << QString::number(t_staff->getNightClass()) << ","
           << t_staff->getAvailabilityStr()
           << t_staff->getFinalsStr() << endl;
    }

    ts << "[EXAMS]" << endl;

    foreach (Exam::Ptr ex, examList)
    {
        ts << QString::number(ex->getId()) << ","
           << ex->toString("dd/MM/yyyy") << ","
           << (ex->getPeriod() == Exam::NIGHT ? "1" : "0") << endl;
    }

    return true;
}

bool IOHandler::saveStaffTeamJson(QFile &file,
                                  const StaffList &sList,
                                  const QList<Exam::Ptr> &finalList,
                                  const QList<Exam::Ptr> &midtermList) {

    QJson::Serializer serializer;
//    serializer.setIndentMode(QJson::IndentFull);

    QVariantMap out;

    QVariantList o_sList;

    foreach (Staff::Ptr pStaff, sList) {
        QVariantMap sMap;
        *pStaff >> sMap;

        o_sList.append(sMap);
    }

    out["team"] = o_sList;

    QVariantMap o_ExamMap;

    QVariantList o_fExams;
    int id = 0;
    foreach (Exam::Ptr pExam, finalList) {
        if (pExam->getStaff().size() == 0)
            continue;

        QVariantMap eMap;
        *pExam >> eMap;

        eMap["id"] = id++;

        o_fExams.append(eMap);
    }

    QVariantList o_mExams;
    id = 0;
    foreach (Exam::Ptr pExam, midtermList) {
        if (pExam->getStaff().size() == 0)
            continue;

        QVariantMap eMap;
        *pExam >> eMap;

        eMap["id"] = id++;

        o_mExams.append(eMap);
    }

    o_ExamMap["finals"] = o_fExams;
    o_ExamMap["mid"] = o_mExams;

    out["exams"] = o_ExamMap;

    bool ok = false;
    QByteArray ba = serializer.serialize(QVariant(out));

    if (ba.count() != 0) {
        file.write(ba);
        ok = true;
    }

    return ok;
}

/////////////
// SCHEDULES
/////////////

bool IOHandler::loadSchedule(const QString &fileName,
                             const StaffList &team,
                             QList<SDate> &dateList,
                             QList<QList<QString > > &nightClasses,
                             QList<int > &donsNeeded,
                             QList<int > &rasNeeded )
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
            result = loadScheduleJson(file, dateList, donsNeeded, rasNeeded);
        } break;
        case CSV: {
            result = loadScheduleFile(file, dateList, nightClasses, donsNeeded, rasNeeded);
            qWarning() << "WARNING: DO NOT USE CSV.";
        } break;
        case BAD:
        default: {
            return false;
        } break;
        }

        // add each person who can't work due to midterms
        foreach (Staff::Ptr ptr, team) {
            foreach (Exam::Ptr pexam, ptr->getMidterms()) {
                foreach (SDate sd, dateList) {
                    if (sd == *pexam) {
                        sd.addCantWork(ptr->uid());
                        break;
                    }
                }
            }
        }

        team.nightClasses(nightClasses);

        currentScheduleFile = fileName;
    } else {
        currentScheduleFile = "";
    }

    return result;
}

bool IOHandler::saveScheduleJson(QFile &file,
                                 const QList<SDate> &dateList,
                                 const QList<int > &donsNeeded,
                                 const QList<int > &rasNeeded ) {

    QJson::Serializer serializer;
//    serializer.setIndentMode(QJson::IndentMedium);

    QVariantMap out;

    // all the SDate objects
    QVariantList o_dList;

    foreach (SDate date, dateList) {
        QVariantMap dmap;
        date >> dmap;
        o_dList.append(dmap);
    }

    out["calendar"] = o_dList;

//    // 2D array of night classes with IDs
//    QVariantList o_nListAll;
//    for (int i = 0; i < 7; i++) {
//        QList<QString > *nightList = nightClasses[i];
//        QVariantList o_nListSingle;

//        foreach( QString id, *nightList ) {
//           o_nListSingle.append(id);
//        }

//        o_nListAll.append(QVariant(o_nListSingle));
//    }

//    out["night"] = o_nListAll;

    // dons and ras needed by default
    QVariantMap o_needed;
    QVariantList o_donsNeeded, o_rasNeeded;
    foreach(int num, donsNeeded) {
        o_donsNeeded.append(num);
    }
    foreach(int num, rasNeeded) {
        o_rasNeeded.append(num);
    }

    o_needed["dons"] = o_donsNeeded;
    o_needed["ras"] = o_rasNeeded;

    out["need"] = o_needed;

    bool ok = false;
    QByteArray ba = serializer.serialize(QVariant(out));

    if (ba.count() != 0) {
        file.write(ba);
        ok = true;
    }

    return ok;

}

void IOHandler::export_writeWeekArrays(QTextStream &ts,
                                       const QList<int > &maxNeededForWeek,
                                       QList<QList<QStringList > > &weekDayLists,
                                       const QStringList &writtenDates) const {
    // pad the days so that they are all the same length
    for (int p = 0; p < 7; p++)
    {
        for (int i = 0; i < 2; i++) {
            int count = maxNeededForWeek[i] - weekDayLists[p][i].count();
            for(int j = 0; j < count; j++)
                weekDayLists[p][i] += "";
        }
        if (weekDayLists[p][AM].count() == 0) {
            weekDayLists[p][AM] += "";
        }
    }


    //write the stuff out to the file.
    ts << BLANK_ENTRY << ","; // left column
    ts << writtenDates.join(",") << endl;
    QString out;

    // AMs:
    out = "\"\",";
    for (int x = 0; x < 7; x++) {
        out += "\"" + weekDayLists[x][AM][0] + "\",";
    }
    ts << out << endl;

    // put a line between
    for (int i = 0; i < 8; i++) {
        ts << BLANK_ENTRY << ",";
    }
    ts << endl;

    for (int y = 0; y < maxNeededForWeek[DON]; y++) {
        out = BLANK_ENTRY + ",";
        for (int x = 0; x < 7; x++) {
             out += "\"" + weekDayLists[x][DON][y] + "\",";
        }
        ts << out << endl;
    }

    // put a line between
    for (int i = 0; i < 8; i++) {
        ts << BLANK_ENTRY << ",";
    }
    ts << endl;

    for (int y = 0; y < maxNeededForWeek[RA]; y++) {
        out = "\"\",";
        for (int x = 0; x < 7; x++) {
             out += "\"" + weekDayLists[x][RA][y] + "\",";
        }
        ts << out << endl;
    }

    // put a line between
    for (int i = 0; i < 8; i++) {
        ts << BLANK_ENTRY << ",";
    }
    ts << endl;
}

bool IOHandler::exportSchedule(const QString &filePath,
                               const QList<SDate> &datesList,
                               const StaffList &theTeam,
                               const QMap<QString, QList<int > > &tableMap,
                               const QList<QString > &idList)
{
    if (filePath.right(4) != ".csv")
    {
        setErrorInfo("Filename must be of type \".csv\"", "Export Schedule");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        setErrorInfo("Cannot write file.", "Export Schedule");
        return false;
    }

    QTextStream ts(&file);

    QDate startDate = datesList.first(),
            endDate = datesList.last();

    QList<QList<QStringList > > weekDayLists; // 3D array, three string lists (AM, Dons, RAs)

    for (int i = 0; i < 7; i++) {
        QList<QStringList> wList;
        for (int j = 0; j < 3; j++) {
            wList += QStringList();
        }
        weekDayLists += wList;
    }


    QStringList writtenDates;
    writtenDates << "" << "" << "" << "" << "" << "" << "";
    QList<int > maxNeededForWeek;
    maxNeededForWeek += 0; // first is dons, then RAs
    maxNeededForWeek += 0;

    //for(int y = 0; y < datesList->at(0)->getDate().dayOfWeek() - 1; y++)        // if the first day is in the middle of the week we need pre-padding
    //    lists.at(y)->append("\"\",");

    for (int dayCount = 0; dayCount <= startDate.daysTo(endDate); dayCount++) {
        int weekDayIndex = datesList[dayCount].dayOfWeek() - 1;

        SDate date = datesList[dayCount];
        QList<QStringList > &dayList = weekDayLists[weekDayIndex];

        writtenDates.replace(weekDayIndex, date.toString("\"ddd MMM d\""));
//            writtenDates.replace(dayOfWeekCounter,"\"" + QDate::shortDayName(date.dayOfWeek()) + " " + QDate::shortMonthName(date.month()) + " " + QString::number(date.day()) + "\"");

        QList<Staff::Ptr> dons, ras;
        Staff::Ptr am;

        bool sp = date.exportOn(theTeam, am, dons, ras);

        if(sp)
        {
            foreach (QStringList role, dayList)
                role += "Special Duty";
        }
        else if (!am)
        {
            dayList[AM] += "NO AM";
        }
        else
        {
            dayList[AM] += am->getFirstName() + " " + am->getLastName()[0];

            foreach (Staff::Ptr pstaff, dons ) {
                dayList[DON] += pstaff->getFirstName() + " " + pstaff->getLastName()[0];
            }

            foreach (Staff::Ptr pstaff, ras) {
                dayList[RA] += pstaff->getFirstName() + " " + pstaff->getLastName()[0];
            }

        }

        if (dayList[RA].count() > maxNeededForWeek[RA]) { // keep track for padding sake
            maxNeededForWeek[RA] = dayList[RA].count();
        }

        if (dayList[DON].count() > maxNeededForWeek[DON]) {
            maxNeededForWeek[DON] = dayList[DON].count();
        }

        if (weekDayIndex == 6) {
            // end of the week, so write information:

            export_writeWeekArrays(ts, maxNeededForWeek, weekDayLists, writtenDates);

            //clean up stuff for the next week.
            maxNeededForWeek[0] = maxNeededForWeek[1] = 0;
            writtenDates.clear();
            writtenDates << "" << "" << "" << "" << "" << "" << "";

            // clear all lists
            for (int i = 0; i < weekDayLists.count(); i++) {
                for (int j = 0; j < weekDayLists[i].count(); j++) {
                    weekDayLists[i][j].clear();
                }
            }

        }
    }

    export_writeWeekArrays(ts, maxNeededForWeek, weekDayLists, writtenDates);

    // Average tables:
    QList<int > donAvgs = tableMap["_dAvgs"],
            raAvgs = tableMap["_rAvgs"];

    ts << "Average:,Total,Weekend,AM Shifts" << endl;
    ts << "Dons:,";
    foreach (int val, donAvgs) {
        ts << QString::number(val) << ",";
    }
    ts << endl;

    for (int i = 0; i < 5; i++) {
        ts << BLANK_ENTRY << ",";
    }
    ts << endl;

    ts << "RAs:,";
    foreach (int val, raAvgs) {
        ts << QString::number(val) << ",";
    }
    ts << BLANK_ENTRY << "," << endl;

    ts << endl << endl;


    ts << "Name,Position,Total,Weekend,AM" << endl;
    foreach (QString id, idList) {
        QList<int > vals = tableMap[id];
        ts << "\"" << theTeam[id]->getFirstName() + " " + theTeam[id]->getLastName()[0] + "\"" << ",";
        ts << "\"" << (theTeam[id]->getPosition() ? "Don" : "RA") << "\",";
        foreach (int val, vals) {
            ts << val << ",";
        }
        ts << endl;
    }

    file.close();

    return true;
}

bool IOHandler::saveSchedule(const QString &fileName,
                             const QList<SDate> &dateList,
                             const QList<int > &donsNeeded,
                             const QList<int > &rasNeeded )
{
    FileExtension ext;
    bool result = IOHandler::checkFileName(fileName, &ext);

    if (result) {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            setErrorInfo("Duty Scheduler", "Cannot open file.");
            return false;
        }

        // act based on extension
        switch (ext) {
        case JSON: {
            result = saveScheduleJson(file, dateList, donsNeeded, rasNeeded);
        } break;
        case CSV: {
            QList<QList<QString > > nightClasses;
            result = saveScheduleFile(file, dateList, nightClasses, donsNeeded, rasNeeded);
        } break;
        case BAD:
        default: {
            return false;
        } break;
        }

        currentScheduleFile = fileName;
    } else {
        currentScheduleFile = "";
    }

    return result;
}

bool IOHandler::loadScheduleFile(QFile &file, QList<SDate> &dateList, QList<QList<QString > > &nightClasses, QList<int > &donsNeeded, QList<int > &rasNeeded ) {

    dateList.clear();

    QTextStream ts(&file);

    QStringList inp = ts.readLine().split(":",QString::SkipEmptyParts);     //get start date
    QDate startDate = QDate::fromString(inp.at(1),"dd/MM/yyyy");

    inp = ts.readLine().split(":",QString::SkipEmptyParts);                 //get end date

    QDate endDate = QDate::fromString(inp.at(1),"dd/MM/yyyy");

    donsNeeded.clear();
    rasNeeded.clear();

    inp = ts.readLine().split(":",QString::SkipEmptyParts);                 //get staff req for each night
    for(int x = 0; x < 7; x++)
    {
        donsNeeded.append(inp.at(x).toInt());
        rasNeeded.append(inp.at(x+7).toInt());
    }

    nightClasses.clear();
    for(int q = 0; q<7; q++)                                                //get night classes
        nightClasses += QList<QString >();

    for(int x = 0; x<7; x++)
    {
        inp = ts.readLine().split("-",QString::KeepEmptyParts);
        if (inp.at(1).isEmpty())
            continue;

        inp = inp.at(1).split(":", QString::SkipEmptyParts);

        QList<QString > list = nightClasses[x];
        for (int y = 0; y < inp.count(); y++)
            list += inp.at(y);

    }

    QDate dateCounter = startDate;;
    int length = startDate.daysTo(endDate) + 1;

    // load all the dates
    for(int z = 0; z<length; z++)
    {
        SDate sDate(dateCounter, false, donsNeeded[dateCounter.dayOfWeek()-1], rasNeeded[dateCounter.dayOfWeek()-1]);
        dateList.append(sDate);
        dateCounter = dateCounter.addDays(1);
    }

    for(int x = 0; x < dateList.count(); x++)                           //fill the dates
    {
        SDate sdate = dateList[x];
        inp = ts.readLine().split(":",QString::KeepEmptyParts);

        if (inp.at(1) == "Y")
        {
            sdate.setSpecial(true);
            continue;
        }

        QStringList unavail = inp.at(2).split(",", QString::SkipEmptyParts);
        for(int z = 0; z < unavail.count(); z++)
            sdate.addCantWork(unavail.at(z));

        sdate.setAM(inp.at(3));             //add the staff to the date object

        QStringList dons_list = inp.at(4).split(",", QString::SkipEmptyParts);
        for(int d = 0; d < dons_list.count(); d++)
        {
            sdate.addStaff(dons_list.at(d), true);      //add staff id to date obj
        }

        QStringList ras_list = inp.at(5).split(",", QString::SkipEmptyParts);
        for(int r = 0; r < ras_list.count(); r++)
        {
            sdate.addStaff(ras_list.at(r), false);      //add staff id to date obj
        }
    }

    return true;
}

bool IOHandler::saveScheduleFile(QFile &file,
                                 const QList<SDate> &dateList,
                                 const QList<QList<QString > > &nightClasses,
                                 const QList<int > &donsNeeded,
                                 const QList<int > &rasNeeded ) {
    QTextStream ts(&file);

    ts << "start:" << (dateList.first()).toString("dd/MM/yyyy") << endl;
    ts << "end:" << (dateList.last()).toString("dd/MM/yyyy") << endl;

    //x:y:z:a:b:c:d:1:2:3:4:5:6:7
    for(int d = 0; d < 7; d++)
        ts << QString::number(donsNeeded[d]) << ":";
    for(int r = 0; r < 7; r++)
        ts << QString::number(rasNeeded[r]) << ":";
    ts << endl;

    //a line for each day of the week with the staff who can't work ids. Monday first.
    //1-id:id:id:
    //2-id:id:
    // etc
    for(int n = 0; n<7; n++)
    {
        ts << QString::number(n) << "-";
        for(int i = 0; i < nightClasses[n].count(); i++)
        {
            ts << nightClasses[n][i] << ":";
        }
        ts << endl;
    }

    // dd/MM/yyyy:isSpecial:cantwork,cantwork,cantwork:AM:don,don,don,don:ra,ra,ra,ra,ra
    foreach (SDate sdate, dateList)
    {
        ts << sdate.toString("dd/MM/yyyy") << ":";

        if(sdate.isSpecial())
            ts << "Y:";
        else
            ts << "N:";

        ts << sdate.getCantWorkStr() << ":";
        ts << sdate.getAM() << ":";
        ts << sdate.getDonsStr() << ":";
        ts << sdate.getRasStr() << endl;
    }

    return true;
}

bool IOHandler::loadScheduleJson(QFile &file, QList<SDate> &dateList, QList<int > &donsNeeded, QList<int > &rasNeeded ) {

    QJson::Parser parser;

    // read the whole file into the QByteArray then put it into a string for
    // parsing
    QByteArray data = file.readAll();

    // parse using json.h
    bool ok = false;
    QVariantMap v = parser.parse(data, &ok).toMap();

    if (!ok) {
        // no idea why it would, unless someone muddled on their own
        setErrorInfo("Duty Scheduler", "Could not parse Json file");
        return false;
    }

    // all the SDate objects
    QVariantList dList = v["calendar"].toList();

    dateList.clear();

    foreach (QVariant _date, dList) {
        QVariantMap dmap = _date.toMap();
        SDate date(dmap);
        dateList.append(date);
    }

    // dons and ras needed by default
    donsNeeded.clear();
    rasNeeded.clear();

    QVariantMap needed = v["need"].toMap();
    QVariantList vdonsNeeded = needed["dons"].toList(),
            vrasNeeded = needed["ras"].toList();
    foreach(QVariant num, vdonsNeeded) {
        donsNeeded.append(num.toInt());
    }
    foreach(QVariant num, vrasNeeded) {
        rasNeeded.append(num.toInt());
    }

    return true;

}

QString IOHandler::getSaveFileName(QWidget *parent, const IOType type) {
    QString caption(""), dir(QDir::homePath()),
            filters(""), selectedFilter(""),
            *currentMember(NULL);

    switch (type) {
    case STAFF: {
        caption = "Save Staff File..";
        filters = "Text files (*.txt);;Json Files (*.json)";
        selectedFilter = "Json Files (*.json)";

        if (!currentStaffFile.isEmpty()) {
            dir = QFileInfo(currentStaffFile).dir().path();
        }

        currentMember = &currentStaffFile;
    } break;
    case SCHEDULE: {
        caption = "Save Schedule File..";
        filters = "Text files (*.txt);;Json Files (*.json)";
        selectedFilter = "Json Files (*.json)";

        if (!currentScheduleFile.isEmpty()) {
            dir = QFileInfo(currentScheduleFile).dir().path();
        } else if (!currentStaffFile.isEmpty()) {
            dir = QFileInfo(currentStaffFile).dir().path();
        }

        currentMember = &currentScheduleFile;
    } break;

    case CSV_EXPORT: {
        caption = "Select CSV Export File..";
        filters = "Comma Separated Values (*.csv)";
        selectedFilter = filters;

        if (!currentScheduleFile.isEmpty()) {
            dir = QFileInfo(currentScheduleFile).dir().path();
        } else if (!currentStaffFile.isEmpty()) {
            dir = QFileInfo(currentStaffFile).dir().path();
        }

        currentMember = nullptr;
    }
    default: {
        // baad
    } break;
    }

    if (caption.isEmpty()) {
        // baaad
        return "";
    }

    QString file = QFileDialog::getSaveFileName(parent, caption, dir, filters, &selectedFilter);
    if (!file.isEmpty() && currentMember) {
        *currentMember = file;
    }

    return file;
}

QString IOHandler::getOpenFileName(QWidget *parent, const IOType type) {
    QString caption(""), dir(QDir::homePath()),
            filters(""), selectedFilter(""),
            *currentMember = NULL;

    switch (type) {
    case STAFF: {
        caption = "Open Staff File..";
        filters = "Text files (*.txt);;Json Files (*.json)";
        selectedFilter = "Json Files (*.json)";

        if (!currentStaffFile.isEmpty()) {
            dir = QFileInfo(currentStaffFile).dir().path();
        }

        currentMember = &currentStaffFile;
    } break;
    case SCHEDULE: {
        caption = "Open Schedule File..";
        filters = "Text files (*.txt);;Json Files (*.json)";
        selectedFilter = "Json Files (*.json)";

        if (!currentScheduleFile.isEmpty()) {
            dir = QFileInfo(currentScheduleFile).dir().path();
        } else if (!currentStaffFile.isEmpty()) {
            dir = QFileInfo(currentStaffFile).dir().path();
        }

        currentMember = &currentScheduleFile;
    } break;
    default: {
        // baad
    } break;
    }

    if (caption.isEmpty()) {
        // baaad
        return "";
    }

    QString file = QFileDialog::getOpenFileName(parent, caption, dir, filters, &selectedFilter);
    if (!file.isEmpty() && currentMember) {
        *currentMember = file;
    }

    return file;
}


