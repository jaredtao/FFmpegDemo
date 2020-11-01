#ifndef FFMPEG_MUXER_H
#define FFMPEG_MUXER_H
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

#include "libavutil/avassert.h"
#include "libavutil/channel_layout.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "libavutil/timestamp.h"
}

#endif // FFMPEG_MUXER_H
