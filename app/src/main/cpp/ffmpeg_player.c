//
// Created by Administrator on 2019/3/28.
//
#include "cn_onestravel_ndk_ffmpeg_render_VideoPlayer.h"
#include <android/log.h>
#include <unistd.h>
//编码
#include "include/libavcodec/avcodec.h"
//封装格式处理
#include "include/libavformat/avformat.h"
//像素处理
#include "include/libswscale/swscale.h"
#include "include/libavutil/avutil.h"
#include "include/libavutil/frame.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <libyuv.h>
#include <pthread.h>


#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFMPEG",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFMPEG",FORMAT,##__VA_ARGS__);


/*
 * Class:     cn_onestravel_ndk_ffmpeg_render_VideoPlayer
 * Method:    render
 * Signature: (Ljava/lang/String;Landroid/view/Surface;)V
 */
JNIEXPORT void JNICALL Java_cn_onestravel_ndk_ffmpeg_render_VideoPlayer_render
        (JNIEnv *env, jclass jcls, jstring jstr_input, jobject surface) {
    const char *cstr_input = (*env)->GetStringUTFChars(env, jstr_input, NULL);
    //注册ffmpeg 所有组件
    av_register_all();
    //封装格式上下文
    AVFormatContext *formatContext = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&formatContext, cstr_input, NULL, NULL) != 0) {
        LOGE("无法打开视频文件");
        return;
    }
    //获取视频文件信息
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        LOGE("获取视频文件信息失败");
        return;
    }
    //获取视频流的索引位置
    //遍历所有类型的流（音频流，视频流、字幕流）
    int i = 0;
    int v_stream_index = -1;
    for (; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_index = i;
            break;
        }
    }
    if (v_stream_index < 0) {
        LOGE("%s", "找不到视频流\n");
        return;
    }
    //获取视频流中的编解码的上下文
    AVCodecContext *avCodecContext = formatContext->streams[v_stream_index]->codec;
    //根据视频编解码上下文的id得到对应的编解码器

    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("未找到解码器");
        return;
    }
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("打开解码器失败");
        return;
    }
    //输出视频信息
    LOGI("视频文件格式：%s", formatContext->iformat->name);
    //formatContext->duration单位为微妙
    LOGI("视频时长：%lld", (formatContext->duration) / 1000000);
    LOGI("视频的宽度和高度 W：%d ,H：%d", avCodecContext->width, avCodecContext->height);
    LOGI("视频解码器名称：%s", avCodec->name);
    //准备读取
    //AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *yuv_scale_frame = av_frame_alloc();
    AVFrame *rgba_frame = av_frame_alloc();
    //Native 绘制
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //缓冲区buffer
    ANativeWindow_Buffer windowBuffer;

    int len, got_frame, frame_count = 0;
    while (av_read_frame(formatContext, packet) >= 0) {
        len = avcodec_decode_video2(avCodecContext, yuv_frame, &got_frame, packet);
        //不为0,正在解码
        if (got_frame) {
            int i = frame_count++;
            LOGI("解码%d帧", i);
            ANativeWindow_setBuffersGeometry(nativeWindow, avCodecContext->width,
                                             avCodecContext->height, WINDOW_FORMAT_RGBA_8888);
//            struct SwsContext *m_pSwsContext = sws_getContext(avCodecContext->width,
//                                                              avCodecContext->height,
//                                                              PIX_FMT_YUV420P,
//                                                              avCodecContext->height,
//                                                              avCodecContext->width,
//                                                              PIX_FMT_YUV420P,
//                                                              SWS_BICUBIC,
//                                                              NULL, NULL, NULL);
//            sws_scale(m_pSwsContext, yuv_frame->data, yuv_frame->linesize, 0,
//                      avCodecContext->height, yuv_frame->data, yuv_frame->linesize);
            //LOCK
            ANativeWindow_lock(nativeWindow, &windowBuffer, NULL);

            avpicture_fill((AVPicture *) rgba_frame, windowBuffer.bits, PIX_FMT_RGBA,
                           avCodecContext->width,
                           avCodecContext->height);

            //fix buffer
            I420ToARGB(
                    yuv_frame->data[0], yuv_frame->linesize[0],
                    yuv_frame->data[2], yuv_frame->linesize[2],
                    yuv_frame->data[1], yuv_frame->linesize[1],
                    rgba_frame->data[0], rgba_frame->linesize[0],
                    avCodecContext->width, avCodecContext->height);

            //UNLOCK
            ANativeWindow_unlockAndPost(nativeWindow);
//            ANativeWindow_release(nativeWindow);
            usleep(16 * 1000);
        }
        av_free_packet(packet);
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(yuv_frame);
    av_frame_free(rgba_frame);
    avcodec_close(avCodec);
    avcodec_free_context(avCodecContext);
    avformat_free_context(formatContext);
    (*env)->ReleaseStringUTFChars(env, jstr_input, cstr_input);

}

