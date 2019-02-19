#pragma once

#include "decodertmp.h"
#include <QImage>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QWidget>
class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
signals:

public slots:
    void processVideoData(const QByteArray &data, int width, int height, int pixfmt);

private:
    QImage mImage;
    QByteArray mArrY, mArrU, mArrV;

    DecoderController mDecoder;
};
