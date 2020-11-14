//#include <fmt/format.h>

#include "BMP.h"
#include "Buffer.h"
#include "FileIO.h"
#include "libyuv.h"
#include <iostream>
#include <string>

using namespace BMP;

using std::endl;
using std::wcerr;
using std::wcout;

using std::wostream;

using std::stoi;

using std::string;
using std::wstring;

struct Args
{
    wstring inputFileName;
    wstring outputBaseFileName;
    uint32_t width;
    uint32_t height;
    uint32_t bitrate;
} gArgs;
wostream &operator<<(wostream &wout, const Args &args)
{
    wout << args.inputFileName << " " << args.width << " " << args.height << " " << args.bitrate << endl;
    return wout;
}

static const wstring resPath = ResPath;
void parseArgs(int argc, wchar_t **argv)
{
    if (argc <= 4 || argv == nullptr) {
        wcerr << L"parse args failed, use default " << endl;
        gArgs.inputFileName = resPath + L"flower_cif.yuv";
        gArgs.outputBaseFileName = L"flower_cif";
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
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int rgbFrameSize = gArgs.width * gArgs.height * 3;

    int yStride = gArgs.width;
    int uStride = gArgs.width / 2;
    int vStride = gArgs.width / 2;
    int rgbStride = gArgs.width * 3;
    U8Buffer rgbBuf;
    rgbBuf.reserve(rgbFrameSize);
    // gen bmp for all frame

    for (int i = 0; i < frameCount; ++i) {
        rgbBuf.fill(0);
        int yuvFrameOffset = i * yuvFrameSize;
        char *pY = buffer.data() + yuvFrameOffset;
        char *pU = pY + gArgs.width * gArgs.height;
        char *pV = pU + gArgs.width * gArgs.height / 4;

        // for (int i = 0; i < frameCount; ++i) {
        int ret = libyuv::I420ToRGB24((uint8_t *)pY, yStride, (uint8_t *)pU, uStride, (uint8_t *)pV, vStride, rgbBuf.data(), rgbStride, gArgs.width,
                                      gArgs.height);

        if (ret) {
            wcout << "I420ToRGB24 failed " << ret << endl;
        }
        wchar_t fileName[1024];
        swprintf(fileName, 1024, L"%s_%d.bmp", gArgs.outputBaseFileName.c_str(), i);

        // auto fileName = fmt::format(L"flower_{1}.bmp", i);
        saveBMP(rgbBuf.data(), gArgs.width, gArgs.height, fileName);
    }
}
void toI444(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = gArgs.width * gArgs.height * 3;

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;
    int dstYStride = gArgs.width;
    int dstUStride = gArgs.width;
    int dstVStride = gArgs.width;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dstY = dstBuf.data() + i * dstFrameSize;
        uint8_t *dstU = dstY + gArgs.width * gArgs.height;
        uint8_t *dstV = dstU + gArgs.width * gArgs.height;
        libyuv::I420ToI444(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dstY, dstYStride, dstU, dstUStride, dstV, dstVStride, gArgs.width,
                           gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.i444", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}
void toI422(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = gArgs.width * gArgs.height * 2;

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;
    int dstYStride = gArgs.width;
    int dstUStride = gArgs.width / 2;
    int dstVStride = gArgs.width / 2;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dstY = dstBuf.data() + i * dstFrameSize;
        uint8_t *dstU = dstY + gArgs.width * gArgs.height;
        uint8_t *dstV = dstU + gArgs.width * gArgs.height / 2;
        libyuv::I420ToI422(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dstY, dstYStride, dstU, dstUStride, dstV, dstVStride, gArgs.width,
                           gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.i422", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}
void toYUY2(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = gArgs.width * gArgs.height * 2;

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;

    int dstStride = gArgs.width * 2;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dst = dstBuf.data() + i * dstFrameSize;
        libyuv::I420ToYUY2(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dst, dstStride, gArgs.width, gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.yuy2", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}
void toUYVY(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = gArgs.width * gArgs.height * 2;

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;

    int dstStride = gArgs.width * 2;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dst = dstBuf.data() + i * dstFrameSize;
        libyuv::I420ToUYVY(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dst, dstStride, gArgs.width, gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.uyvy", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}
void toNV12(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = (int)(gArgs.width * gArgs.height * 1.5);

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;

    int dstYStride = gArgs.width;
    int dstUVStride = gArgs.width;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dstY = dstBuf.data() + i * dstFrameSize;
        uint8_t *dstUV = dstY + gArgs.width * gArgs.height;
        libyuv::I420ToNV12(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dstY, dstYStride, dstUV, dstUVStride, gArgs.width, gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.nv12", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}
void toNV21(const CharBuffer &buffer)
{
    int size = buffer.size();
    int yuvFrameSize = (int)(gArgs.width * gArgs.height * 1.5);
    int frameCount = size / yuvFrameSize;
    int dstFrameSize = (int)(gArgs.width * gArgs.height * 1.5);

    int srcYStride = gArgs.width;
    int srcUStride = gArgs.width / 2;
    int srcVStride = gArgs.width / 2;

    int dstYStride = gArgs.width;
    int dstUVStride = gArgs.width;

    U8Buffer dstBuf;
    dstBuf.reserve(dstFrameSize * frameCount);
    dstBuf.fill(0);
    for (int i = 0; i < frameCount; ++i) {
        int yuvFrameOffset = i * yuvFrameSize;
        uint8_t *srcY = (uint8_t *)buffer.data() + yuvFrameOffset;
        uint8_t *srcU = srcY + gArgs.width * gArgs.height;
        uint8_t *srcV = srcU + gArgs.width * gArgs.height / 4;

        uint8_t *dstY = dstBuf.data() + i * dstFrameSize;
        uint8_t *dstUV = dstY + gArgs.width * gArgs.height;
        libyuv::I420ToNV21(srcY, srcYStride, srcU, srcUStride, srcV, srcVStride, dstY, dstYStride, dstUV, dstUVStride, gArgs.width, gArgs.height);
    }
    wchar_t fileName[1024];
    swprintf(fileName, 1024, L"%s%s.nv21", resPath.c_str(), gArgs.outputBaseFileName.c_str());
    writeFile(dstBuf, fileName);
}

int wmain(int argc, wchar_t *argv[])
{
    // wcout << resPath << endl;
    // wcout << sizeof(BMPFileHeader) << " " << sizeof(BMPInfoHeader) << endl;
    parseArgs(argc, argv);
    wcout << gArgs << endl;
    CharBuffer buffer;
    if (!readFile(buffer, gArgs.inputFileName)) {
        return 0;
    }
    //    toBMP(buffer);
    toI444(buffer);
    toI422(buffer);
    toYUY2(buffer);
    toUYVY(buffer);
    toNV12(buffer);
    toNV21(buffer);
    return 0;
}
