TEMPLATE = app
CONFIG += c++11
QT += core gui widgets quick qml
TARGET = Player
DEFINES += USE_OPENGL
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
    encodertmp.cpp \
    widget.cpp \
    openglwindow.cpp

FORMS +=

HEADERS += \
    encodertmp.h \
    widget.h \
    openglwindow.h

RESOURCES += \
    shader.qrc
