#include "widget.h"
#include <QDebug>
Widget::Widget(QWidget *parent) : QWidget(parent)
{
	connect(&mDecoder, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SLOT(processVideoData(QByteArray,int,int,int)));
	emit mDecoder.work();
}
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (mImage.isNull()) {
        return ;
    }
    QPainter paint(this);
    paint.drawImage(0,0, mImage.scaled(width(), height()));
}
void Widget::processVideoData(const QByteArray &data, int width, int height, int pixfmt)
{
	if (pixfmt !=AV_PIX_FMT_YUV444P ) {
		qDebug() << "pixfmt cannot support";
		return ;
	}
	mArrY = data.left(width * height);
	mArrU = data.mid(width * height, width * height);
	mArrV = data.mid(width * height * 2, width * height);

	QByteArray rgb;
	for (int i = 0; i < width * height; i++) {
		unsigned char y = mArrY[i];
		unsigned char u = mArrU[i];
		unsigned char v = mArrV[i];

		unsigned char r = y + 1.402 * (v - 128);
		unsigned char g = y - 0.34414 * (u - 128) - 0.71414 * (v - 128);
		unsigned char b = y + 1.772 * (u -128);
		rgb.append(r);
		rgb.append(g);
		rgb.append(b);
	}
	QImage image = QImage((uchar *)rgb.data(), width, height, QImage::Format_RGB888);
	mImage = image.copy();
	update();
}
