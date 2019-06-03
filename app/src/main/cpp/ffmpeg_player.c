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
#include <libswresample/swresample.h>
#include <libavutil/time.h>
//队列
#include "quene.h"


#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFMPEG",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFMPEG",FORMAT,##__VA_ARGS__);


/*
 * Class:     cn_onestravel_ndk_ffmpeg_render_VideoPlayer
 * Method:    render
 * Signature: (Ljava/lang/String;Landroid/view/Surface;)V
 */
JNIEXPORT void JNICALL Java_cn_onestravel_ndk_ffmpeg_render_VideoPlayer_render
        (JNIEnv *env, jclass jcls, jstring jstr_input, jobject surface) {
//    const char *cstr_input = (*env)->GetStringUTFChars(env, jstr_input, NULL);
//    //注册ffmpeg 所有组件
//    av_register_all();
//    //封装格式上下文
//    AVFormatContext *formatContext = avformat_alloc_context();
//    //打开输入视频文件
//    if (avformat_open_input(&formatContext, cstr_input, NULL, NULL) != 0) {
//        LOGE("无法打开视频文件");
//        return;
//    }
//    //获取视频文件信息
//    if (avformat_find_stream_info(formatContext, NULL) < 0) {
//        LOGE("获取视频文件信息失败");
//        return;
//    }
//    //获取视频流的索引位置
//    //遍历所有类型的流（音频流，视频流、字幕流）
//    int i = 0;
//    int v_stream_index = -1;
//    for (; i < formatContext->nb_streams; i++) {
//        if (formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
//            v_stream_index = i;
//            break;
//        }
//    }
//    if (v_stream_index < 0) {
//        LOGE("%s", "找不到视频流\n");
//        return;
//    }
//    //获取视频流中的编解码的上下文
//    AVCodecContext *avCodecContext = formatContext->streams[v_stream_index]->codec;
//    //根据视频编解码上下文的id得到对应的编解码器
//
//    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
//    if (avCodec == NULL) {
//        LOGE("未找到解码器");
//        return;
//    }
//    //打开解码器
//    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
//        LOGE("打开解码器失败");
//        return;
//    }
//    //输出视频信息
//    LOGI("视频文件格式：%s", formatContext->iformat->name);
//    //formatContext->duration单位为微妙
//    LOGI("视频时长：%lld", (formatContext->duration) / 1000000);
//    LOGI("视频的宽度和高度 W：%d ,H：%d", avCodecContext->width, avCodecContext->height);
//    LOGI("视频解码器名称：%s", avCodec->name);
//    //准备读取
//    //AVPacket用于存储一帧一帧的压缩数据（H264）
//    //缓冲区，开辟空间
//    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
//    AVFrame *yuv_frame = av_frame_alloc();
//    AVFrame *yuv_scale_frame = av_frame_alloc();
//    AVFrame *rgba_frame = av_frame_alloc();
//    //Native 绘制
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
//    //缓冲区buffer
//    ANativeWindow_Buffer windowBuffer;
//
//    int len, got_frame, frame_count = 0;
//    while (av_read_frame(formatContext, packet) >= 0) {
//        len = avcodec_decode_video2(avCodecContext, yuv_frame, &got_frame, packet);
//        //不为0,正在解码
//        if (got_frame) {
//            int i = frame_count++;
//            LOGI("解码%d帧", i);
//            ANativeWindow_setBuffersGeometry(nativeWindow, avCodecContext->width,
//                                             avCodecContext->height, WINDOW_FORMAT_RGBA_8888);
//            //LOCK
//            ANativeWindow_lock(nativeWindow, &windowBuffer, NULL);
//
//            avpicture_fill((AVPicture *) rgba_frame, windowBuffer.bits, PIX_FMT_RGBA,
//                           avCodecContext->width,
//                           avCodecContext->height);
//
//            //fix buffer
//            I420ToARGB(
//                    yuv_frame->data[0], yuv_frame->linesize[0],
//                    yuv_frame->data[2], yuv_frame->linesize[2],
//                    yuv_frame->data[1], yuv_frame->linesize[1],
//                    rgba_frame->data[0], rgba_frame->linesize[0],
//                    avCodecContext->width, avCodecContext->height);
//
//            //UNLOCK
//            ANativeWindow_unlockAndPost(nativeWindow);
////            ANativeWindow_release(nativeWindow);
//            usleep(16 * 1000);
//        }
//        av_free_packet(packet);
//    }
//    ANativeWindow_release(nativeWindow);
//    av_frame_free(&yuv_frame);
//    av_frame_free(&rgba_frame);
//    avcodec_free_context(&avCodecContext);
//    avcodec_close(avCodec);
//    avformat_free_context(formatContext);
//    (*env)->ReleaseStringUTFChars(env, jstr_input, cstr_input);

}

