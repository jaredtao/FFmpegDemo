#include "stream.h"

static int add_video_stream(OutputStream *st, AVFormatContext *fmtCtx, AVCodec **codec, enum AVCodecID codecID, int frameWidth, int frameHeight)
{
    *codec = avcodec_find_encoder(codecID);
    if (!(*codec))
    {
        qDebug() << "find codec failed";
        return -1;
    }
    st->stream = avformat_new_stream(fmtCtx, *codec);
    if (!st->stream)
    {
        qDebug() << "cannot add new video stream";
        return -2;
    }
    st->stream->id = fmtCtx->nb_streams - 1;
    AVCodecContext *codecCtx = st->stream->codec;
    codecCtx->codec_id = codecID;
    codecCtx->bit_rate = 400000;
    codecCtx->width = frameWidth;
    codecCtx->height = frameHeight;
    AVRational r = { 1, STREAM_FRAME_RATE };
    codecCtx->time_base = r;
    codecCtx->gop_size = 12;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    if (codecCtx->codec_id == AV_CODEC_ID_MPEG2VIDEO)
    {
        codecCtx->max_b_frames = 2;
    }
    if (codecCtx->codec_id == AV_CODEC_ID_MPEG1VIDEO)
    {
        codecCtx->mb_decision = 2;
    }
    if (fmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    return 1;
}
static int add_audio_stream(OutputStream *st, AVFormatContext *fmtCtx, AVCodec **codec, enum AVCodecID codecID)
{
    *codec = avcodec_find_encoder(codecID);
    if (!(*codec))
    {
        qDebug() << "find audio codec failed";
        return -1;
    }
    st->stream = avformat_new_stream(fmtCtx, *codec);
    if (!st->stream)
    {
        qDebug() << "cannot add new audio stream";
        return -2;
    }
    st->stream->id = fmtCtx->nb_chapters - 1;
    AVCodecContext *codecCtx = st->stream->codec;
    codecCtx->sample_fmt = (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
    codecCtx->bit_rate = 64000;
    codecCtx->sample_rate = 44100;

    if ((*codec)->supported_samplerates)
    {
        codecCtx->sample_rate = (*codec)->supported_samplerates[0];
        for (int i = 0; (*codec)->supported_samplerates[i]; i++)
        {
            if ((*codec)->supported_samplerates[i] == 44100)
            {
                codecCtx->sample_rate = 44100;
            }
        }
    }
    codecCtx->channels = av_get_channel_layout_nb_channels(codecCtx->channel_layout);
    codecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    if ((*codec)->channel_layouts)
    {
        codecCtx->channel_layout = (*codec)->channel_layouts[0];
        for (int i = 0; (*codec)->channel_layouts[i]; i++)
        {
            if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                codecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
        }
    }
    codecCtx->channels = av_get_channel_layout_nb_channels(codecCtx->channel_layout);
    AVRational r = { 1, codecCtx->sample_rate };
    st->stream->time_base = r;
    if (fmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    return 1;
}

int Add_audio_video_stream(
    OutputStream *videoStream,
    OutputStream *audioStream,
    AVFormatContext *fmtCtx,
    AVOutputFormat *fmt,
    AVCodec *videoCodec,
    AVCodec *audioCodec,
    const FileIO &fileIO)
{
    int ret = 0;
    if (fmt->video_codec != AV_CODEC_ID_NONE)
    {
        if (add_video_stream(videoStream, fmtCtx, &videoCodec, fmt->video_codec, fileIO.width, fileIO.height) > 0)
        {
            qDebug() << "add 1 video stream";
            ret |= HAVE_VIDEO;
            ret |= ENCODE_VIDEO;
        }
    }
    if (fmt->audio_codec != AV_CODEC_ID_NONE)
    {
        if (add_audio_stream(audioStream, fmtCtx, &audioCodec, fmt->audio_codec) > 0)
        {
            qDebug() << "add i audio stream";
            ret |= HAVE_AUDIO;
            ret |= ENCODE_AUDIO;
        }
    }
    return ret;
}
void Close_Stream(AVFormatContext *fmtCtx, OutputStream *outputSt)
{
    avcodec_close(outputSt->stream->codec);
    if (outputSt->audioFrame)
        av_frame_free(&outputSt->audioFrame);
    if (outputSt->videoFrame)
        av_frame_free(&outputSt->videoFrame);
    if (outputSt->tmp_frame)
        av_frame_free(&outputSt->tmp_frame);

    sws_freeContext(outputSt->sws_ctx);
    swr_free(&outputSt->swr_ctx);
}
