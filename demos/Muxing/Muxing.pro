#-------------------------------------------------
#
# Project created by QtCreator 2016-10-06T02:58:15
#
#-------------------------------------------------

TEMPLATE = app
CONFIG += console c++11
TARGET = Muxing

win32{
    DEFINES +=__STDC_FORMAT_MACROS
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


SOURCES += main.cpp \
    fileio.cpp \
    stream.cpp \
    audio.cpp \
    video.cpp

HEADERS += \
    fileio.h \
    stream.h \
    common.h \
    ffmpeg_muxer.h \
    audio.h \
    video.h

