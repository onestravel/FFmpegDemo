package cn.onestravel.ndk.ffmpeg;


public class VideoUtils {
    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("postproc-53");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("ffmpeg-video");
    }
    public native static void decode(String input,String output);
}
