#pragma once
#include "shared/Args.hpp"
#include <fstream>

extern "C" {
#include <cstdint>
#include <cstdio>
#include "x264.h"
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

    void read_yuv420p_to_pic(int color, int w, int h);
private:
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t *encoder = nullptr;

    bool inited = false;

    std::ifstream in;
    std::ofstream out;
};