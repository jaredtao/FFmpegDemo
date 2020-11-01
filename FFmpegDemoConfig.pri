include($${FFMPEGDemo_3RDPARTY_TREE}/3rdparty.pri)
BinPath = $$PWD/bin/win64
win32 {
    contains(QT_ARCH, x86_64) {
        #x64
        LIBS += -L$$FFmpegPath/lib/win64
        BinPath = $$PWD/bin/win64
        message("ffmpeg win64")
    } else {
        # 32
        LIBS += -L$$FFmpegPath/lib/win32
        BinPath = $$PWD/bin/win32
        message("ffmpeg win32")
    }
}
macos {
    LIBS += -L$$FFmpegPath/lib/macos
    BinPath = $$PWD/bin/macos
    message("ffmpeg macos")
}

LIBS += -lavcodec \
    -lavdevice \
    -lavfilter \
    -lavformat \
    -lavutil \
    -lpostproc \
    -lswresample \
    -lswscale

DESTDIR = $$BinPath
DEFINES += ResPath=\"\\\"$${PWD}/res/\\\"\"
