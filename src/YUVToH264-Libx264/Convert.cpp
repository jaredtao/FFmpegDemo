#include "Convert.h"
#include "shared/FileIO.hpp"
#include <filesystem>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

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
    filesystem::path inPath(args.inputFileName);
    in.open(inPath.string(), ios::in | ios::binary | ios::ate);
    if (!in) {
        cerr << "open file failed " << args.inputFileName << endl;
        return;
    }
    filesystem::path outPath(args.outputFileName);
    out.open(outPath.string(), ios::out | ios::binary);
    if (!out) {
        cerr << "open file failed " << args.outputFileName << endl;
        return;
    }
    in.seekg(0, ios::end);
    int size = (int)in.tellg();
    in.seekg(0, ios::beg);
    int yuvFrameSize = (int)(args.width * args.height * 1.5);
    int frameCount = size / yuvFrameSize;

    int i_frameSize = 0;
    x264_nal_t *nal;
    int i_nal;
    for (int i = 0; i < frameCount; ++i) {

        read_yuv420p_to_pic(0, args.width, args.height);
        read_yuv420p_to_pic(1, args.width, args.height);
        read_yuv420p_to_pic(2, args.width, args.height);

        pic.i_pts = i;

        i_frameSize = x264_encoder_encode(encoder, &nal, &i_nal, &pic, &pic_out);
        if (i_frameSize < 0) {
            break;
        } else if (i_frameSize > 0) {
            out.write((const char *)nal->p_payload, i_frameSize);
        }
    } 

    while (x264_encoder_delayed_frames(encoder)) 
    {
        i_frameSize = x264_encoder_encode(encoder, &nal, &i_nal, nullptr, &pic_out);
        if (i_frameSize < 0) {
            break;
        } else if (i_frameSize > 0) {
            out.write((const char *)nal->p_payload, i_frameSize);
        }
    }

    in.close();
    out.close();
}

bool Convert::init(const Args &args)
{
    if (inited) {
        return true;
    }
    if (x264_param_default_preset(&param, "medium", nullptr) < 0) {
        cerr << "x264_param_default_preset failed" << endl;
        goto fail;
    }
    param.i_bitdepth = 8;
    param.i_csp = X264_CSP_I420;
    param.i_width = args.width;
    param.i_height = args.height;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    if (x264_param_apply_profile(&param, "high") < 0) {
        cerr << "x264_param_apply_profile failed" << endl;
        goto fail;
    }
    if (x264_picture_alloc(&pic, param.i_csp, param.i_width, param.i_height) < 0) {
        cerr << "x264_picture_alloc failed" << endl;
        goto fail;
    }
    encoder = x264_encoder_open(&param);
    if (!encoder) {
        cerr << "x264_encoder_open failed" << endl;
        goto fail2;
    }

    inited = true;
    return true;
fail2:
    x264_picture_clean(&pic);
fail:
    return false;
}

void Convert::unInit()
{
    if (false == inited) {
        return;
    }
    x264_encoder_close(encoder);
    x264_picture_clean(&pic);
}
void Convert::read_yuv420p_to_pic(int color, int w, int h)
{
    // color == 0 : Y value
    // color == 1 : U value
    // color == 2 : V value
    int color_height = color == 0 ? h : h / 2;
    int color_width = color == 0 ? w : w / 2;
    int color_size = color_height * color_width;
    in.read((char *)pic.img.plane[color], color_size);
}
// void Convert::read_yuv420p_to_frame(int color, int w, int h)
// {
//     // color == 0 : Y value
//     // color == 1 : U value
//     // color == 2 : V value
//     int color_height = color == 0 ? h : h / 2;
//     int color_width = color == 0 ? w : w / 2;
//     int color_size = color_height * color_width;
//     int color_stride = frame->linesize[color];
//     if (color_width == color_stride) {
//         in.read((char *)frame->data[color], color_size);
//     } else {
//         for (int i = 0; i < color_height; ++i) {
//             in.read((char *)frame->data[color] + color_stride * i, color_width);
//         }
//     }
// }
