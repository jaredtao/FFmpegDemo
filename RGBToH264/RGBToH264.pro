#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T22:26:58
#
#-------------------------------------------------
TEMPLATE = app
CONFIG += console c++11
#CONFIG -= app_bundle


TARGET = RGBToH264

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

SOURCES += main.cpp \
    convert.cpp

HEADERS += \
    errcode.h \
    convert.h