#define MIN_SLEEP_TIME_US 1000ll
#define AUDIO_TIME_ADJUST_US -200000ll

//nb_streams,视频文件中可能存在音频流，视频流，字幕
#define MAX_AV_STREAM 2
#define PACKET_QUEUE_SIZE 50
struct _Player {
    JavaVM *javaVM;
    //封装格式上下文
    AVFormatContext *input_av_format_ctx;
    int capture_stream_no;
    //音视频流索引位置
    int video_stream_index;
    int audio_stream_index;
    //解码器上下文,每个AVStream对应一个解码器上下文
    AVCodecContext *input_av_codec_ctx[MAX_AV_STREAM];
    //解码线程id
    pthread_t *decode_threads[MAX_AV_STREAM];
    //视频绘制所需的window
    ANativeWindow *nativeWindow;
    //输入的采样格式
    enum AVSampleFormat in_sample_fmt;
    //输出采样格式16bit PCM
    enum AVSampleFormat out_sample_fmt;
    //输入采样率
    int in_sample_rate;
    //输出采样率
    int out_sample_rate;
    //根据输入的声道格式
    int out_channel_nb;
    //重采样结构体上下文
    SwrContext *swrCtx;
    //Android中 AudioTrack.java 对象
    jobject audio_track;
    //Android中 AudioTrack.java 对象 中的 write 方法ID
    jmethodID audio_track_write_mid;

    //生产者线程
    pthread_t thread_player_read_from_stream;
    //音频，视频队列数组
    Queue *packets[MAX_AV_STREAM];

    //互斥锁
    pthread_mutex_t mutex;
    //条件变量
    pthread_cond_t cond;

    //视频开始播放的时间
    int64_t start_time;

    int64_t audio_clock;
};


typedef struct _Player Player;

struct _DecodeData {
    Player *player;
    int stream_index;
};

typedef struct _DecodeData DecodeData;

/**
 * 初始化封装格式上下文
 * @param player 需要初始化的结构体指针
 * @param input_cstr 文件路径的c字符串
 */
