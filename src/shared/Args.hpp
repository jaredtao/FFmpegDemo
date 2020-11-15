#pragma once
#include <iostream>
#include <string>
struct Args
{
    std::wstring inputFileName;
    std::wstring outputFileName;
    int width;
    int height;
    int bitrate;
    int frameToEncode;
};
static std::wostream &operator<<(std::wostream &wout, const Args &args)
{
    wout << args.inputFileName << " " << args.outputFileName << " " << args.width << " " << args.height << " " << args.bitrate << " " << args.frameToEncode;
    return wout;
}