//nb_streams,视频文件中可能存在音频流，视频流，字幕
#define MAX_AV_STREAM 2
struct Player {
    //封装格式上下文
    AVFormatContext *input_av_format_ctx;
    //音视频流索引位置
    int video_stream_index;
    int audio_stream_index;
    //解码器上下文,每个AVStream对应一个解码器上下文
    AVCodecContext *input_av_codec_ctx[MAX_AV_STREAM];
    //解码线程id
    pthread_t *decode_threads[MAX_AV_STREAM];
    ANativeWindow *nativeWindow;
};

void rendee_video(JNIEnv const *env, const void *surface, const AVFormatContext *formatContext,
                  const AVCodecContext *videoAvCodecContext, AVFrame **yuv_frame,
                  AVFrame **rgba_frame, ANativeWindow **nativeWindow);

void init_input_format_ctx(struct Player *player, const char *input_cstr) {

    //注册ffmpeg 所有组件
    av_register_all();
    //封装格式上下文
    AVFormatContext *format_ctx = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&format_ctx, input_cstr, NULL, NULL) != 0) {
        LOGE("无法打开视频文件");
        return;
    }
    //获取视频文件信息
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        LOGE("获取视频文件信息失败");
        return;
    }
    //获取视频流的索引位置
    //遍历所有类型的流（音频流，视频流、字幕流）
    int i;
    int v_stream_index = -1;
    for (i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            player->video_stream_index = i;
        } else if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            player->audio_stream_index = i;
        }
    }
    if (player->video_stream_index < 0 && player->audio_stream_index < 0) {
        LOGE("%s", "找不到流\n");
        return;
    }
    player->input_av_format_ctx = format_ctx;
}


void init_codec_context(struct Player *player, int stream_index) {
    AVFormatContext *formatContext = player->input_av_format_ctx;
    //获取视频流中的编解码的上下文
    AVCodecContext *avCodecContext = formatContext->streams[stream_index]->codec;
    //根据视频编解码上下文的id得到对应的编解码器

    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("未找到解码器");
        return;
    }
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("打开解码器失败");
        return;
    }

    //输出视频信息
    LOGI("文件格式：%s", formatContext->iformat->name);
    //formatContext->duration单位为微妙
    LOGI("文件时长：%lld", (formatContext->duration) / 1000000);
    LOGI("文件的宽度和高度 W：%d ,H：%d", avCodecContext->width, avCodecContext->height);
    LOGI("文件解码器名称：%s", avCodec->name);
    player->input_av_codec_ctx[stream_index] = avCodecContext;
}


/**
 * 解码视频准备，获取nativeWindow
 * @param env  JNIEnv
 * @param surface
 * @param player
 */
void decode_video_prepare(JNIEnv *env, jobject surface, struct Player *player) {
    player->nativeWindow = ANativeWindow_fromSurface(env, surface);//准备读取
}

/**
 * 视频解码
 * @param player
 */
