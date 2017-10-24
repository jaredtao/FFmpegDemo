TEMPLATE = app
CONFIG += c++11
QT += core gui widgets quick qml
TARGET = Player
#DEFINES += USE_OPENGL
#DEFINES += OUT_VIDEO_TO_FILE
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
    widget.cpp \
    openglwindow.cpp \
    decodertmp.cpp

FORMS +=

HEADERS += \
    widget.h \
    openglwindow.h \
    decodertmp.h

RESOURCES += \
    shader.qrc
