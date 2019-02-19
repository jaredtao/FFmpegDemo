#include "video.h"
#include <QDebug>

static QFile *inputVideoFile = NULL;

static AVFrame *alloc_picture(enum AVPixelFormat fmt, int w, int h)
{
    AVFrame *frame;
    frame = av_frame_alloc();
    if (!frame)
    {
        qDebug() << "frame alloc failed";
        return NULL;
    }
    frame->format = fmt;
    frame->width = w;
    frame->height = h;
    int ret = av_frame_get_buffer(frame, 32);
    if (ret < 0)
    {
        qDebug() << "get buffer failed";
        outError(ret);
        return NULL;
    }
    return frame;
}

int OpenVideo(AVFormatContext *fmtCtx, AVCodec *codec, OutputStream *st, FileIO &io)
{
    AVCodecContext *c = st->stream->codec;
    int ret = avcodec_open2(c, codec, NULL);
    if (ret < 0)
    {
        qDebug() << "open video encode failed";
        outError(ret);
        return ret;
    }

    st->videoFrame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!st->videoFrame)
    {
        qDebug() << "alloc video frame failed";
        return -2;
    }
    io.videoFile.setFileName(QString(io.inputVideoFileName));
    if (!io.videoFile.open(QFile::ReadOnly))
    {
        qDebug() << "open audio file failed" << io.videoFile.errorString();
        return -3;
    }
    inputVideoFile = &io.videoFile;
    return 0;
}
static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, *time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    //	log_packet(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

static void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
    int x, y, i, ret;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    ret = av_frame_make_writable(pict);
    if (ret < 0)
    {
        exit(1);
    }

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
    {
        //        ret = fread_s(&pict->data[0][y * pict->linesize[0]], pict->linesize[0], 1, width, g_inputYUVFile);
        ret = inputVideoFile->read((char *)&pict->data[0][y * pict->linesize[0]], width);
        if (ret != width)
        {
            printf("Error: Read Y data error.\n");
            exit(1);
        }
    }

    /* U */
    for (y = 0; y < height / 2; y++)
    {
        ret = inputVideoFile->read((char *)&pict->data[1][y * pict->linesize[1]], width / 2);
        //        ret = fread_s(&pict->data[1][y * pict->linesize[1]], pict->linesize[1], 1, width / 2, g_inputYUVFile);
        if (ret != width / 2)
        {
            printf("Error: Read U data error.\n");
            exit(1);
        }
    }

    /* V */
    for (y = 0; y < height / 2; y++)
    {
        ret = inputVideoFile->read((char *)&pict->data[2][y * pict->linesize[2]], width / 2);
        //        ret = fread_s(&pict->data[2][y * pict->linesize[2]], pict->linesize[2], 1, width / 2, g_inputYUVFile);
        if (ret != width / 2)
        {
            printf("Error: Read V data error.\n");
            exit(1);
        }
    }
}

static AVFrame *get_video_frame(OutputStream *ost)
{
    AVCodecContext *c = ost->stream->codec;

    /* check if we want to generate more frames */
    {
        AVRational r = { 1, 1 };
        if (av_compare_ts(ost->nextPts, ost->stream->codec->time_base, STREAM_DURATION, r) >= 0)
        {
            return NULL;
        }
    }

    fill_yuv_image(ost->videoFrame, ost->nextPts, c->width, c->height);

    ost->videoFrame->pts = ost->nextPts++;

    return ost->videoFrame;
}

int Write_video_frame(AVFormatContext *oc, OutputStream *ost)
{
    int ret;
    AVCodecContext *c;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    c = ost->stream->codec;

    frame = get_video_frame(ost);

    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        qDebug() << "Error encoding video frame: %d\n";
        outError(ret);
        exit(1);
    }

    if (got_packet)
    {
        ret = write_frame(oc, &c->time_base, ost->stream, &pkt);
    }
    else
    {
        ret = 0;
    }

    if (ret < 0)
    {
        qDebug() << "Error while writing video frame: %d\n";
        outError(ret);
        exit(1);
    }

    return (frame || got_packet) ? 0 : 1;
}
