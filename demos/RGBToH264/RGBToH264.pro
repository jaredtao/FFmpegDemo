#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T22:26:58
#
#-------------------------------------------------
TEMPLATE = app
CONFIG += console c++11
QT -= gui

include($$PWD/../../FFmpegDemoConfig.pri)


SOURCES += main.cpp \
    convert.cpp

HEADERS += \
    errcode.h \
    convert.h


