#include <QDebug>
#include <QFile>
#include <stdlib.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
}
#include "errcode.h"

#include "convert.h"
struct _Args
{
    const char *inputFileName;
    const char *outputFileName;
    int width;
    int height;
    int bitrate;
    int frameToEncode;
} Args;
QDebug operator<<(QDebug dbg, const _Args &args)
{
    QDebugStateSaver saver(dbg);
    dbg.noquote() << args.inputFileName << args.outputFileName << args.width << args.height << args.bitrate << args.frameToEncode;
    return dbg;
}

static AVCodec *codec = nullptr;
static AVCodecContext *codecCtx = nullptr;
static AVFrame *frame = nullptr;
static AVPacket packet;
static QFile inFile;
static QFile outFile;

void parseArgs(int argc, char *argv[])
{
    if (argc < 1 || argv == nullptr)
    {
        qDebug() << "parse args failed";
        return;
    }
    Args.inputFileName = argv[1];
    Args.outputFileName = argv[2];
    Args.width = atoi(argv[3]);
    Args.height = atoi(argv[4]);
    Args.bitrate = atoi(argv[5]);
    Args.frameToEncode = atoi(argv[6]);
}

static int read_yuv_data(int color)
{
    // color == 0 : Y value
    // color == 1 : U value
    // color == 2 : V value
    int color_height = color == 0 ? Args.height : Args.height / 2;
    int color_width = color == 0 ? Args.width : Args.width / 2;
    int color_size = color_height * color_width;
    int color_stride = frame->linesize[color];
    if (color_width == color_stride)
    {
        inFile.read((char *)frame->data[color], color_size);
    }
    else
    {
        for (int i = 0; i < color_height; i++)
        {
            inFile.read((char *)frame->data[color] + color_stride * i, color_width);
        }
    }
    return color_size;
}
static int read_gray_data()
{
    int color_height = Args.height;
    int color_width = Args.width;
    int color_size = color_height * color_width;
    inFile.read((char *)frame->data[0], color_size);

    memset((char *)frame->data[1], 0x80, color_size / 4);
    memset((char *)frame->data[2], 0x80, color_size / 4);
    return color_size;
}
static int read_gray_data_by_file(int index)
{
    QFile file;
    QString name;
    if (index < 512 / 2)
    {
        name = QString("images/B_%1.pgm").arg(index, 3, 10, QChar('0'));
    }
    else
    {
        name = QString("images/C_%1.pgm").arg(index - 256, 3, 10, QChar('0'));
    }
    file.setFileName(name);
    if (!file.open(QFile::ReadWrite))
    {
        qDebug() << "open file failed" << file.fileName();
        return -1;
    }
    int color_height = Args.height;
    int color_width = Args.width;
    int color_size = color_height * color_width;
    file.read((char *)frame->data[0], color_size);
    memset((char *)frame->data[1], 0x80, color_size / 4);
    memset((char *)frame->data[2], 0x80, color_size / 4);

    return color_size;
}
static int read_rgb_data()
{
    int color_height = Args.height;
    int color_width = Args.width;
    int color_size = color_height * color_width;
    inFile.read((char *)frame->data[0], color_size);
    memcpy((char *)frame->data[1], (char *)frame->data[0], color_size);
    memcpy((char *)frame->data[2], (char *)frame->data[0], color_size);
    return color_size;
}

int main(int argc, char *argv[])
{

    parseArgs(argc, argv);
    qDebug() << Args;

    inFile.setFileName(Args.inputFileName);
    if (!inFile.open(QFile::ReadOnly))
    {
        qDebug() << __FILE__ << __LINE__ << "open file failed";
        return OPEN_FILE_FAILED;
    }
    //	QTextStream in(&inFile);
    //	int sum, frames, w, h;
    //	in>> sum >> frames>> w >> h;
    //	qDebug() << "read filem head:" << sum << frames << w << h;
    //	inFile.seek(sizeof(int) * 4);
    outFile.setFileName(Args.outputFileName);
    if (!outFile.open(QFile::ReadWrite))
    {
        qDebug() << __FILE__ << __LINE__ << "open file failed";
        return OPEN_FILE_FAILED;
    }

    av_register_all();

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (codec == NULL)
    {
        return FF_FIND_CODEC_FAILED;
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (codecCtx == nullptr)
    {
        return FF_ALLOC_CONDECCTX_FAILED;
    }
    // set encode params
    codecCtx->width = Args.width;
    codecCtx->height = Args.height;
    codecCtx->bit_rate = Args.bitrate;
    // 25 frames 1 second
    AVRational r = { 1, 25 };
    codecCtx->time_base = r;
    codecCtx->gop_size = 12;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //	av_opt_set(codecCtx->priv_data,"preset", "slow", 0);
    int open_ret = avcodec_open2(codecCtx, codec, NULL);
    if (open_ret < 0)
    {
        qDebug() << "open codec fail" << open_ret;
        return FF_OPEN_CODEC_FAILED;
    }
    frame = av_frame_alloc();
    if (!frame)
    {
        return FF_ALLOC_FRAME_FAILED;
    }
    frame->width = codecCtx->width;
    frame->height = codecCtx->height;
    frame->format = codecCtx->pix_fmt;
    if (av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 12) < 0)
    {
        return FF_AV_IMAGE_ALLOC_FAILED;
    }
    int got_packet = 0;

    for (int i = 0; i <= Args.frameToEncode; i++)
    {
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        read_gray_data_by_file(i);
        //		read_yuv_data(0);
        //		read_yuv_data(1);
        //		read_yuv_data(2);

        frame->pts = i;
        if (avcodec_encode_video2(codecCtx, &packet, frame, &got_packet) < 0)
        {
            return FF_ENCODE_FAILED;
        }
        if (got_packet)
        {
            qDebug() << QString("Write packet of frame: %1, size = %2").arg(i).arg(packet.size);
            outFile.write((const char *)packet.data, packet.size);
            av_packet_unref(&packet);
        }
    }
    for (got_packet = 1; got_packet;)
    {
        if (avcodec_encode_video2(codecCtx, &packet, NULL, &got_packet) < 0)
        {
            return FF_ENCODE_FAILED;
        }
        if (got_packet)
        {
            qDebug() << QString("Write cached packet, size = %1").arg(packet.size);
            outFile.write((const char *)packet.data, packet.size);
            av_packet_unref(&packet);
        }
    }
    avcodec_close(codecCtx);
    av_free(codecCtx);

    av_freep(&frame->data[0]);
    av_frame_free(&frame);

    inFile.close();
    outFile.close();
    return 0;
}
