#include "videodec.h"
#include <QDebug>
#include <QThread>
VideoDec::VideoDec(QObject *parent) : QObject(parent)
{
    pFormatCtx = NULL;
    pCodecCtx = NULL;
    pCodec = NULL;
    pFrame = NULL;
    pFrameRGB = NULL;
}
void VideoDec::setFilename(QString filename)
{
    QByteArray array = filename.toLatin1();
    if ((unsigned int)array.length() >= (unsigned int)sizeof(m_filename) / sizeof(m_filename[0]))
    {
        qDebug() << "filename to Latin1 is too long";
        return;
    }
    memset(m_filename, 0, sizeof(m_filename) / sizeof(m_filename[0]));
    memcpy(m_filename, array.data(), array.length());
}
void VideoDec::setMaxBufsize(int size)
{
    m_maxBufsize = size;
}
void VideoDec::init()
{
    m_bufsize = 0;
    avcodec_register_all();
    av_register_all();

    pFormatCtx = avformat_alloc_context();
    pCodecCtx = avcodec_alloc_context3(pCodec);

    int ret = avformat_open_input(&pFormatCtx, m_filename, NULL, NULL);
    if (ret != 0)
    {
        qDebug() << "open failed" << ret;
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        qDebug() << "Find failed";
        return;
    }
    videoindex = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {

        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }
    }
    if (videoindex == -1)
    {
        qDebug() << "Don't find video stream";
        return;
    }
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        qDebug() << "codec not find";
        return;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        qDebug() << "Can't open codec";
        return;
    }
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    play();
}
void VideoDec::play()
{
    int got_picture;
    av_new_packet(&packet, pCodecCtx->width * pCodecCtx->height);
    uint8_t *out_buffer;
    out_buffer = new uint8_t[avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height)];
    avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
    avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height);
    while (1)
    {
        if (m_bufsize > m_maxBufsize)
        {
            qDebug() << "bufsize bigger than maxBufsize";
            break;
        }
        int ret = av_read_frame(pFormatCtx, &packet);
        if (ret < 0)
        {
            qDebug() << "read frame ret" << ret;
            break;
        }
        if (packet.stream_index == videoindex)
        {
            int rec = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
            if (rec > 0)
            {
                SwsContext *convertCtx = sws_getContext(
                    pCodecCtx->width,
                    pCodecCtx->height,
                    pCodecCtx->pix_fmt,
                    pCodecCtx->width,
                    pCodecCtx->height,
                    AV_PIX_FMT_RGB32,
                    SWS_BICUBIC,
                    NULL,
                    NULL,
                    NULL);
                sws_scale(convertCtx, (const uint8_t *const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                QImage img((uchar *)pFrameRGB->data[0], pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
                // NOTE here send out image
                emit sendImage(img.copy());
                qDebug() << "send one frame image" << m_bufsize;
                m_bufsize++;
                sws_freeContext(convertCtx);
            }
            av_free_packet(&packet);
        }
    }
}
