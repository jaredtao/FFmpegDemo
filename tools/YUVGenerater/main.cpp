#include <iostream>
#include <string>
#include <string_view>
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
    wout << args.inputFileName << args.width << args.height << args.bitrate << endl;
    return wout;
}

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

    parseArgs(argc, argv);
    wcout << gArgs;



    return 0;
}
