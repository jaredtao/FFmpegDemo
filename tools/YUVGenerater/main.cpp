#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include "libyuv.h"
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
template<class char_type>
class Buffer
{
public:
    Buffer(const char_type *buf, int size)
    {
        init(size);

        memcpy(pBuf, buf, size);
        mSize = size;
    }
    Buffer() { }
    ~Buffer()
    {
        delete[] pBuf;
        mSize = 0;
        mCapacity = 0;
    }
    void setData(const char_type *buf, int size)
    {
        if (mCapacity < size) {
            delete[] pBuf;
            init(size);
        }
        memcpy(pBuf, buf, size);
        mSize = size;
    }
    void reserve(int capacity)
    {
        if (capacity > mCapacity) {
            mCapacity = capacity;
            char_type *newBuf = new char_type[mCapacity];
            memset(newBuf, 0, mCapacity);

            memcpy(newBuf, pBuf, mSize);
            delete[] pBuf;
            pBuf = newBuf;
        }
    }
    void fill(char_type value)
    {
        memset(pBuf, value, mCapacity);
        mSize = mCapacity;
    }
    char_type *data() { return pBuf; }
    const char_type *constData() const { return pBuf; }
    int size() const { return mSize; }
    int capacity() const { return mCapacity; }

private:
    void init(int size)
    {
        mCapacity = size;
        pBuf = new char_type[mCapacity];
        memset(pBuf, 0, mCapacity);
    }

private:
    char_type *pBuf = nullptr;
    int mSize = 0;
    int mCapacity = 0;
};

#pragma pack(1)

struct BMPFileHeader
{
    uint16_t fileType = 0x4d42;
    uint32_t fileSize = 0; // sizeof BMPFileHeader + sizeof BMPInfoHeader + width * height * 3
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;
    uint32_t fileOffset = 0; // sizeof BMPFileHeader + sizeof BMPInfoHeader
};
struct BMPInfoHeader
{
    uint32_t infoSize = 0; // sizeof BMPInfoHeader
    int32_t width = 0;
    int32_t height = 0;
    uint16_t planes = 0;
    uint16_t bitCount = 0; // RGB24, 24
    uint32_t compressed = 0; // 0, not compressed
    uint32_t imageSize = 0; // RGB24 size
    int32_t xPelsPerMeter = 0;
    int32_t yPelsPerMeter = 0;
    uint32_t clrUsed = 0;
    uint32_t clrImported = 0;
};
#pragma pack()

class BMP
{
public:
    BMP(uint8_t *pBuf, int w, int h)
    {
        fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + w * h * 3;
        fileHeader.fileOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        infoHeader.infoSize = sizeof(BMPInfoHeader);
        infoHeader.width = w;
        infoHeader.height = h;
        infoHeader.planes = 1;
        infoHeader.bitCount = 24;
        infoHeader.imageSize = w * h * 3;

        buffer.reserve(w * h * 3);
        buffer.fill(0);

        int pos = 0;
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                buffer.data()[pos] = pBuf[i * w * 3 + j * 3 + 2];
                buffer.data()[pos + 1] = pBuf[i * w * 3 + j * 3+ 1];
                buffer.data()[pos+ 2] = pBuf[i * w * 3 + j * 3 ];
                pos += 3;
            }
        }

    }
    void save(const wstring &fileName)
    {
        ofstream out(fileName, ios::out | ios::binary);
        out.write((const char *)&fileHeader, sizeof fileHeader);
        out.write((const char *)&infoHeader, sizeof infoHeader);
        out.write((const char *)buffer.data(), infoHeader.imageSize);
    }

private:
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    Buffer<uint8_t> buffer;
};

static const wstring resPath = ResPath;
void parseArgs(int argc, wchar_t **argv)
{
    if (argc <= 4 || argv == nullptr) {
        cerr << "parse args failed, use default";
        gArgs.inputFileName = resPath + L"/flower_cif.yuv";
        gArgs.width = 352;
        gArgs.height = 288;
        gArgs.bitrate = 30412000;
        return;
    }
    gArgs.inputFileName = argv[1];
    gArgs.width = stoi(argv[2]);
    gArgs.height = stoi(argv[3]);
    gArgs.bitrate = stoi(argv[4]);
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
    int size = in.tellg();
    Buffer<char> buffer;
    buffer.reserve(size);
    buffer.fill(0);
    in.seekg(0, ios::beg);
    in.read(buffer.data(), size);

    in.close();

    int frameCount = size / (gArgs.width * gArgs.height * 1.5);

    char *pY = buffer.data();
    char *pU = buffer.data() + gArgs.width * gArgs.height * frameCount;
    char *pV = buffer.data() + gArgs.width * gArgs.height * frameCount
            + gArgs.width * gArgs.height * frameCount / 4;

    Buffer<uint8_t> rgbBuf;
    rgbBuf.reserve(gArgs.width * gArgs.height * 3 * frameCount);
    rgbBuf.fill(0);

    int yStride = gArgs.width;
    int uStride = gArgs.width / 2;
    int vStride = gArgs.width / 2;
    int rgbStride = gArgs.width * 3;
    // for (int i = 0; i < frameCount; ++i) {
    int ret = libyuv::I420ToRGB24((uint8_t *)pY, yStride, (uint8_t *)pU, uStride, (uint8_t *)pV,
                                  vStride, rgbBuf.data(), rgbStride, gArgs.width, gArgs.height);

    if (ret) {
        wcout << "I420ToRGB24 failed " << ret << endl;
        return 0;
    }
    BMP bmp(rgbBuf.data(), gArgs.width, gArgs.height);

    bmp.save(resPath + L"/flower.bmp");
    //}

    return 0;
}