void decode_video(struct Player *player, AVPacket *packet) {
    LOGI("解码开始");
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgba_frame = av_frame_alloc();
    AVCodecContext *codec_ctx = player->input_av_codec_ctx[player->video_stream_index];

    //缓冲区buffer
    ANativeWindow_Buffer windowBuffer;
    int len, got_frame, frame_count;
    len = avcodec_decode_video2(codec_ctx, yuv_frame, &got_frame, packet);
    if(len<0){
        av_frame_free(yuv_frame);
        av_frame_free(rgba_frame);
        LOGI("解码len<0");
        return;
    }
    //不为0,正在解码
    if (got_frame) {
        ANativeWindow_setBuffersGeometry(player->nativeWindow, codec_ctx->width,
                                         codec_ctx->height, WINDOW_FORMAT_RGBA_8888);
        //LOCK
        ANativeWindow_lock(player->nativeWindow, &windowBuffer, NULL);

        avpicture_fill((AVPicture *) rgba_frame, windowBuffer.bits, PIX_FMT_RGBA,
                       codec_ctx->width,
                       codec_ctx->height);

        //fix buffer
        I420ToARGB(
                yuv_frame->data[0], yuv_frame->linesize[0],
                yuv_frame->data[2], yuv_frame->linesize[2],
                yuv_frame->data[1], yuv_frame->linesize[1],
                rgba_frame->data[0], rgba_frame->linesize[0],
                codec_ctx->width, codec_ctx->height);

        //UNLOCK
        ANativeWindow_unlockAndPost(player->nativeWindow);
//            ANativeWindow_release(nativeWindow);
        usleep(16 * 1000);
    }
    av_frame_free(yuv_frame);
    av_frame_free(rgba_frame);
}

void *decode_data_thr_fun(void *arg) {
    LOGI("%s", "开始子线程解码");
    struct Player *player = (struct Player *) arg;
    //AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));//Native 绘制
    int video_frame_count = 0;
    int audio_frame_count = 0;
    while (av_read_frame(player->input_av_format_ctx, packet) >= 0) {
        if (packet->stream_index == player->video_stream_index) {
            decode_video(player, packet);
            LOGI("video_frame_count:%d", video_frame_count++);
        } else if (packet->stream_index == player->audio_stream_index) {
            LOGI("audio_frame_count:%d", audio_frame_count++);
            usleep(16 * 1000);
        }
        av_free_packet(packet);
        LOGI("av_free_packet");
    }
    ANativeWindow_release(player->nativeWindow);
}


void JNICALL Java_cn_onestravel_ndk_ffmpeg_render_VideoPlayer_play
        (JNIEnv *env, jclass jcls, jstring jstr_input, jobject surface) {
    const char *cstr_input = (*env)->GetStringUTFChars(env, jstr_input, NULL);
    struct Player *player = (struct Player *) malloc(sizeof(struct Player));
    //初始化封装格式上下文
    init_input_format_ctx(player, cstr_input);
    int video_stream_index = player->video_stream_index;
    int audio_stream_index = player->audio_stream_index;
    init_codec_context(player, audio_stream_index);
    init_codec_context(player, video_stream_index);
    LOGI("%s", "初始化完成");
    AVFormatContext *formatContext = player->input_av_format_ctx;
    AVCodecContext *videoAvCodecContext = player->input_av_codec_ctx[video_stream_index];
    AVCodecContext *audioAvCodecContext = player->input_av_codec_ctx[audio_stream_index];
    decode_video_prepare(env, surface, player);
    //创建子线程解码
    LOGI("%s,%d,%d", "创建子线程解码", video_stream_index, audio_stream_index);
    pthread_create(&player->decode_threads[video_stream_index], NULL, decode_data_thr_fun,
                   (void *) player);

    LOGI("%s,%d,%d", "创建子线程解码完成", video_stream_index, audio_stream_index);
    /*ANativeWindow_release(nativeWindow);
    av_frame_free(yuv_frame);
    av_frame_free(rgba_frame);
//    avcodec_close(avCodec);
    avcodec_free_context(videoAvCodecContext);
    avformat_free_context(formatContext);
    (*env)->ReleaseStringUTFChars(env, jstr_input, cstr_input);
    free(player);*/
}
