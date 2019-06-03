package cn.onestravel.ndk.ffmpeg.render;

import android.app.Activity;
import android.os.Environment;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.View;

import cn.onestravel.ndk.ffmpeg.MainActivity;
import cn.onestravel.ndk.ffmpeg.R;
import cn.onestravel.ndk.ffmpeg.VideoUtils;

public class PlayActivity extends Activity {

    private Thread playThread;
    private VideoView videoView;
    private VideoPlayer videoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        //得到当前界面的装饰视图
        View decorView = getWindow().getDecorView();
//        SYSTEM_UI_FLAG_FULLSCREEN表示全屏的意思，也就是会将状态栏隐藏
        //设置系统UI元素的可见性
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);
        videoView = findViewById(R.id.videoView);
        playThread = new PlayThread(videoView,videoView.getHolder().getSurface());
        videoPlayer = new VideoPlayer();
    }
    

    public void play(View view) {
        if (playThread == null) {
            playThread = new PlayThread(videoView,videoView.getHolder().getSurface());
        }
        try {
            if(!playThread.isAlive()) {
                playThread.start();
            }
//            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/xuebao.mp4";
//            videoPlayer.play(input,videoView.getHolder().getSurface());
        }catch (Exception e){
            e.printStackTrace();
        }
    }


    public static class PlayThread extends Thread {
        SurfaceView surfaceView;
        Surface  surface;
        public PlayThread(SurfaceView surfaceView, Surface  surface){
            this.surfaceView = surfaceView;
            this.surface = surface;
        }
        @Override
        public void run() {
            super.run();
            final VideoPlayer videoPlayer = new VideoPlayer();
            videoPlayer.setPlayListener(new VideoPlayer.PlayListener() {
                @Override
                public void ready( final int width, final int height) {
                    surfaceView.post(new Runnable() {
                        @Override
                        public void run() {
                            ((VideoView)surfaceView).resetSize(width,height);
//                            Log.i("PlayThread","width= "+width+",height = "+height);
                        }
                    });
                }


            });
            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/xuebao.mp4";
            videoPlayer.play(input,surface);
        }
    }
}
