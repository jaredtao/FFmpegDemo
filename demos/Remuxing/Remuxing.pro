#-------------------------------------------------
#
# Project created by QtCreator 2016-10-06T02:58:15
#
#-------------------------------------------------

TEMPLATE = app
CONFIG += console c++11
TARGET = Remuxing



win32{
    FFMPEGPATH = $$PWD/../FFMPEG
}
unix{
    FFMPEGPATH = /usr/local/ffmpeg/shared
}

INCLUDEPATH += $${FFMPEGPATH}/include
DEPENDPATH += $${FFMPEGPATH}/include

unix|win32: LIBS += -L$${FFMPEGPATH}/lib/ \
    -lavdevice \
    -lavcodec \
    -lavformat \
    -lswresample \
    -lavutil \
    -lavfilter \
    -lswscale

HEADERS += \
    src/args.h \
    src/ffmpegHeader.h

SOURCES += \
    src/args.cpp \
    src/ffmpegHeader.cpp \
    src/main.cpp



