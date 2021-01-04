#include "Convert.h"
#include "Math.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

#include <random>
int main(int argc, char **argv) { 
    using std::default_random_engine;
    using std::cout;
    using std::endl;

    std::vector<int> s;
    default_random_engine e;
    for (int i = 0; i < 10; ++i) { 
        s.push_back(e() % 100);
    }
    for (int i = 0; i < 10; ++i) { 
        cout << i << " "<< s.at(i) << endl;
    }
    cout << "Min random:" << e.min() << endl;
    cout << "Max random:" << e.max() << endl;
    auto v = average<int>(s.begin(), s.end());
    cout << v << endl;

    auto varianc = standardDeviation<float>(s.begin(), s.end());
    cout << varianc << endl;
    return 0;
}
        //
//extern "C" {
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavutil/imgutils.h>
//#include <libavutil/opt.h>
//#include <libavutil/samplefmt.h>
//}
//
//#define INBUF_SIZE (512 * 256)
//#define AUDIO_INBUF_SIZE 20480
//
//struct _Args
//{
//    QString inputFileName;
//    QString outputFileName;
//    int width;
//    int height;
//    int bitrate;
//    int frameToEncode;
//    _Args()
//        : inputFileName(""), outputFileName(""), width(0), height(0), bitrate(0), frameToEncode(0)
//    {
//    }
//} gArgs;
//QDebug operator<<(QDebug dbg, const _Args &args)
//{
//    QDebugStateSaver saver(dbg);
//    dbg.noquote() << args.inputFileName << args.outputFileName << args.width << args.height
//                  << args.bitrate << args.frameToEncode;
//    return dbg;
//}
//static AVCodec *codec = nullptr;
//static AVCodecContext *codecCtx = nullptr;
//static AVCodecParserContext *codecParserCtx = nullptr;
//static AVFrame *frame = nullptr;
//static AVPacket packet;
//static QFile inFile;
//static QFile outFile;
//
//static int parseArgs(int argc, char *argv[])
//{
//    if (argc <= 1 || argv == nullptr) {
//        qDebug() << "parse args failed, use default";
//        gArgs.inputFileName = QString(ResPath) + QString("flower_cif.h264");
//        gArgs.outputFileName = QString(ResPath) + QString("flower_cif_fromh264.yuv");
//        return 0;
//    }
//    gArgs.inputFileName = argv[1];
//    gArgs.outputFileName = argv[2];
//    //	Args.width = atoi( argv[3]);
//    //	Args.height = atoi(argv[4]);
//    //	Args.bitrate = atoi(argv[5]);
//    //	Args.frameToEncode = atoi(argv[6]);
//    return 0;
//}
//
//static int init()
//{
//    inFile.setFileName(gArgs.inputFileName);
//    if (!inFile.open(QFile::ReadOnly)) {
//        qDebug() << __FILE__ << __LINE__ << "open input file failed";
//        return -1;
//    }
//    outFile.setFileName(gArgs.outputFileName);
//    if (!outFile.open(QFile::ReadWrite)) {
//        qDebug() << __FILE__ << __LINE__ << "open output file failed";
//        return -1;
//    }
////    av_register_all();
//    av_init_packet(&packet);
//    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
//    if (NULL == codec) {
//        qDebug() << "codec find failed";
//        return -1;
//    }
//    codecCtx = avcodec_alloc_context3(codec);
//    if (codecCtx == nullptr) {
//        qDebug() << "codecCtx find failed";
//        return -2;
//    }
//    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
//        codecCtx->flags |= AV_CODEC_CAP_TRUNCATED;
//    }
//    codecParserCtx = av_parser_init(AV_CODEC_ID_H264);
//    if (codecParserCtx == nullptr) {
//        qDebug() << "parser context init failed";
//        return -3;
//    }
//
//    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
//        qDebug() << "codec open failed";
//        return -4;
//    }
//
//    frame = av_frame_alloc();
//    if (frame == nullptr) {
//        qDebug() << "frame alloc falied";
//        return -5;
//    }
//    return 0;
//}
//static void release()
//{
//    inFile.close();
//    outFile.close();
//    avcodec_close(codecCtx);
//    av_free(codecCtx);
//    av_frame_free(&frame);
//}
//
//static void write_out_frame()
//{
//    uint8_t **pBuf = frame->data;
//    int *pStride = frame->linesize;
//    for (int i = 0; i < 3; i++) {
//        int w = (i == 0 ? frame->width : frame->width / 2);
//        int h = (i == 0 ? frame->height : frame->height / 2);
//        for (int j = 0; j < h; j++) {
//            outFile.write((const char *)pBuf[i], w);
//            pBuf[i] += pStride[i];
//        }
//        outFile.flush();
//    }
//}
//int main(int argc, char *argv[])
//{
//    uint8_t *pDataPtr = nullptr;
//    int uDataSize = 0;
//    int got_picture = 0, len = 0;
//
//    uint8_t inBuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
//    memset(inBuf, 0, INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
//
//    if (parseArgs(argc, argv) < 0) {
//        qDebug() << "args parse failed";
//        return -1;
//    }
//    qDebug() << gArgs;
//    if (init() < 0) {
//        qDebug() << "init failed";
//        return -1;
//    }
//    packet.data = NULL;
//    packet.size = 0;
//    forever {
//        uDataSize = inFile.read((char *)inBuf, INBUF_SIZE);
//        if (uDataSize <= 0) {
//            break;
//        }
//        pDataPtr = inBuf;
//        while (uDataSize > 0) {
//            len = av_parser_parse2(codecParserCtx, codecCtx, &packet.data, &packet.size, pDataPtr,
//                                   uDataSize, AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
//
//            pDataPtr += len;
//            uDataSize -= len;
//            if (0 == packet.size) {
//                continue;
//            }
//            int ret = avcodec_decode_video2(codecCtx, frame, &got_picture, &packet);
//            if (ret < 0) {
//                qDebug() << "decode  failed";
//                return ret;
//            }
//            if (got_picture) {
//                write_out_frame();
//                qDebug() << "Succeed to decode 1 frame ! Frame pts: " << packet.pts;
//            }
//        }
//    }
//    packet.data = NULL;
//    packet.size = 0;
//    forever {
//        int ret = avcodec_decode_video2(codecCtx, frame, &got_picture, &packet);
//        if (ret < 0) {
//            qDebug() << "decode  failed";
//            return ret;
//        }
//        if (got_picture) {
//            write_out_frame();
//            qDebug() << "Succeed to decode 1 frame ! Frame pts: " << packet.pts;
//        } else {
//            break;
//        }
//    }
//    release();
//    return 0;
//}
