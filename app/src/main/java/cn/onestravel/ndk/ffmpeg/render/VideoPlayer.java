package cn.onestravel.ndk.ffmpeg.render;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;
import android.view.Surface;

/**
 * @author onestravel
 * @version 1.0.0
 * @name cn.onestravel.ndk.ffmpeg.render.VideoPlayer
 * @description //TODO
 * @createTime 2019/3/28 15:43
 */
public class VideoPlayer {
    private PlayListener playListener;

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

//    public native void prepare(String input);

    public native void play(String input,Surface surface);

    public void setPlayListener(PlayListener playListener) {
        this.playListener = playListener;
    }

    public void ready(int width, int height){
        if(playListener!=null){
            playListener.ready(width,height);
        }
    }
//    public native void destroy(String input, Surface surface);

    /**
     * 创建一个AudioTrac对象，用于播放
     * @param nb_channels
     * @return
     */
    public AudioTrack createAudioTrack(int sampleRateInHz, int nb_channels){
        //固定格式的音频码流
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        Log.i("onestravel", "nb_channels:"+nb_channels);
        //声道布局
        int channelConfig;
        if(nb_channels == 1){
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_MONO;
        }else if(nb_channels == 2){
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_STEREO;
        }else{
            channelConfig = android.media.AudioFormat.CHANNEL_OUT_STEREO;
        }

        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
        AudioTrack audioTrack;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
            AudioTrack.Builder builder = new AudioTrack.Builder();
            builder.setAudioAttributes(new AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_ALARM)
                    .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                    .setLegacyStreamType(AudioManager.STREAM_MUSIC)
                    .build())
                    .setAudioFormat(new AudioFormat.Builder()
                            .setEncoding(audioFormat)
                            .setSampleRate(sampleRateInHz)
                            .setChannelMask(channelConfig)
                            .build())
                    .setBufferSizeInBytes(bufferSizeInBytes);
            audioTrack = builder.build();
        }else {
            audioTrack = new AudioTrack(
                    AudioManager.STREAM_MUSIC,
                    sampleRateInHz, channelConfig,
                    audioFormat,
                    bufferSizeInBytes, AudioTrack.MODE_STREAM);
        }
        //播放
        //audioTrack.play();
        //写入PCM
        //audioTrack.write(audioData, offsetInBytes, sizeInBytes);
        return audioTrack;
    }


    interface PlayListener{
        void ready(int width,int height);
    }
}
