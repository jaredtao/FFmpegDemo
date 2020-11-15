#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include "Buffer.hpp"
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


//RGB24 little endian (bgr in memory) to bmp

int saveBMP(uint8_t *pRGB24Buf, int w, int h, const std::wstring &fileName)
{
    using std::ios;
    using std::ofstream;
    using std::wstring;

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    int padSize = (4 - (w * 3) % 4) % 4;
    int dataSize = w * h * 3 + h * padSize;
    fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + dataSize;
    fileHeader.fileOffset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    infoHeader.infoSize = sizeof(BMPInfoHeader);
    infoHeader.width = w;
    infoHeader.height = -h; // flip
    infoHeader.planes = 1;
    infoHeader.bitCount = 24;
    infoHeader.imageSize = dataSize;

    ofstream out(fileName, ios::out | ios::binary);
    if (!out.is_open()) {
        return -1;
    }
    out.write((const char *)&fileHeader, sizeof fileHeader);
    out.write((const char *)&infoHeader, sizeof infoHeader);
    out.write((const char *)pRGB24Buf, infoHeader.imageSize);
    out.close();
    return 0;
}

}
