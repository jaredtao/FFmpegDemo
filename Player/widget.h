#pragma once

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
    QImage mImage;
    QByteArray mArrY, mArrU, mArrV;

    DecoderController mDecoder;

};

