#include "openglwindow.h"

#include "libavformat/avformat.h"
#include <QPainter>
#include <string.h>
OpenGLWindow::OpenGLWindow(QWidget *parent)
    : QOpenGLWidget(parent), mLogger(0), mTexY(0), mTexU(0), mTexV(0), mTimer(0)
{
    mFps = 0;
    mFrameNumber = 0;
    mPixFmt = -1;
    mArrY.clear();
    mArrU.clear();
    mArrV.clear();
    mVertices.clear();
    mTexcoords.clear();
    mIsAlloced = false;
    startTimer(50);
}
OpenGLWindow::~OpenGLWindow()
{
    emit mDecoder.stop();
    makeCurrent();
    if (mLogger) {
        mLogger->stopLogging();
        delete mLogger;
        mLogger = NULL;
    }
    if (mTexY) {
        if (mTexY->isBound())
            mTexY->release();
        if (mTexY->isCreated())
            mTexY->destroy();
        delete mTexY;
        mTexY = nullptr;
    }
    if (mTexU) {
        if (mTexU->isBound())
            mTexU->release();
        if (mTexU->isCreated())
            mTexU->destroy();
        delete mTexU;
        mTexU = nullptr;
    }
    if (mTexV) {
        if (mTexV->isBound())
            mTexV->release();
        if (mTexV->isCreated())
            mTexV->destroy();
        delete mTexV;
        mTexV = nullptr;
    }
    if (mTimer) {
        mTimer->stop();
        delete mTimer;
        mTimer = NULL;
    }
    doneCurrent();
}
void OpenGLWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    int ret = 0;
    VideoData data = mDecoder.getData(ret);
    if (0 != ret) {
        processVideoData(data.data, data.width, data.heigth, data.pixfmt);
    }
}
void OpenGLWindow::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    resize(width(), height());

    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);

    initTextures();
    initShaders();
    initData();
    mLogger = new QOpenGLDebugLogger(this->context());

    bool ret = mLogger->initialize(); // initializes in the current context, i.e. ctx
    if (ret) {
        connect(mLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
                SLOT(handleLoggedMessage(QOpenGLDebugMessage)));
        mLogger->startLogging();
    } else {
        qDebug() << "logger initialize failed";
    }

    //	connect(&work, SIGNAL(readyVideo(QByteArray,int,int,int)), this,
    //SLOT(processVideoData(QByteArray,int,int,int)));
    emit mDecoder.work();
}
void OpenGLWindow::handleLoggedMessage(QOpenGLDebugMessage message)
{
    qDebug() << message;
}
void OpenGLWindow::processVideoData(const QByteArray &data, int width, int height, int pixfmt)
{
    mArrY.clear();
    mArrU.clear();
    mArrV.clear();

    switch (pixfmt) {
    case AV_PIX_FMT_YUV444P:
        mArrY = data.left(width * height);
        mArrU = data.mid(width * height, width * height);
        mArrV = data.mid(width * height * 2, width * height);
        mPixFmt = 1;
        //			qDebug()<< "yuv444p";
        break;
    case AV_PIX_FMT_YUV420P:
        mArrY = data.left(width * height);
        mArrU = data.mid(width * height, width * height / 4);
        mArrV = data.mid(width * height * 5 / 4, width * height / 4);
        mPixFmt = 0;
        //			qDebug() << "yuv420p";
        break;
    default:
        qDebug() << "this format not support GPU speed up" << pixfmt;
        return;
        break;
    }

    resize(width, height);
    if (!mIsAlloced)
        allocTexture(mPixFmt);
    update();
}

void OpenGLWindow::paintGL()
{
    glDepthMask(true);
    glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mProgram.bind();
    draw();
    mProgram.release();

    calcFPS();
    paintFPS();
}
void OpenGLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    float ratio = (float)w / h;
    float left = -ratio;
    float right = ratio;
    float bottom = -1.0f;
    float top = 1.0f;
    float n = 1.0f;
    float f = 100.0f;
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.frustum(-1.0, 1.0, bottom, top, n, f);
}

