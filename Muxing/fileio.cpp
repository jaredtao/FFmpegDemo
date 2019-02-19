#include "fileio.h"

void parseArgs(FileIO &fileIO, int argc, char *argv[])
{
    if (argc <= 1 || argv == nullptr)
    {
        qDebug() << "parse args failed";
        qDebug() << "Usage: " << argv[0] << " outputFileName inputAudioFileName inputVideoFilename width height";
        return;
    }
    fileIO.outputFileName = argv[1];
    fileIO.inputAudioFileName = argv[2];
    fileIO.inputVideoFileName = argv[3];
    fileIO.width = atoi(argv[4]);
    fileIO.height = atoi(argv[5]);
    qDebug() << fileIO;
}
QDebug operator<<(QDebug dbg, const FileIO &args)
{
    QDebugStateSaver saver(dbg);
    dbg.noquote() << args.inputVideoFileName << args.inputAudioFileName << args.outputFileName << args.width << args.height;
    return dbg;
}
void outError(int number)
{
    QByteArray error;
    error.resize(1024);
    av_strerror(number, error.data(), error.size());
    qDebug() << QString(error);
}
