//
// Created by heshiqi on 17/7/5.
//
#include <stdio.h>
#include "pushvideo.h"
#include <android/log.h>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <libavutil/mathematics.h>
};

#define LOGD(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "pushvideo", format, ##__VA_ARGS__)
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR,  "pushvideo", format, ##__VA_ARGS__)

static jobject gObj = NULL;
jmethodID nativeCallBack = NULL;
bool isRuning= false;

void clear(AVFormatContext *inFormatContext, AVFormatContext *outFormatContext) {
    avformat_close_input(&inFormatContext);
    if (outFormatContext && !(outFormatContext->flags & AVFMT_NOFILE))
        avio_close(outFormatContext->pb);
    avformat_free_context(outFormatContext);

}

AVCodecContext* getCodecWithStream(AVStream *stream) {
    AVCodecContext *codec = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(codec, stream->codecpar);
    return codec;
}

void setParametersWithCodec(AVCodecContext* codecContext,AVStream* stream){
    avcodec_parameters_from_context(stream->codecpar, codecContext);
}

int findVideoIndex(AVFormatContext* avFormatContext){
    int videoIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        AVMediaType type = getCodecWithStream(avFormatContext->streams[i])->codec_type;
        if (type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    return videoIndex;
}
JNIEXPORT jint JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_init(JNIEnv *env, jobject obj){
    gObj = env->NewGlobalRef(obj);
    if (gObj == NULL) return -1;
    jclass clazz = env->GetObjectClass(obj);
    if (NULL == nativeCallBack){
        nativeCallBack = env->GetMethodID(clazz,"nativeCall","(I)V");
        if (NULL == nativeCallBack){
            LOGE("不能找到方法 FFmpeg nativeCall.\n");
        }
    }

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_pushVideoStream(JNIEnv *env, jobject instance,
                                                                   jstring srcFilePath_,
                                                                   jstring url_) {
    const char *srcFilePath = env->GetStringUTFChars(srcFilePath_, 0);
    const char *url = env->GetStringUTFChars(url_, 0);

    isRuning= true;

    AVFormatContext *inFormatContext = NULL;
    AVFormatContext *outFormatContext = NULL;

    av_register_all();
    avformat_network_init();

    if (avformat_open_input(&inFormatContext, srcFilePath, NULL, NULL) < 0) {
        LOGE("打开文件失败\n");
        clear(inFormatContext, outFormatContext);
        return -1;
    }

    if (avformat_find_stream_info(inFormatContext, 0) < 0) {
        LOGE("获取流信息失败\n");
        clear(inFormatContext, outFormatContext);
        return -1;
    }

    int videoIndex = findVideoIndex(inFormatContext);

    if (avformat_alloc_output_context2(&outFormatContext, NULL, "flv", url) < 0) {
        LOGE("创建输出流context 失败\n");
        clear(inFormatContext, outFormatContext);
        return -1;
    }

    for (int i = 0; i < inFormatContext->nb_streams; i++) {
        AVStream *inStream = inFormatContext->streams[i];
        AVStream *outStream = avformat_new_stream(outFormatContext, inStream->codec->codec);
        if (!outStream) {
            LOGE("创建输出流失败\n");
            clear(inFormatContext, outFormatContext);
            return -1;
        }
        if (avcodec_copy_context(outStream->codec, inStream->codec) < 0) {
            LOGE("创建输出流失败\n");
            clear(inFormatContext, outFormatContext);
            return -1;
        }
        outStream->codec->codec_tag = 0;
        if (outFormatContext->oformat->flags & AVFMT_GLOBALHEADER) {
            outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    if (!(outFormatContext->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outFormatContext->pb, url, AVIO_FLAG_WRITE) < 0) {
            LOGE("不能打开推流服务器地址 URL '%s'", url);
            clear(inFormatContext, outFormatContext);
            return -1;
        }
    }

    //Write file header
    if (avformat_write_header(outFormatContext, NULL) < 0) {
        LOGE("连接打开推流服务器失败\n");
        clear(inFormatContext, outFormatContext);
        return -1;
    }

    AVPacket packet;
    int frame_index = 0;
    int64_t start_time = av_gettime();
    while (isRuning) {
        AVStream *in_stream;
        AVStream *out_stream;

        if (av_read_frame(inFormatContext, &packet) < 0) {
            break;
        }
        if (packet.pts == AV_NOPTS_VALUE) {
            AVRational time_base1 = inFormatContext->streams[videoIndex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE /
                    av_q2d(inFormatContext->streams[videoIndex]->r_frame_rate);

            packet.pts = (double) (frame_index * calc_duration) /
                         (double) (av_q2d(time_base1) * AV_TIME_BASE);
            packet.dts = packet.pts;
            packet.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
        }
        //Important:Delay
        if (packet.stream_index == videoIndex) {
            AVRational time_base = inFormatContext->streams[videoIndex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(packet.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time) {
                av_usleep(pts_time - now_time);
            }
        }

        in_stream = inFormatContext->streams[packet.stream_index];
        out_stream = outFormatContext->streams[packet.stream_index];

        /* copy packet */
        //Convert PTS/DTS

        packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base,
                                      (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base,
                                      (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet.duration = av_rescale_q(packet.duration, in_stream->time_base,
                                       out_stream->time_base);
        packet.pos = -1;
        //Print to Screen
        if (packet.stream_index == videoIndex) {
            if(gObj){
                env->CallVoidMethod(gObj, nativeCallBack, frame_index);
            }

            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);

        if (av_interleaved_write_frame(outFormatContext, &packet) < 0) {
            LOGE("Error muxing packet\n");
            break;
        }
        av_packet_unref(&packet);
    }

    av_write_trailer(outFormatContext);

    clear(inFormatContext, outFormatContext);

    env->ReleaseStringUTFChars(srcFilePath_, srcFilePath);
    env->ReleaseStringUTFChars(url_, url);

    return 0;
}

JNIEXPORT void JNICALL
Java_com_example_heshiqi_livepushdome_FFmpeg_exit(JNIEnv *env, jobject instance){
    isRuning= false;
    env->DeleteGlobalRef(gObj);
    gObj=NULL;
}

