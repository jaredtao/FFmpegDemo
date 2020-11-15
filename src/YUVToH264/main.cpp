#include "Convert.h"
#include <cstdlib>
#include <iostream>
#include <string>
using std::endl;
using std::wcerr;
using std::wcout;

using std::wostream;

using std::stoi;

using std::string;
using std::wstring;

static Args gArgs;

static const wstring resPath = ResPath;

void parseArgs(int argc, wchar_t **argv)
{
    if (argc <= 6 || argv == nullptr) {
        wcerr << L"parse args failed, use default" << endl;
        gArgs.inputFileName = resPath + L"flower_cif.yuv";
        gArgs.outputFileName = resPath + L"flower_cif.h264";
        gArgs.width = 352;
        gArgs.height = 288;
        gArgs.bitrate = 30412000;
        gArgs.frameToEncode = 600;
        return;
    }
    gArgs.inputFileName = argv[1];
    gArgs.outputFileName = argv[2];
    gArgs.width = stoi(argv[3]);
    gArgs.height = stoi(argv[4]);
    gArgs.bitrate = stoi(argv[5]);
    gArgs.frameToEncode = stoi(argv[6]);
}

int wmain(int argc, wchar_t *argv[])
{
    parseArgs(argc, argv);
    wcout << gArgs << endl;
    Convert convert;
    convert.convert(gArgs);

    return 0;
}
