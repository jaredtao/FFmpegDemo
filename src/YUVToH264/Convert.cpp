#include "Convert.h"
#include "shared/FileIO.hpp"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::wcerr;
using std::wcout;

Convert::Convert() { }

Convert::~Convert()
{
    unInit();
}

void Convert::convert(const Args &args)
{
    using namespace std;
    int ret = 0;
    if (false == init(args)) {
        return;
    }
    in.open(args.inputFileName, ios::in | ios::binary | ios::ate);
    if (!in) {
        wcerr << "open file failed " << args.inputFileName << endl;
        return;
    }
    out.open(args.outputFileName, ios::out | ios::binary);
    if (!out) {
        wcerr << "open file failed " << args.outputFileName << endl;
        return;
    }
    in.seekg(0, ios::end);
    int size = (int)in.tellg();
    in.seekg(0, ios::beg);
    int yuvFrameSize = (int)(args.width * args.height * 1.5);
    int frameCount = size / yuvFrameSize;
    for (int i = 0; i < frameCount; ++i) {
        av_init_packet(&packet);
        packet.data = nullptr;
        packet.size = 0;

        read_yuv420p_to_frame(0, args.width, args.height);
        read_yuv420p_to_frame(1, args.width, args.height);
        read_yuv420p_to_frame(2, args.width, args.height);

        frame->pts = i;
        ret = avcodec_send_frame(codecCtx, frame);

        if (ret < 0) {
            cerr << "avcodec_send_frame failed " << ret << endl;
            break;
        }
        ret = avcodec_receive_packet(codecCtx, &packet);
        if (ret < 0) {
            cerr << "avcodec_receive_packet failed " << ret << endl;
            continue;
        }
        if (packet.size <= 0) {
            continue;
        }
        cout << "write packet of frame " << i << ", size = " << packet.size << endl;
        out.write((const char *)packet.data, packet.size);
        av_packet_unref(&packet);
    }

    do {
        ret = avcodec_receive_packet(codecCtx, &packet);
        if (ret < 0) {
            cerr << "avcodec_receive_packet failed" << endl;
            break;
        }

        cout << "write cache packet, size = " << packet.size << endl;
        out.write((const char *)packet.data, packet.size);
        av_packet_unref(&packet);
    } while (1);

    in.close();
    out.close();
}

bool Convert::init(const Args &args)
{
    if (inited) {
        return true;
    }
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (nullptr == codec) {
        cerr << "avcodec_find_encoder failed" << endl;
        return false;
    }
    codecCtx = avcodec_alloc_context3(codec);
    if (nullptr == codecCtx) {
        cerr << "avcodec_alloc_context3 failed" << endl;
        return false;
    }
    codecCtx->width = args.width;
    codecCtx->height = args.height;
    codecCtx->bit_rate = args.bitrate;
    // 25 frames 1 second
    AVRational r = { 1, 25 };

    codecCtx->time_base = r;
    codecCtx->gop_size = 12;
    codecCtx->max_b_frames = 1;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        cerr << "avcodec_open2 failed" << endl;
        return false;
    }
    frame = av_frame_alloc();
    if (!frame) {
        cerr << "av_frame_alloc failed" << endl;
        return false;
    }
    frame->width = args.width;
    frame->height = args.height;
    frame->format = codecCtx->pix_fmt;
    if (av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 32) < 0) {
        cerr << "av_image_alloc failed" << endl;
        return false;
    }

    inited = true;
    return true;
}

void Convert::unInit()
{
    if (false == inited) {
        return;
    }
    avcodec_close(codecCtx);
    av_free(codecCtx);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
}

void Convert::read_yuv420p_to_frame(int color, int w, int h)
{
    // color == 0 : Y value
    // color == 1 : U value
    // color == 2 : V value
    int color_height = color == 0 ? h : h / 2;
    int color_width = color == 0 ? w : w / 2;
    int color_size = color_height * color_width;
    int color_stride = frame->linesize[color];
    if (color_width == color_stride) {
        in.read((char *)frame->data[color], color_size);
    } else {
        for (int i = 0; i < color_height; ++i) {
            in.read((char *)frame->data[color] + color_stride * i, color_width);
        }
    }
}
