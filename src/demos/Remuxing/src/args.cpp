#include "args.h"
#include <QDebug>
int Hello(FileIO &io, int argc, char *argv[])
{
    if (argc <= 2 || argv == nullptr)
    {
        qDebug() << "USAGE: inputFileName outputFileName";
        return -1;
    }
    io.inputFileName = argv[1];
    io.outputFileNme = argv[2];
    qDebug() << "args: " << io.inputFileName << io.outputFileNme;
    return 0;
}

