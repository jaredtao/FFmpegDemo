#ifndef VIDEODEC_H
#define VIDEODEC_H

#include <QObject>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/fifo.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
}

#include <QImage>
#include <QScopedPointer>
class VideoDec : public QObject
{
    Q_OBJECT
public:
    explicit VideoDec(QObject *parent = 0);

    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket packet;
    AVFrame *pFrame, *pFrameRGB;
signals:
    void sendImage(QImage);
public slots:
    void setFilename(QString filename);
    void setMaxBufsize(int size);

    void init();
    void play();

private:
    char m_filename[256];
    int m_bufsize;
    int m_maxBufsize;
    int videoindex;
};

#endif // VIDEODEC_H
