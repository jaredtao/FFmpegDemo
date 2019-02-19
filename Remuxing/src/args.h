#ifndef ARGS_H
#define ARGS_H
typedef struct _FileIO
{
    const char *inputFileName;
    const char *outputFileNme;
} FileIO;
extern int Hello(FileIO &io, int argc, char *argv[]);
#endif // ARGS_H
