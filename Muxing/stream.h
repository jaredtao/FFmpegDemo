#ifndef STREAM_H
#define STREAM_H
#include "ffmpeg_muxer.h"
#include "fileio.h"

#define HAVE_VIDEO (1)
#define ENCODE_VIDEO (1 << 1)
#define HAVE_AUDIO (1 << 2)
#define ENCODE_AUDIO (1 << 3)

#define STREAM_FRAME_RATE 25

typedef struct _OutputStream
{
    AVStream *stream;
    int64_t nextPts;
    int samples_count;

    AVFrame *videoFrame;
    AVFrame *audioFrame;
    AVFrame *tmp_frame;
    float t, tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

extern int Add_audio_video_stream(
    OutputStream *videoStream,
    OutputStream *audioStream,
    AVFormatContext *fmtCtx,
    AVOutputFormat *fmt,
    AVCodec *videoCodec,
    AVCodec *audioCodec,
    const FileIO &fileIO);
extern void Close_Stream(AVFormatContext *fmtCtx, OutputStream *outputSt);
#endif // STREAM_H
