#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include "libyuv.h"
#include "Buffer.h"
#include "BMP.h"
#include <fmt/format.h>
using namespace BMP;
using namespace std;

struct Args
{
    wstring inputFileName;
    uint32_t width;
    uint32_t height;
    uint32_t bitrate;
} gArgs;
wostream &operator<<(wostream &wout, const Args &args)
{
    wout << args.inputFileName << " " << args.width << " " << args.height << " " << args.bitrate
         << endl;
    return wout;
}

static const wstring resPath = ResPath;
void parseArgs(int argc, wchar_t **argv)
{
    if (argc <= 4 || argv == nullptr) {
        cerr << "parse args failed, use default ";
        gArgs.inputFileName = resPath + L"flower_cif.yuv";
        gArgs.width = 352;
        gArgs.height = 288;
        gArgs.bitrate = 30412000;
        return;
    }
    gArgs.inputFileName = resPath + argv[1];
    gArgs.width = stoi(argv[2]);
    gArgs.height = stoi(argv[3]);
    gArgs.bitrate = stoi(argv[4]);
}

void toBMP(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = gArgs.width * gArgs.height * 1.5;
    int frameCount = size / yuvFrameSize;
    int rgbFrameSize = gArgs.width * gArgs.height * 3;

    int yStride = gArgs.width;
    int uStride = gArgs.width / 2;
    int vStride = gArgs.width / 2;
    int rgbStride = gArgs.width * 3;
    Buffer<uint8_t> rgbBuf;
    rgbBuf.reserve(rgbFrameSize);
    //gen bmp for all frame

    for (int f = 0; f < frameCount; ++f) {
        rgbBuf.fill(0);
        int yuvFrameOffset = f * yuvFrameSize;
        char *pY = buffer.data() + yuvFrameOffset;
        char *pU = pY + gArgs.width * gArgs.height;
        char *pV = pU + gArgs.width * gArgs.height / 4;

        // for (int i = 0; i < frameCount; ++i) {
        int ret = libyuv::I420ToRGB24((uint8_t *)pY, yStride, (uint8_t *)pU, uStride, (uint8_t *)pV,
                                      vStride, rgbBuf.data(), rgbStride, gArgs.width, gArgs.height);

        if (ret) {
            wcout << "I420ToRGB24 failed " << ret << endl;
        }
        auto name = fmt::format(L"flower_{1}.bmp", f);
        saveBMP(rgbBuf.data(), gArgs.width, gArgs.height, resPath + name);
    }
}
void toI422(const CharBuffer &buffer)
{


}
void toI444(const CharBuffer &buffer)
{


}
void toI411(const CharBuffer &buffer)
{


}
void toNV12(const CharBuffer &buffer)
{


}
void toNV21(const CharBuffer &buffer)
{


}
void toYUY2(const CharBuffer &buffer)
{


}
void toUYVY(const CharBuffer &buffer)
{


}
int wmain(int argc, wchar_t *argv[])
{
    wcout << resPath << endl;
    wcout << sizeof(BMPFileHeader) << " " << sizeof(BMPInfoHeader) << endl;
    parseArgs(argc, argv);
    wcout << gArgs;

    ifstream in(gArgs.inputFileName, ios::in | ios::binary | ios::ate);
    if (!in.is_open()) {
        wcout << "open file failed " << gArgs.inputFileName << endl;
        return 0;
    }
    uint32_t size = in.tellg();
    CharBuffer buffer;
    buffer.reserve(size);
    buffer.fill(0);
    in.seekg(0, ios::beg);
    in.read(buffer.data(), size);

    in.close();

//    toBMP(buffer);
    toI422(buffer);
    return 0;
}
