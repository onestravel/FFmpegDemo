package cn.onestravel.ndk.ffmpeg.render;

import android.view.Surface;

/**
 * @author onestravel
 * @version 1.0.0
 * @name cn.onestravel.ndk.ffmpeg.render.VideoPlayer
 * @description //TODO
 * @createTime 2019/3/28 15:43
 */
public class VideoPlayer {

    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("postproc-53");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("yuv_shared");
        System.loadLibrary("ffmpeg-video");
    }

    public native static void render(String input, Surface surface);

    public native static void sound(String input, Surface surface);

    public native static void play(String input, Surface surface);
}
