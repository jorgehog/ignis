include(../defaults.pri)

TEMPLATE = app
CONFIG += console

INCLUDEPATH  += $$TOP_PWD/include

LIBS += -L../lib -lignis -lunittest++


TARGET = ignistests

SOURCES = testmain.cpp

HEADERS += \
    testsetup.h \
    testevents.h


