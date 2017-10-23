#ifndef WIDGET_H
#define WIDGET_H

#include <QTimer>
#include <QTime>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include "decodertmp.h"
class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event) override;
signals:

public slots:
    void processVideoData(const QByteArray &data, int width, int height, int pixfmt);
private:
    QImage m_image;
    QByteArray arrY, arrU, arrV;

    Work work;

};

#endif // WIDGET_H
