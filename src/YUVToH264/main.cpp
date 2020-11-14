#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <fmt/format.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

using std::string;
using std::wstring;
using std::wostream;
using std::wostream;
using std::wstring;
using std::stoi;

using std::wcout;
using std::wcerr;
using std::endl;

struct Args
{
    wstring inputFileName;
    wstring outputFileName;
    int width;
    int height;
    int bitrate;
    int frameToEncode;
} gArgs;

wostream &operator<<(wostream &wout, const Args &args)
{
    wout << args.inputFileName << args.outputFileName << args.width << args.height
                  << args.bitrate << args.frameToEncode;
    return wout;
}

static AVCodec *codec = nullptr;
static AVCodecContext *codecCtx = nullptr;
static AVFrame *frame = nullptr;
static AVPacket packet;

static const wstring resPath = ResPath;

void parseArgs(int argc, wchar_t *argv[])
{
    if (argc <= 6 || argv == nullptr) {
        wcerr << L"parse args failed, use default" << endl;
        gArgs.inputFileName = resPath + L"flower_cif.yuv";
        gArgs.outputFileName = resPath + L"flower_cif.h264";
        gArgs.width = 352;
        gArgs.height = 288;
        gArgs.bitrate = 30412000;
        gArgs.frameToEncode = 600;
        return;
    }
    gArgs.inputFileName = argv[1];
    gArgs.outputFileName = argv[2];
    gArgs.width = stoi(argv[3]);
    gArgs.height = stoi(argv[4]);
    gArgs.bitrate = stoi(argv[5]);
    gArgs.frameToEncode = stoi(argv[6]);
}

static int read_yuv_data(int color)
{
    // color == 0 : Y value
    // color == 1 : U value
    // color == 2 : V value
    int color_height = color == 0 ? gArgs.height : gArgs.height / 2;
    int color_width = color == 0 ? gArgs.width : gArgs.width / 2;
    int color_size = color_height * color_width;
    int color_stride = frame->linesize[color];
    if (color_width == color_stride) {
        inFile.read((char *)frame->data[color], color_size);
    } else {
        for (int i = 0; i < color_height; i++) {
            inFile.read((char *)frame->data[color] + color_stride * i, color_width);
        }
    }
    return color_size;
}


int main(int argc, char *argv[])
{
    using namespace YUV;
    parseArgs(argc, argv);
    qDebug() << gArgs;

    inFile.setFileName(gArgs.inputFileName);
    if (!inFile.open(QFile::ReadOnly)) {
        qDebug() << "open file failed" << inFile.fileName()<< inFile.errorString() ;
        return OPEN_FILE_FAILED;
    }
    outFile.setFileName(gArgs.outputFileName);
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "open file failed" << outFile.fileName()<< outFile.errorString() ;
        return OPEN_FILE_FAILED;
    }

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (codec == NULL) {
        return FF_FIND_CODEC_FAILED;
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (codecCtx == nullptr) {
        return FF_ALLOC_CONDECCTX_FAILED;
    }
    // set encode params
    codecCtx->width = gArgs.width;
    codecCtx->height = gArgs.height;
    codecCtx->bit_rate = gArgs.bitrate;
    // 25 frames 1 second
    AVRational r = { 1, 25 };
    codecCtx->time_base = r;
    codecCtx->gop_size = 12;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //	av_opt_set(codecCtx->priv_data,"preset", "slow", 0);
    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        return FF_OPEN_CODEC_FAILED;
    }
    frame = av_frame_alloc();
    if (!frame) {
        return FF_ALLOC_FRAME_FAILED;
    }
    frame->width = codecCtx->width;
    frame->height = codecCtx->height;
    frame->format = codecCtx->pix_fmt;
    if (av_image_alloc(frame->data, frame->linesize, frame->width, frame->height,
                       (AVPixelFormat)frame->format, 32)
        < 0) {
        return FF_AV_IMAGE_ALLOC_FAILED;
    }
    int got_packet = 0;
    for (int i = 0; i < gArgs.frameToEncode; i++) {
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        read_yuv_data(0);
        read_yuv_data(1);
        read_yuv_data(2);

        frame->pts = i;
        if (avcodec_encode_video2(codecCtx, &packet, frame, &got_packet) < 0) {
            return FF_ENCODE_FAILED;
        }
        if (got_packet) {
            qDebug() << QString("Write packet of frame: %1, size = %2").arg(i).arg(packet.size);
            outFile.write((const char *)packet.data, packet.size);
            av_packet_unref(&packet);
        }
    }
    for (got_packet = 1; got_packet;) {
        if (avcodec_encode_video2(codecCtx, &packet, NULL, &got_packet) < 0) {
            return FF_ENCODE_FAILED;
        }
        if (got_packet) {
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
