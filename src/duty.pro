#-------------------------------------------------
#
# Project created by QtCreator 2012-07-12T19:33:51
#
#-------------------------------------------------

QT       += core gui

TARGET = duty
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x


SOURCES += main.cpp \
    staff.cpp \
    sdate.cpp \
    schedulewizzard.cpp \
    schedulewidget.cpp \
    mainwindow.cpp \
    mainwidget.cpp \
    exam.cpp \
    myqlistwidget.cpp \
    iohandler.cpp \
    availabilitywidget.cpp \
    availabledate.cpp \
    schedviewer.cpp \
    stafflist.cpp \
    availrangewidget.cpp \
    examwidget.cpp

HEADERS  += \
    staff.h \
    sdate.h \
    schedulewizzard.h \
    schedulewidget.h \
    mainwindow.h \
    mainwidget.h \
    exam.h \
    myqlistwidget.h \
    iohandler.h \
    availabilitywidget.h \
    availabledate.h \
    schedviewer.h \
    stafflist.h \
    availrangewidget.h \
    examwidget.h

unix|win32: LIBS += -lqjson

unix: LIBS += -L/usr/local/lib/
