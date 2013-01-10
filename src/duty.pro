######################################################################
# Automatically generated by qmake (2.01a) Mon Dec 17 23:39:51 2012
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . . obj ui
INCLUDEPATH += . ui obj

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lqjson

# Input
HEADERS += obj/availabledate.h \
           obj/exam.h \
           obj/sdate.h \
           obj/staff.h \
           obj/stafflist.h \
           ui/availabilitywidget.h \
           ui/availrangewidget.h \
           ui/examwidget.h \
           ui/iohandler.h \
           ui/mainwidget.h \
#           ui/mainwindow.h \
           ui/myqlistwidget.h \
           ui/schedulewidget.h \
           ui/schedulewizzard.h \
           ui/schedviewer.h \
    ui/dsgn/teamwidget.h \
    ui/dsgn/mainwindow.h \
    ui/multiselectcalendarwidget.h \
    ui/ctrl/examcalendar.h
SOURCES += main.cpp \
           obj/availabledate.cpp \
           obj/exam.cpp \
           obj/sdate.cpp \
           obj/staff.cpp \
           obj/stafflist.cpp \
           ui/availabilitywidget.cpp \
           ui/availrangewidget.cpp \
           ui/examwidget.cpp \
           ui/iohandler.cpp \
           ui/mainwidget.cpp \
#           ui/mainwindow.cpp \
           ui/myqlistwidget.cpp \
           ui/schedulewidget.cpp \
           ui/schedulewizzard.cpp \
           ui/schedviewer.cpp \
    ui/dsgn/teamwidget.cpp \
    ui/dsgn/mainwindow.cpp \
    ui/multiselectcalendarwidget.cpp \
    ui/ctrl/examcalendar.cpp

FORMS += \
    ui/dsgn/teamwidget.ui \
    ui/dsgn/mainwindow.ui