void OpenGLWindow::initShaders()
{
    if (!mProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.vsh")) {
        qDebug() << __FILE__ << __FUNCTION__ << " add vertex shader file failed.";
        return;
    }
    if (!mProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.fsh")) {
        qDebug() << __FILE__ << __FUNCTION__ << " add fragment shader file failed.";
        return;
    }
    mProgram.bindAttributeLocation("qt_Vertex", 0);
    mProgram.bindAttributeLocation("texCoord", 1);
    mProgram.link();
    mProgram.bind();
}
void OpenGLWindow::initTextures()
{
    // yuv420p
    mTexY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTexY->setFormat(QOpenGLTexture::LuminanceFormat);
    //    mTexY->setFixedSamplePositions(false);
    mTexY->setMinificationFilter(QOpenGLTexture::Nearest);
    mTexY->setMagnificationFilter(QOpenGLTexture::Nearest);
    mTexY->setWrapMode(QOpenGLTexture::ClampToEdge);

    mTexU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTexU->setFormat(QOpenGLTexture::LuminanceFormat);
    //    mTexU->setFixedSamplePositions(false);
    mTexU->setMinificationFilter(QOpenGLTexture::Nearest);
    mTexU->setMagnificationFilter(QOpenGLTexture::Nearest);
    mTexU->setWrapMode(QOpenGLTexture::ClampToEdge);

    mTexV = new QOpenGLTexture(QOpenGLTexture::Target2D);
    mTexV->setFormat(QOpenGLTexture::LuminanceFormat);
    //    mTexV->setFixedSamplePositions(false);
    mTexV->setMinificationFilter(QOpenGLTexture::Nearest);
    mTexV->setMagnificationFilter(QOpenGLTexture::Nearest);
    mTexV->setWrapMode(QOpenGLTexture::ClampToEdge);
}
void OpenGLWindow::allocTexture(int fmt)
{
    int w = width();
    int h = height();
    if (fmt == 0) {
        // yuv420p
        mTexY->setSize(w, h);
        mTexY->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

        mTexU->setSize(w / 2, h / 2);
        mTexU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

        mTexV->setSize(w / 2, h / 2);
        mTexV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
    } else {
        // yuv444p
        mTexY->setSize(w, h);
        mTexY->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

        mTexU->setSize(w, h);
        mTexU->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);

        mTexV->setSize(w, h);
        mTexV->allocateStorage(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8);
    }
    mIsAlloced = true;
}
void OpenGLWindow::initData()
{
    mVertices << QVector3D(-1, 1, 0.0f) << QVector3D(1, 1, 0.0f) << QVector3D(1, -1, 0.0f)
              << QVector3D(-1, -1, 0.0f);
    mTexcoords << QVector2D(0, 0) << QVector2D(1, 0) << QVector2D(1, 1) << QVector2D(0, 1);

    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.001f), QVector3D(0.0f, 0.0f, -5.0f),
                       QVector3D(0.0f, 1.0f, 0.0f));
    mModelMatrix.setToIdentity();
}

void OpenGLWindow::draw()
{

    if (mArrY.size() <= 0) {
        qDebug() << "y array is empty";
        return;
    }
    if (mArrU.size() <= 0) {
        qDebug() << "u array is empty";
        return;
    }
    if (mArrV.size() <= 0) {
        qDebug() << "v array is empty";
        return;
    }
    if (!mIsAlloced) {
        return;
    }
    //	static int once = allocTexture();
    mModelMatHandle = mProgram.uniformLocation("u_modelMatrix");
    mViewMatHandle = mProgram.uniformLocation("u_viewMatrix");
    mProjectMatHandle = mProgram.uniformLocation("u_projectMatrix");
    mVerticesHandle = mProgram.attributeLocation("qt_Vertex");
    mTexCoordHandle = mProgram.attributeLocation("texCoord");

    //顶点
    mProgram.enableAttributeArray(mVerticesHandle);
    mProgram.setAttributeArray(mVerticesHandle, mVertices.constData());

    //纹理坐标
    mProgram.enableAttributeArray(mTexCoordHandle);
    mProgram.setAttributeArray(mTexCoordHandle, mTexcoords.constData());

    // MVP矩阵
    mProgram.setUniformValue(mModelMatHandle, mModelMatrix);
    mProgram.setUniformValue(mViewMatHandle, mViewMatrix);
    mProgram.setUniformValue(mProjectMatHandle, mProjectionMatrix);

    // pixFmt
    mProgram.setUniformValue("pixFmt", mPixFmt);
    //纹理
    if (mPixFmt == 1) {
        // yuv444p
        mTexY->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, mArrY.data());
        mTexU->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, mArrU.data());
        mTexV->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, mArrV.data());

        // Y
        glActiveTexture(GL_TEXTURE0);
        mTexY->bind();
        mProgram.setUniformValue("tex_y", 0);

        // U
        glActiveTexture(GL_TEXTURE1);
        mTexU->bind();
        mProgram.setUniformValue("tex_u", 1);

        // V
        glActiveTexture(GL_TEXTURE2);
        mTexV->bind();
        mProgram.setUniformValue("tex_v", 2);
    } else {
        // yuv420p

        // Y
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexY->textureId());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width(), height(), 0, GL_RED, GL_UNSIGNED_BYTE,
                     mArrY.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // U
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mTexU->textureId());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width() / 2, height() / 2, 0, GL_RED,
                     GL_UNSIGNED_BYTE, mArrU.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // V
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mTexV->textureId());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width() / 2, height() / 2, 0, GL_RED,
                     GL_UNSIGNED_BYTE, mArrV.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mProgram.setUniformValue("tex_y", 0);
        mProgram.setUniformValue("tex_u", 1);
        mProgram.setUniformValue("tex_v", 2);
    }
    glDrawArrays(GL_TRIANGLE_FAN, 0, mVertices.size());

    mProgram.disableAttributeArray(mVerticesHandle);
    mProgram.disableAttributeArray(mTexCoordHandle);
    mTexY->release();
    //	texU->release();
    //	texV->release();
}
void OpenGLWindow::calcFPS()
{
    static QTime time;
    static int once = [=]() {
        time.start();
        return 0;
    }();
    static int frame = 0;
    if (frame++ > 100) {
        qreal elasped = time.elapsed();
        updateFPS(frame / elasped * 1000);
        time.restart();
        frame = 0;
    }
}
void OpenGLWindow::updateFPS(qreal v)
{
    mFps = v;
}
void OpenGLWindow::paintFPS()
{
    mFrameNumber++;
    QPainter painter(this);
    painter.setPen(Qt::green);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.drawText(10, 10,
                     QString("FPS:%1 %2").arg(QString::number(mFps, 'f', 3)).arg(mFrameNumber));
}
