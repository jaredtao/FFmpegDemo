TEMPLATE = app
CONFIG -= qt
CONFIG += console c++2z utf8_source


BinPath = $${FFMPEGDemo_SOURCE_TREE}/bin/win64
win32 {
    contains(QT_ARCH, x86_64) {
        BinPath = $${FFMPEGDemo_SOURCE_TREE}/bin/win64
    } else {
        BinPath = $${FFMPEGDemo_SOURCE_TREE}/bin/win32
    }
}
macos {
    BinPath = $${FFMPEGDemo_SOURCE_TREE}/bin/macos
}
DESTDIR = $$BinPath

load(vcpkg)
#useVcpkgLib(fmt yuv)
useVcpkgLib(yuv)

resPath=$$clean_path($${FFMPEGDemo_SOURCE_TREE}/res)
DEFINES += ResPath=\"L\\\"$${resPath}/\\\"\"

HEADERS += \
    BMP.h \
    Buffer.h

SOURCES += main.cpp
