#include <QApplication>
#include <QScreen>

#ifdef USE_OPENGL
#include "openglwindow.h"
#include <QSurfaceFormat>
#else
#include "widget.h"
#endif
int main(int argc, char *argv[])
{
    qSetMessagePattern("log[%{file} %{line} %{function} %{threadid}] %{message}");
    QApplication a(argc, argv);
#ifdef USE_OPENGL
    // asks for a OpenGL 3.2 debug context using the Core profile
    auto format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);

    OpenGLWindow w;
    w.show();
#else
    Widget widget;
    widget.show();
#endif
    return a.exec();
}
