#pragma once
#include <iostream>
#include <string>
struct Args
{
    std::string inputFileName;
    std::string outputFileName;
    int width;
    int height;
    int bitrate;
    int frameToEncode;
};
static std::ostream &operator<<(std::ostream &out, const Args &args)
{
    out << args.inputFileName << " " << args.outputFileName << " " << args.width << " " << args.height << " " << args.bitrate << " " << args.frameToEncode;
    return out;
}
