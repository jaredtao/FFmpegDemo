TEMPLATE = app
CONFIG += c++11
QT += core gui widgets quick qml
TARGET = Player
DEFINES += USE_OPENGL
#DEFINES += OUT_VIDEO_TO_FILE

include($$PWD/../../FFmpegDemoConfig.pri)

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
