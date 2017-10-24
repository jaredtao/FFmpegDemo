#include "decodertmp.h"
#include <QDebug>
#include <QCoreApplication>
//const char *url = "rtmp://192.168.1.154:1935/p/l live=1\0";
const char *url = "E:/AutoIO/Shared/Shared/video.mov";
static void outError(int num)
{
	QByteArray error;
	error.resize(1024);
	av_strerror(num, error.data(), error.size());
	qDebug() << "FFMPEG ERROR:" << QString(error);
}
// <0 : failed
// >=0 : index
int DecodeRtmp::open_codec_context(AVMediaType type)
{
	AVDictionary *opts = NULL;
	if (!FFmpeg.fmtCtx) {
		qDebug()<< "fmtCtx is null";
		return -1;
	}
	int index = av_find_best_stream(FFmpeg.fmtCtx, type, -1, -1, NULL, 0);
	if (index < 0) {
		qDebug() << "find best stream failed";
		outError(index);
		return -1;
		//		int ret = avformat_alloc_output_context2(&FFmpeg.fmtCtx, NULL, "flv", NULL);

		//		if (ret < 0) {
		//			qDebug() << "alloc output context failed" ;
		//			outError(ret);
		//			return ret;
		//		}
	}
	FFmpeg.stream = FFmpeg.fmtCtx->streams[index];
	if (type == AVMEDIA_TYPE_VIDEO) {
		qDebug() << "Video stream time base: {" <<FFmpeg.stream->time_base.num <<", "<< FFmpeg.stream->time_base.den << "}";
	}
	FFmpeg.codecCtx = FFmpeg.stream->codec;
	FFmpeg.codec = avcodec_find_decoder(FFmpeg.codecCtx->codec_id);
	if (!FFmpeg.codec) {
		qDebug() << "find codec failed";
		return -1;
	}

	//		/* Init the decoders, with or without reference counting */

	//without
	av_dict_set(&opts, "refcounted_frames", "0", 0);

	//with
	//	av_dict_set(&opts, "refcounted_frames", "1", 0);

	int ret = avcodec_open2(FFmpeg.codecCtx, FFmpeg.codec, &opts) ;
	if (ret < 0) {
		qDebug() << "open codec failed";
		outError(ret);
		return -2;
	}
	return index;
}

