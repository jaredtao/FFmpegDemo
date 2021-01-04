#include "Convert.h"
#include <cstdlib>
#include <iostream>
#include <string>
using std::endl;
using std::cerr;
using std::cout;


using std::stoi;

using std::string;

static Args gArgs;

static const string resPath = ResPath;

void parseArgs(int argc, char **argv)
{
    if (argc <= 6 || argv == nullptr) {
        cerr << "parse args failed, use default" << endl;
        gArgs.inputFileName = resPath + "flower_cif.yuv";
        gArgs.outputFileName = resPath + "flower_cif.h264";
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

int main(int argc, char *argv[])
{
    parseArgs(argc, argv);
    cout << gArgs << endl;
    Convert convert;
    convert.convert(gArgs);

    return 0;
}
