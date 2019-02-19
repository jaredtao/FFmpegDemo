#include "audio.h"
#include "common.h"
#include "fileio.h"
#include "video.h"
#include <QDebug>
static int open_encoder_muxer(AVOutputFormat **fmt, AVFormatContext **fmtCtx, const char *fileName)
{
    av_register_all();
    avformat_alloc_output_context2(fmtCtx, NULL, NULL, fileName);
    if (!fmtCtx)
    {
        qDebug() << "could not deduce output format from file extension. using MPEG";
        avformat_alloc_output_context2(fmtCtx, NULL, "mpeg", fileName);
    }
    if (!fmtCtx)
    {
        qDebug() << "Error: cannot alloc any format context.";
        return -1;
    }
    *fmt = (*fmtCtx)->oformat;
    return 0;
}

int main(int argc, char *argv[])
{
    FileIO fileIO = { NULL };

    parseArgs(fileIO, argc, argv);

    AVDictionary *opt = NULL;
    AVOutputFormat *fmt = nullptr;
    AVFormatContext *fmtCtx = nullptr;
    AVCodec *videoCodec = nullptr;
    AVCodec *audioCodec = nullptr;
    int videoFrameIdx = 0, audioFrameIdx = 0;
    OutputStream audioStream = { NULL }, videoStream = { NULL };
    open_encoder_muxer(&fmt, &fmtCtx, fileIO.outputFileName);
    int ret = Add_audio_video_stream(&videoStream, &audioStream, fmtCtx, fmt, videoCodec, audioCodec, fileIO);
    int haveAudio = ret & HAVE_AUDIO;
    int haveVideo = ret & HAVE_VIDEO;
    int encode_video = ret & ENCODE_VIDEO;
    int encode_audio = ret & ENCODE_AUDIO;
    if (haveVideo)
    {
        ret = OpenVideo(fmtCtx, videoCodec, &videoStream, fileIO);
    }
    if (haveAudio)
    {
        ret = OpenAudio(fmtCtx, audioCodec, &audioStream, fileIO, opt);
    }
    av_dump_format(fmtCtx, 0, fileIO.outputFileName, 1);

    /* open the output file, if needed */
    if (!(fmt->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&fmtCtx->pb, fileIO.outputFileName, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            qDebug() << "could not open output file" << fileIO.outputFileName;
            outError(ret);
            return 1;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(fmtCtx, &opt);

    if (ret < 0)
    {
        qDebug() << "write head failed";
        outError(ret);
        return 1;
    }
    while (encode_video || encode_audio)
    {
        if (encode_video
            && (!encode_audio
                || av_compare_ts(videoStream.nextPts, videoStream.stream->codec->time_base, audioStream.nextPts, audioStream.stream->codec->time_base) <= 0))
        {
            encode_video = !Write_video_frame(fmtCtx, &videoStream);
            if (encode_video)
            {
                videoFrameIdx++;
                qDebug() << "write " << videoFrameIdx - 1 << "video frame";
            }
            else
            {
                qDebug() << "video end";
            }
        }
        else
        {
            encode_audio = !Write_audio_frame(fmtCtx, &audioStream);
            if (encode_audio)
            {
                audioFrameIdx++;
                qDebug() << "write " << audioFrameIdx - 1 << "audio frame";
            }
            else
            {
                qDebug() << "audio end";
            }
        }
    }
    av_write_trailer(fmtCtx);

    if (haveVideo)
    {
        Close_Stream(fmtCtx, &videoStream);
    }
    if (haveAudio)
    {
        Close_Stream(fmtCtx, &audioStream);
    }
    if (!(fmt->flags & AVFMT_NOFILE))
    {
        avio_closep(&fmtCtx->pb);
    }
    avformat_free_context(fmtCtx);
    fileIO.audioFile.close();
    fileIO.videoFile.close();

    qDebug() << "process succeed";
    return 0;
}
