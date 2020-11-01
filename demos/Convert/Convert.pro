#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T12:28:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Convert
TEMPLATE = app

win32{
    FFMPEGPATH = $$PWD/../FFMPEG
}
unix{
    FFMPEGPATH = /usr/local/ffmpeg/shared
}
INCLUDEPATH += $${FFMPEGPATH}/include
DEPENDPATH += $${FFMPEGPATH}/include

LIBS += -L$${FFMPEGPATH}/lib -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale

SOURCES += main.cpp\
        widget.cpp \
    convert.cpp

HEADERS  += widget.h \
    convert.h

FORMS    += widget.ui
