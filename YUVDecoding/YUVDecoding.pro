TEMPLATE = app
CONFIG += c++11 console
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
SOURCES += main.cpp
