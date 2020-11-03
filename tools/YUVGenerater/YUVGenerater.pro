TEMPLATE = app
CONFIG -= qt
CONFIG += console c++17 utf8_source

SOURCES += main.cpp

libyuvInclude=E:/Dev/Tools/libyuv/include
libyuvBin=E:/Dev/Tools/libyuv-install/bin/Debug
CONFIG(debug,debug|release){
    libyuvBin=E:/Dev/Tools/libyuv-install/bin/Debug
} else {
    libyuvBin=E:/Dev/Tools/libyuv-install/bin/Release
}
INCLUDEPATH += $$libyuvInclude
LIBS += -L$$libyuvBin
LIBS += -lyuv

DESTDIR = $$libyuvBin

resPath=$$clean_path($$PWD/../../res/)
DEFINES += ResPath=\"L\\\"$${resPath}\\\"\"
