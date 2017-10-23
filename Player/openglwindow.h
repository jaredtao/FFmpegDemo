#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLDebugLogger>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include "decodertmp.h"

class OpenGLWindow :public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWidget *parent = 0);
    ~OpenGLWindow();
protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;
	void timerEvent(QTimerEvent *event) override;
	void initShaders();
	void initTextures();

	void initData();
	void draw();
	void calcFPS();
	void updateFPS(qreal);
	void paintFPS();
	void allocTexture(int fmt);


public slots:
	void processVideoData(const QByteArray &data, int width, int height, int pixfmt);
	void handleLoggedMessage(QOpenGLDebugMessage message);
private:
	QOpenGLDebugLogger *logger;
	GLint m_pixFmt;
	QByteArray arrY, arrU, arrV;

	QOpenGLShaderProgram program;
	QOpenGLTexture *texY, *texU, *texV;
	QVector<QVector3D> vertices;
	QVector<QVector2D> texcoords;
	int mModelMatHandle, mViewMatHandle, mProjectMatHandle;
	int mVerticesHandle;
	int mTexCoordHandle;

	QMatrix4x4 mModelMatrix;
	QMatrix4x4 mViewMatrix;
	QMatrix4x4 mProjectionMatrix;

	QTimer *timer;
	qreal fps;
	int frameNumber;
	Work work;
	bool isAlloced;
};

#endif // WINDOW_H
