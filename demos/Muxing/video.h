#ifndef VIDEO_H
#define VIDEO_H

#include "common.h"
#include "fileio.h"
extern int OpenVideo(AVFormatContext *fmtCtx, AVCodec *codec, OutputStream *st, FileIO &io);
extern int Write_video_frame(AVFormatContext *oc, OutputStream *ost);

#endif // VIDEO_H
