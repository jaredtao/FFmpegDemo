#ifndef AUDIO_H
#define AUDIO_H
#include "common.h"
#include "fileio.h"

extern int OpenAudio(AVFormatContext *fmtCtx, AVCodec *codec, OutputStream *st, FileIO &io, AVDictionary *opt_arg);
extern int Write_audio_frame(AVFormatContext *oc, OutputStream *ost);
#endif // AUDIO_H
