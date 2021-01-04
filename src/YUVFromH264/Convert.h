#pragma once
#include "shared/Args.hpp"
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}
class Convert
{
public:
    Convert();
    ~Convert();
    void convert(const Args &args);

private:
    bool init(const Args &args);
    void unInit();

    void read_yuv420p_to_frame(int color, int w, int h);
private:
    AVCodec *codec = nullptr;
    AVCodecContext *codecCtx = nullptr;
    AVFrame *frame = nullptr;
    AVPacket packet;
    bool inited = false;

    std::ifstream in;
    std::ofstream out;
};