TEMPLATE = app
CONFIG -= qt
CONFIG += console c++17 utf8_source



#libYuvInstallPath=E:/Tools/libyuv/install/
libYuvInstallPath=E:/Dev/Tools/libyuv-install/


libyuvInclude=$${libYuvInstallPath}include
libyuvBin=$${libYuvInstallPath}bin/Debug
CONFIG(debug,debug|release){
    libyuvBin=$${libYuvInstallPath}bin/Debug
} else {
    libyuvBin=$${libYuvInstallPath}bin/Release
}

INCLUDEPATH += $$libyuvInclude
LIBS += -L$$libyuvBin
LIBS += -lyuv

DESTDIR = $$libyuvBin

load(vcpkg)
useVcpkgLib(fmt)

resPath=$$clean_path($$PWD/../../res/)
DEFINES += ResPath=\"L\\\"$${resPath}/\\\"\"

HEADERS += \
    BMP.h \
    Buffer.h

SOURCES += main.cpp