int DecodeRtmp::decode_packet(int & gotFrame, bool cached)
{
	int ret = 0;
	int decoded = packet.size;
	static int video_frame_count = 0;
	static int audio_frame_count = 0;
	gotFrame = 0;
	if (packet.stream_index == video.streamIndex) {
		ret = avcodec_decode_video2(video.codecCtx, frame, &gotFrame, &packet);
		if (ret < 0) {
			qDebug() << "decode video frame failed";
			outError(ret);
			return -1;
		}

		if (gotFrame) {
			if (video.codecCtx->width != frame->width || video.codecCtx->height != frame->height || video.pix_format != frame->format){
				qDebug() << "Error, width or height mot match";
				return -1;
			}

			//qDebug() << "video frame "<< (cached  ? QString("(cached)") : QString("")) <<video_frame_count << "coded_n: " << frame->coded_picture_number << "pts:" << frame->pts;
			video_frame_count++;
			av_image_copy(video.dest_data, video.dest_linesize, (const uint8_t **)frame->data, frame->linesize, video.pix_format, frame->width, frame->height);
#ifdef OUT_VIDEO_TO_FILE
			video.outFile.write((const char *)video.dest_data[0], video.dest_bufsize);
#endif
			//TODO output videobuf
			videoData.clear();
			videoData.append((const char *)video.dest_data[0], video.dest_bufsize);
			emit readyVideo(videoData, frame->width, frame->height, video.pix_format);
		}
	} else if (packet.stream_index == audio.streamIndex){
		ret = avcodec_decode_audio4(audio.codecCtx, frame, &gotFrame, &packet);
		if (ret < 0) {
			qDebug() << "audio decode failed";
			outError(ret);
			return -1;
		}

		decoded = FFMIN(ret, packet.size);

		if (gotFrame) {
			//qDebug() << "audio frame " << (cached  ? QString("(cached)") : QString("")) <<audio_frame_count << "b_samples" << frame->nb_samples << "pts:" << frame->pts;
			audio_frame_count++;
			size_t linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
			//			audioFile.write((const char *)frame->extended_data[0], linesize);
			//TODO output audio buf
			audioData.clear();
			audioData.append((const char *)frame->extended_data[0], linesize);
			emit readyAudio(audioData);
		}
	}
	return FFMIN(ret, packet.size);
}
int DecodeRtmp::init()
{
	int ret = 0;
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
    //这里的URL可以是网络地址，也可以是本地文件
	ret = avformat_open_input(&FFmpeg.fmtCtx, url, NULL, NULL);

	if (ret < 0) {
		qDebug() << "open url error";
		outError(ret);
		return -1;
	}
	ret = avformat_find_stream_info(FFmpeg.fmtCtx, NULL);
	if ( ret< 0) {
		qDebug() <<"find stream failed";
		outError(ret);
		return -1;
	}
	ret = open_codec_context(AVMEDIA_TYPE_VIDEO);
    if ( ret >= 0 ) {
		video.streamIndex = ret;
		video.codecCtx = FFmpeg.codecCtx;
		video.stream = FFmpeg.stream;
		video.codec  = FFmpeg.codec;
		video.frameWidth = video.codecCtx->width;
		video.frameHeight= video.codecCtx->height;
		video.pix_format = video.codecCtx->pix_fmt;

		video.dest_bufsize = av_image_alloc(video.dest_data, video.dest_linesize, video.frameWidth, video.frameHeight, video.pix_format, 1);
		if (video.dest_bufsize < 0) {
			qDebug() << "alloc image buf failed";
			return -2;
		}
#ifdef OUT_VIDEO_TO_FILE
		video.outFile.setFileName("out.yuv");

		if (!video.outFile.open(QFile::ReadWrite)) {
			qDebug() << "open video output file failed";
			return -3;
		}
#endif
		qDebug() << "oepn video context success";
	} else {
		qDebug() << "open video context failed";
	}

	ret = open_codec_context(AVMEDIA_TYPE_AUDIO);
	if ( ret>=0) {
		audio.streamIndex = ret;
		audio.codecCtx = FFmpeg.codecCtx;
		audio.stream = FFmpeg.stream;
		audio.codec  = FFmpeg.codec;
		//		audioFile.setFileName(Args.output_Audio_FileName);
		//		if (!audioFile.open(QFile::ReadWrite)) {
		//			qDebug() << "open audio output file failed";
		//			return -3;
		//		}
        qDebug() << "open audio context success";
	} else {
		qDebug() << "open audio context failed";
	}
	if (video.stream) {
		qDebug() << "Demuxing video ";
	}
	if (audio.stream) {
		qDebug() << "Demuxing audio ";
	}
	av_dump_format(FFmpeg.fmtCtx, 0, url, 0);
	if (!video.stream && !audio.stream) {
		qDebug() << "Could not find audio or video stream.";
		return -1;
	}
	frame = av_frame_alloc();
	if (frame == nullptr) {
		qDebug() << "alloc frame failed";
		return -1;
	}
	av_init_packet(&packet);
	packet.data = nullptr;
	packet.size = 0;
	return 0;
}
void DecodeRtmp::decode()
{
	int ret = 0;
	int gotFrame = 0;
	while(1) {
		int get = av_read_frame(FFmpeg.fmtCtx, &packet);
		if (get < 0) {
			break;
		}
        if (mStop) {
			goto stop;
		}
		do {
            if (mStop) {
				goto stop;
			}
			ret = decode_packet(gotFrame, false);
			if (ret < 0) {
				break;
			}
			packet.data += ret;
			packet.size -= ret;
		} while( packet.size > 0);
	}
	packet.data = nullptr;
	packet.size = 0;
	do {
        if (mStop) {
			goto stop;
		}
		ret = decode_packet(gotFrame, true);
	} while(gotFrame);
stop:
	return ;
}
void DecodeRtmp::release()
{
	if (video.codecCtx)
		avcodec_close(video.codecCtx);
	if (audio.codecCtx)
		avcodec_close(audio.codecCtx);
	if (FFmpeg.fmtCtx)
		avformat_close_input(&FFmpeg.fmtCtx);
	if (frame)
		av_frame_free(&frame);
	if (video.dest_data[0])
		av_free(video.dest_data[0]);
#ifdef OUT_VIDEO_TO_FILE
	video.outFile.close();
#endif
}
DecodeRtmp::DecodeRtmp(QObject *parent) : QObject(parent)
{
    mStop = 0;
}
DecodeRtmp::~DecodeRtmp()
{
	release();
}
void DecodeRtmp::work()
{
    mStop = 0;
	int ret = 0;
	ret = init();
	if (ret!=0) {
		qDebug() << "init failed";
	}
	decode();
	release();
}
void DecodeRtmp::stop()
{
    mStop = 1;
}
DecoderController::DecoderController()
{
	cache.setCapacity(20);
	himma = new DecodeRtmp;
	himma->moveToThread(&thread);
	connect(&thread, SIGNAL(finished()), himma, SLOT(deleteLater()));
	connect(this, SIGNAL(work()), himma, SLOT(work()));
	connect(this, SIGNAL(stop()), himma, SLOT(stop()));
	connect(this, SIGNAL(stop()), &thread, SIGNAL(finished()));
	//	connect(himma, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SIGNAL(readyVideo(QByteArray,int,int,int)));
	connect(himma, SIGNAL(readyVideo(QByteArray,int,int,int)), this, SLOT(processVideo(QByteArray,int,int,int)));
	connect(himma, SIGNAL(readyAudio(QByteArray)), this, SIGNAL(readyAudio(QByteArray)));
	thread.start();
}
DecoderController::~DecoderController()
{
	if (thread.isRunning()) {
		thread.quit();
//		thread.wait();
	}
}
VideoData DecoderController::getData(int &got)
{
	if (cache.isEmpty()) {
		got = 0;
		return VideoData{NULL};
	}
	got = 1;
	return cache.takeFirst();
}
void DecoderController::processVideo(const QByteArray &data, int width, int height, int pixfmt)
{
	VideoData v;
	v.data = data;
	v.width = width;
	v.heigth = height;
	v.pixfmt = pixfmt;
	cache.append(v);
}
