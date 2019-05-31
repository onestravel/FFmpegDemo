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
        videoView = findViewById(R.id.videoView);
        playThread = new PlayThread(videoView);
        videoPlayer = new VideoPlayer();
    }
    

    public void play(View view) {
        if (playThread == null) {
            playThread = new PlayThread(videoView);
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
        public PlayThread(SurfaceView surfaceView){
            this.surfaceView = surfaceView;
        }
        @Override
        public void run() {
            super.run();
            VideoPlayer videoPlayer = new VideoPlayer();
            videoPlayer.setPlayListener(new VideoPlayer.PlayListener() {
                @Override
                public void ready(int width, int height) {
                    ((VideoView)surfaceView).resetSize(width,height);
                }
            });
            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/xuebao.mp4";
            videoPlayer.play(input,surfaceView.getHolder().getSurface());
        }
    }
}
