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
    QApplication app(argc, argv);
    app.setOrganizationName("Tao");
    app.setOrganizationDomain("Tao");

#ifdef USE_OPENGL
    // asks for a OpenGL 3.2 debug context using the Core profile
    auto format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);

    OpenGLWindow w;
    w.show();
#else
    Widget widget;
    widget.show();
#endif
    return app.exec();
}
