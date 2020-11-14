#include <QDebug>

#include "args.h"
#include "ffmpegHeader.h"
int main(int argc, char **argv)
{
    FileIO io;
    if (Hello(io, argc, argv))
    {
        return -1;
    }
    AVOutputFormat *outfmt = NULL;
    AVFormatContext *inputCtx = NULL, *outputCtx = NULL;
    AVPacket packet;
    int ret = 0;

    av_register_all();
    ret = avformat_open_input(&inputCtx, io.inputFileName, NULL, NULL);
    if (ret < 0)
    {
        qDebug() << "open input file failed:";
        outError(ret);
        goto end;
    }
    ret = avformat_find_stream_info(inputCtx, NULL);
    if (ret < 0)
    {
        qDebug() << "find stream failed:";
        outError(ret);
        goto end;
    }
    av_dump_format(inputCtx, 0, io.inputFileName, 0);

    avformat_alloc_output_context2(&outputCtx, NULL, NULL, io.outputFileNme);
    if (!outputCtx)
    {
        qDebug() << "alloc output context failed";
        goto end;
    }
    outfmt = outputCtx->oformat;
    for (unsigned int i = 0; i < inputCtx->nb_streams; i++)
    {
        AVStream *inSt = inputCtx->streams[i];
        AVStream *outSt = avformat_new_stream(outputCtx, inSt->codec->codec);
        if (!outSt)
        {
            qDebug() << "allock output stream failed";
            goto end;
        }
        ret = avcodec_copy_context(outSt->codec, inSt->codec);
        outSt->codec->codec_tag = 0;
        if (outputCtx->oformat->flags & AVFMT_GLOBALHEADER)
        {
            outSt->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    av_dump_format(outputCtx, 0, io.outputFileNme, 1);

    if (!(outfmt->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&outputCtx->pb, io.outputFileNme, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            qDebug() << "open output file failed";
            outError(ret);
            goto end;
        }
    }

    ret = avformat_write_header(outputCtx, NULL);
    if (ret < 0)
    {
        qDebug() << "write out file header failed";
        outError(ret);
        goto end;
    }

    while (1)
    {
        AVStream *inSt, *outSt;
        ret = av_read_frame(inputCtx, &packet);
        if (ret < 0)
        {
            break;
        }

        inSt = inputCtx->streams[packet.stream_index];
        outSt = outputCtx->streams[packet.stream_index];

        packet.pts = av_rescale_q_rnd(packet.pts, inSt->time_base, outSt->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.dts = av_rescale_q_rnd(packet.dts, inSt->time_base, outSt->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.duration = av_rescale_q(packet.duration, inSt->time_base, outSt->time_base);
        packet.pos = -1;

        ret = av_interleaved_write_frame(outputCtx, &packet);

        if (ret < 0)
        {
            qDebug() << "muxing packet failed";
            break;
        }
        av_free_packet(&packet);
    }
    av_write_trailer(outputCtx);
    qDebug() << "process succeed";
end:
    avformat_close_input(&inputCtx);
    if (outputCtx && !(outfmt->flags & AVFMT_NOFILE))
        avio_closep(&outputCtx->pb);
    avformat_free_context(outputCtx);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        qDebug() << "write packet to output file failed";
        return 1;
    }
    return 0;
}
