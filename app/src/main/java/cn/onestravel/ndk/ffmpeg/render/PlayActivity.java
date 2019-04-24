package cn.onestravel.ndk.ffmpeg.render;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.View;

import cn.onestravel.ndk.ffmpeg.MainActivity;
import cn.onestravel.ndk.ffmpeg.R;
import cn.onestravel.ndk.ffmpeg.VideoUtils;

public class PlayActivity extends AppCompatActivity {

    private Thread playThread;
    private VideoView videoView;
    private VideoPlayer videoPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        videoView = findViewById(R.id.videoView);
        playThread = new PlayThread(videoView.getHolder().getSurface());
        videoPlayer = new VideoPlayer();
    }
    

    public void play(View view) {
//        if (playThread == null) {
//            playThread = new MainActivity.VideoThread();
//        }
        try {
//            if(!playThread.isAlive()) {
//                playThread.start();
//            }
            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/input.mp4";
            videoPlayer.play(input,videoView.getHolder().getSurface());
        }catch (Exception e){
            e.printStackTrace();
        }
    }


    public static class PlayThread extends Thread {
        Surface surface;
        public PlayThread(Surface surface){
            this.surface = surface;
        }
        @Override
        public void run() {
            super.run();
            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/input.mp4";
            VideoPlayer.render(input,surface);
        }
    }
}
