#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include "Buffer.h"
namespace BMP {

#pragma pack(1)
struct BMPFileHeader
{
    uint16_t fileType = 'B' + ('M' << 8);
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

class BMPSaver
{
public:
    using wstring = std::wstring;
    using ofstream = std::ofstream;
    using ios = std::ios;

    BMPSaver(uint8_t *pBuf, int w, int h)
    {
        int padSize = (4 - (w * 3) % 4) % 4;
        int dataSize = w * h * 3 + h * padSize;
        fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize;
        fileHeader.fileOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

        infoHeader.infoSize = sizeof(BMPInfoHeader);
        infoHeader.width = w;
        infoHeader.height = -h;
        infoHeader.planes = 1;
        infoHeader.bitCount = 24;
        infoHeader.imageSize = dataSize;

        buffer.reserve(infoHeader.imageSize);
        buffer.fill(0);

        int pos = 0;
        int srcR = 0;
        int srcG = 0;
        int srcB = 0;
        int tarR = 0;
        int tarG = 0;
        int tarB = 0;
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                srcR = i * w * 3 + j * 3;
                srcG = srcR + 1;
                srcB = srcR + 2;

                //bmp need BGR
                tarB = pos;
                tarG = pos + 1;
                tarR = pos + 2;

                buffer.data()[tarR] = pBuf[srcR];
                buffer.data()[tarG] = pBuf[srcG];
                buffer.data()[tarB] = pBuf[srcB];

                pos += 3;
            }
        }
    }
    void save(const wstring &fileName)
    {
        ofstream out(fileName, ios::out | ios::binary);
        if (!out.is_open()) {
            return;
        }
        out.write((const char *)&fileHeader, sizeof fileHeader);
        out.write((const char *)&infoHeader, sizeof infoHeader);
        out.write((const char *)buffer.data(), infoHeader.imageSize);
    }

private:
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    Buffer<uint8_t> buffer;
};

}