void init_input_format_ctx(Player *player, const char *input_cstr) {

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
    //获取流的总个数
    player->capture_stream_no = format_ctx->nb_streams;
    LOGI("capture_stream_no:%d", player->capture_stream_no);
    //获取视频流的索引位置
    //遍历所有类型的流（音频流，视频流、字幕流）
    int i;
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


/**
 * 初始化音视频解码器上下文
 * @param player 需要初始化结构体指针变量
 * @param stream_index 对应的音视频流索引位置
 */
void init_codec_context(Player *player, int stream_index) {
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
void decode_video_prepare(JNIEnv *env, jobject surface, Player *player) {
    player->nativeWindow = ANativeWindow_fromSurface(env, surface);//准备读取
}

/**
 * JNI 视频相关初始化
 * @param env  JNIEnv
 * @param player
 */
void jni_video_prepare(JNIEnv *env, jobject jthiz, Player *player) {
    jclass player_class = (*env)->GetObjectClass(env, jthiz);

    jmethodID ready_mid = (*env)->GetMethodID(env, player_class, "ready",
                                                           "(II)V");
    AVCodecContext *avCodecContext = player->input_av_codec_ctx[player->video_stream_index];
    LOGI("ready width= %d ,height = %d",avCodecContext->width, avCodecContext->height);
     (*env)->CallVoidMethod(env, jthiz, ready_mid,
                              avCodecContext->width, avCodecContext->height);

}

/**
 * 获取视频当前播放时间
 */
int64_t player_get_current_video_time(Player *player) {
    int64_t current_time = av_gettime();
    return current_time - player->start_time;
}

/**
 * 延迟
 */
void player_wait_for_frame(Player *player, int64_t stream_time,
                           int stream_no) {
    pthread_mutex_lock(&player->mutex);
    for (;;) {
        int64_t current_video_time = player_get_current_video_time(player);
        int64_t sleep_time = stream_time - current_video_time;
        LOGI("sleep_time=%d", sleep_time);
        if (sleep_time < -300000ll) {
            // 300 ms late
            int64_t new_value = player->start_time - sleep_time;
            LOGI("player_wait_for_frame[%d] correcting %f to %f because late",
                 stream_no, (av_gettime() - player->start_time) / 1000000.0,
                 (av_gettime() - new_value) / 1000000.0);

            player->start_time = new_value;
            pthread_cond_broadcast(&player->cond);
        }

        if (sleep_time <= MIN_SLEEP_TIME_US) {
            // We do not need to wait if time is slower then minimal sleep time
            LOGI("sleep_time<= MIN_SLEEP_TIME_US");
            break;
        }

        if (sleep_time > 500000ll) {
            // if sleep time is bigger the
            // n 500ms just sleep this 500ms
            // and check everything again
            LOGI("sleep_time> 500000ll");
            sleep_time = 500000ll;
        }
        //等待指定时长
        int timeout_ret = pthread_cond_timeout_np(&player->cond,
                                                  &player->mutex, sleep_time / 1000ll);
        LOGI("sleep_time / 1000ll = %f , timeout_ret = %d", sleep_time / 1000ll, timeout_ret);
        // just go further
        LOGI("player_wait_for_frame[%d] finish", stream_no);
    }
    pthread_mutex_unlock(&player->mutex);
}

/**
 * 解码音频准备
 * @param env  JNIEnv
 * @param player
 */
void decode_audio_prepare(JNIEnv *env, Player *player) {
    AVCodecContext *codecCtx = player->input_av_codec_ctx[player->audio_stream_index];
    //重采样设置参数
    //输入的采样格式
    player->in_sample_fmt = codecCtx->sample_fmt;
    //输出采样格式16bit PCM
    player->out_sample_fmt = AV_SAMPLE_FMT_S16;
    //输入采样率
    player->in_sample_rate = codecCtx->sample_rate;
    //输出采样率
    player->out_sample_rate = player->in_sample_rate;
    //根据输入的声道布局
    //根据声道个数获取默认的声道布局（2个声道，默认立体声stereo）
    uint64_t in_ch_layout = codecCtx->channel_layout;
    //输出的声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    player->swrCtx = swr_alloc();
    swr_alloc_set_opts(player->swrCtx,
                       out_ch_layout, player->out_sample_fmt, player->out_sample_rate,
                       in_ch_layout, player->in_sample_fmt, player->in_sample_rate,
                       0, NULL);

    swr_init(player->swrCtx);
    //输出声道个数
    player->out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
}

/**
 * JNI 音频相关初始化
 * @param env  JNIEnv
 * @param player
 */
void jni_audio_prepare(JNIEnv *env, jobject jthiz, Player *player) {
    jclass player_class = (*env)->GetObjectClass(env, jthiz);

    //AudioTrack对象，通过调用java方法获取AudioTrack对象
    jmethodID create_audio_track_mid = (*env)->GetMethodID(env, player_class, "createAudioTrack",
                                                           "(II)Landroid/media/AudioTrack;");
    jobject audio_track = (*env)->CallObjectMethod(env, jthiz, create_audio_track_mid,
                                                   player->out_sample_rate, player->out_channel_nb);

    //调用AudioTrack.play方法
    jclass audio_track_class = (*env)->GetObjectClass(env, audio_track);
    jmethodID audio_track_play_mid = (*env)->GetMethodID(env, audio_track_class, "play", "()V");
    (*env)->CallVoidMethod(env, audio_track, audio_track_play_mid);
    //AudioTrack.write 方法
    jmethodID audio_track_write_mid = (*env)->GetMethodID(env, audio_track_class, "write",
                                                          "([BII)I");
    LOGI("audio_track=%#x", &audio_track);
    LOGI("audio_track_write_mid=%#x", &audio_track_write_mid);

    player->audio_track_write_mid = audio_track_write_mid;
    //声明称全局变量，解决出现使用了已删除的引用的问题
    player->audio_track = (*env)->NewGlobalRef(env, audio_track);

}

/**
 * 视频解码
 * @param player
 */
void decode_video(Player *player, AVPacket *packet) {
    LOGI("视频解码开始");
    AVStream *stream = player->input_av_format_ctx->streams[player->video_stream_index];
    //初始化视频帧，需要使用yuv视频帧和rgba视频帧
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgba_frame = av_frame_alloc();
    //视频解码器
    AVCodecContext *codec_ctx = player->input_av_codec_ctx[player->video_stream_index];

    //缓冲区buffer
    ANativeWindow_Buffer windowBuffer;
    int len, got_frame;
    //视频解码到yuv视频帧
    len = avcodec_decode_video2(codec_ctx, yuv_frame, &got_frame, packet);
    //不为0,正在解码
    if (got_frame) {
        //设置缓冲区属性
        ANativeWindow_setBuffersGeometry(player->nativeWindow, codec_ctx->width,
                                         codec_ctx->height, WINDOW_FORMAT_RGBA_8888);

        //LOCK
        ANativeWindow_lock(player->nativeWindow, &windowBuffer, NULL);

        //视频yuv帧填充
        avpicture_fill((AVPicture *) rgba_frame, windowBuffer.bits, PIX_FMT_RGBA,
                       codec_ctx->width,
                       codec_ctx->height);

        //fix buffer yuv帧转换为RGBA帧
        I420ToARGB(
                yuv_frame->data[0], yuv_frame->linesize[0],
                yuv_frame->data[2], yuv_frame->linesize[2],
                yuv_frame->data[1], yuv_frame->linesize[1],
                rgba_frame->data[0], rgba_frame->linesize[0],
                codec_ctx->width, codec_ctx->height);

        //计算延迟
        int64_t pts = av_frame_get_best_effort_timestamp(yuv_frame);
        //转换（不同时间基时间转换）
        int64_t time = av_rescale_q(pts, stream->time_base, AV_TIME_BASE_Q);

        player_wait_for_frame(player, time, player->video_stream_index);


        //UNLOCK
        ANativeWindow_unlockAndPost(player->nativeWindow);
//            ANativeWindow_release(nativeWindow);
        //延迟时间，时间太长会导致声音出现卡顿
        usleep(5 * 1000);
    }
    av_frame_free(&yuv_frame);
    av_frame_free(&rgba_frame);
}


/**
 * 音频解码
 * @param player
 */
//32位PCM数据 大小（int类型）
#define MAX_AUDIO_FRME_SIZE 48000 * 4


void decode_audio(Player *player, AVPacket *packet) {
    LOGI("音频解码开始");
    AVCodecContext *codecCtx = player->input_av_codec_ctx[player->audio_stream_index];
    AVFormatContext *input_format_ctx = player->input_av_format_ctx;
    AVStream *stream = input_format_ctx->streams[player->audio_stream_index];
    SwrContext *swrCtx = player->swrCtx;
    AVFrame *frame = av_frame_alloc();
    int got_frame;
    //codecCtx=0xab9de050,frame=0xabe25ba0,got_frame=0xe0e1889c,packet=0xe0f178b8
    LOGI("音频解码 codecCtx=%#x,frame=%#x,got_frame=%#x,packet=%#x", codecCtx, frame, &got_frame,
         packet);
    avcodec_decode_audio4(codecCtx, frame, &got_frame, packet);
    //16Bit 44100 PCM 数据（重采样缓冲区）
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRME_SIZE);
    if (got_frame > 0) {
        LOGI("音频解码%d,player->audio_stream_index=%d", got_frame, player->audio_stream_index);
        swr_convert(swrCtx, &out_buffer, MAX_AUDIO_FRME_SIZE, (const uint8_t **) frame->data,
                    frame->nb_samples);
        LOGI("音频解码swr_convert");
        //获取sample的size
        int out_buffer_size = av_samples_get_buffer_size(NULL, player->out_channel_nb,
                                                         frame->nb_samples, player->out_sample_fmt,
                                                         1);

        int64_t pts = packet->pts;
        if (pts != AV_NOPTS_VALUE) {
            player->audio_clock = av_rescale_q(pts, stream->time_base, AV_TIME_BASE_Q);
            //				av_q2d(stream->time_base) * pts;
            LOGI("player_write_audio - read from pts");
            player_wait_for_frame(player,
                                  player->audio_clock + AUDIO_TIME_ADJUST_US,
                                  player->audio_stream_index);
        }
//        fwrite(out_buffer,1,out_buffer_size,fp_pcm);
        LOGI("音频解码%d,sample的size=%d", got_frame, out_buffer_size);
//        if (out_buffer_size > 0) {
        //关联当前线程 JNIEnv
        LOGI("音频解码,关联当前线程 JNIEnv");
        JavaVM *javaVM = player->javaVM;
        JNIEnv *env;
        (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);
        //out_buffer缓冲区数据，转成byte数组
        LOGI("音频解码,out_buffer缓冲区数据，转成byte数组");
        jbyteArray audio_sample_array = (*env)->NewByteArray(env, out_buffer_size);
        jbyte *sample_byte_p = (*env)->GetByteArrayElements(env, audio_sample_array, NULL);
        //out_buffer 的数据复制到sample_byte_p
        LOGI("音频解码 memcpy out_buffer_size=%d", out_buffer_size);
        memcpy(sample_byte_p, out_buffer, out_buffer_size);
        //同步
        LOGI("音频解NewByteArray码 释放资源");
        (*env)->ReleaseByteArrayElements(env, audio_sample_array, sample_byte_p, 0);
        //AudioTrack.write PCM数据
        (*env)->CallIntMethod(env, player->audio_track, player->audio_track_write_mid,
                              audio_sample_array, 0, out_buffer_size);


        LOGI("音频解 ReleaseByteArrayElements");
        (*env)->DeleteLocalRef(env, audio_sample_array);
        LOGI("音频解 DeleteLocalRef");
        (*javaVM)->DetachCurrentThread(javaVM);
        LOGI("音频解 DetachCurrentThread");
        usleep(1000 * 16);
//        }
    }
    av_frame_free(&frame);
}

/**
 * 子线程中执行的方法
 * @param arg
 * @return
 */
void *decode_data_thr_fun(void *arg) {
    LOGI("%s", "开始子线程解码");
    DecodeData *decodeData = (DecodeData *) arg;
    Player *player = decodeData->player;
    int stream_index = decodeData->stream_index;
    //AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
    Queue *queue = player->packets[stream_index];
    int video_frame_count = 0;
    int audio_frame_count = 0;
    //循环读取帧
    for (;;) {
        pthread_mutex_lock(&player->mutex);
        //消费队列中的 AVPacket
        AVPacket *packet = (AVPacket *) queue_dequeue(queue, &player->mutex, &player->cond);
        pthread_mutex_unlock(&player->mutex);
        //判断是视频流还是音频流
        if (stream_index == player->video_stream_index) {
            decode_video(player, packet);
            LOGI("video_frame_count:%d", video_frame_count++);
        } else if (stream_index == player->audio_stream_index) {
            decode_audio(player, packet);
            LOGI("audio_frame_count:%d", audio_frame_count++);
        }
//        释放packet
//        av_free_packet(packet);
//        LOGI("av_free_packet");
    }
    //刷新windows
    ANativeWindow_release(player->nativeWindow);
}


void *player_fill_packet() {
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    return packet;
}

/**
 * 初始化音频，视频AVPacket队列,默认大小为50
 */
void player_alloc_queues(Player *player) {
    int i;
    for (i = 0; i < player->capture_stream_no; ++i) {
        Queue *queue = queue_init(PACKET_QUEUE_SIZE, player_fill_packet);
        player->packets[i] = queue;
    }
}

/**
 * 生产者线程：负责不断读取视频文件中的AVPacket，分别放入两个队列中
 * @param arg
 * @return
 */
void *player_read_from_stream(void *arg) {
    Player *player = (Player *) arg;
    int ret;
    //栈内存上保存一个AVPacket,可自动被回收
    AVPacket packet, *pkt = &packet;
    for (;;) {
        ret = av_read_frame(player->input_av_format_ctx, pkt);
        if (ret < 0) {
            //到文件结尾
            break;
        }
        //根据AVPacket->stream_index获取对应的队列
        Queue *queue = player->packets[pkt->stream_index];
        pthread_mutex_lock(&player->mutex);
        //AVPacket 进队列
        AVPacket *packet_data = queue_enqueue(queue, &player->mutex, &player->cond);
        pthread_mutex_unlock(&player->mutex);
        *packet_data = packet;
    }
}

void JNICALL Java_cn_onestravel_ndk_ffmpeg_render_VideoPlayer_play
        (JNIEnv *env, jobject jobj, jstring jstr_input, jobject surface) {
    //JNI数据类型转为C 数据类型
    const char *cstr_input = (*env)->GetStringUTFChars(env, jstr_input, NULL);
    //定义初始化结构体类型
    Player *player = (Player *) malloc(sizeof(Player));
    //初始化JavaVM
    (*env)->GetJavaVM(env, &(player->javaVM));
    //初始化封装格式上下文
    init_input_format_ctx(player, cstr_input);
    //获取流索引位置
    int video_stream_index = player->video_stream_index;
    int audio_stream_index = player->audio_stream_index;
    //初始化音频解码器上下文
    init_codec_context(player, audio_stream_index);
    //初始化视频解码器上下文
    init_codec_context(player, video_stream_index);
    jni_video_prepare(env,jobj,player);
    //视频解码准备
    decode_video_prepare(env, surface, player);
    //音频解码准备
    decode_audio_prepare(env, player);
    //JNI调用Android方法准备
    jni_audio_prepare(env, jobj, player);

    player_alloc_queues(player);
    //初始化互斥锁
    pthread_mutex_init(&player->mutex, NULL);
    //初始化条件变量
    pthread_cond_init(&player->cond, NULL);

    //生产者线程
    pthread_create(&(player->thread_player_read_from_stream), NULL, player_read_from_stream,
                   player);
    sleep(1);
    player->start_time = 0;
    DecodeData data1 = {player, video_stream_index}, *video_decode_data = &data1;
    //创建子线程解码视频
    pthread_create(&(player->decode_threads[video_stream_index]), NULL, decode_data_thr_fun,
                   (void *) video_decode_data);
    DecodeData data2 = {player, audio_stream_index}, *audio_decode_data = &data2;
    //创建子线程解码音频
    pthread_create(&(player->decode_threads[audio_stream_index]), NULL, decode_data_thr_fun,
                   (void *) audio_decode_data);
    //将线程加入等待
    pthread_join(player->thread_player_read_from_stream, NULL);
    pthread_join(player->decode_threads[video_stream_index], NULL);
    pthread_join(player->decode_threads[audio_stream_index], NULL);
}
