#include "staff.h"

staff::staff()
{
    id = 127;
    firstName = "Art";
    lastName = "Vandelay";
    position = false;
    gender = false;
    nightClass = 0;
    exams = "";
    availability = "";
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;
}

staff::staff(int i, QString first, QString last, bool pos, bool gen, int night)
{
    id = i;
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    exams = "";
    availability = "";
    numShifts = 0;
    numWeekendShifts = 0;
    numAMShifts = 0;
}

staff::~staff()
{

}

void staff::update(QString first, QString last, bool pos, bool gen, int night)
{
    firstName = first;
    lastName = last;
    position = pos;
    gender = gen;
    nightClass = night;
    exams = "";

}


int staff::getId()
{   return id;  }

QString staff::getFirstName()
{   return firstName;   }

QString staff::getLastName()
{    return lastName;   }

bool staff::getPosition()
{    return position;   }

bool staff::getGender()
{    return gender;     }

int staff::getNightClass()
{   return nightClass;  }

void staff::setId(int i)
{   id = i;             }

void staff::setFirstName(QString name)
{   firstName = name;   }

void staff::setLastName(QString name)
{   lastName = name;    }

void staff::setPosition(bool pos)
{   position = pos;     }

void staff::setGender(bool gen)
{   gender = gen;       }

void staff::setNightClass(int night)
{   nightClass = night; }


void staff::setExams(QString ex)
{
    exams = ex;
}

QString staff::getExams()
{
    return exams;
}

void staff::setAvailability(QString inp)
{
   availability = inp;
}

QString staff::getAvailability()
{
    return availability;
}

void staff::addShift(bool night, bool isAM)
{
    if (isAM)
        numAMShifts++;

    if (night)
        numWeekendShifts++;

    numShifts++;
}

void staff::removeShift(bool night, bool isAM)
{
    if (isAM)
        numAMShifts--;

    if (night)
        numWeekendShifts--;

    numShifts--;
}

int staff::getShifts()
{
    return numShifts;
}

int staff::getWeekendShifts()
{
    return numWeekendShifts;
}

int staff::getAMShifts()
{
    return numAMShifts;
}

