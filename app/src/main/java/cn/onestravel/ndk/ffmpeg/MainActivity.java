package cn.onestravel.ndk.ffmpeg;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.widget.Toast;

import cn.onestravel.ndk.ffmpeg.render.PlayActivity;

public class MainActivity extends Activity {


    private boolean permission;
    private VideoThread videoThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        requestPermission();
        videoThread = new VideoThread();
    }


    /**
     * 获取权限
     */
    private void requestPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            String[] perms = {"android.permission.WRITE_EXTERNAL_STORAGE"};
            if (checkSelfPermission(perms[0]) == PackageManager.PERMISSION_DENIED) {
                permission = false;
                requestPermissions(perms, 200);
            } else {
                permission = true;
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 200) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                permission = true;
            }
        }
    }

    public void decode(View view) {
        if (!permission) {
            Toast.makeText(this, "请允许存储权限", Toast.LENGTH_SHORT).show();
            requestPermission();
            return;
        }
        if (videoThread == null) {
            videoThread = new VideoThread();
        }
        try {
            if(!videoThread.isAlive()) {
                videoThread.start();
            }
        }catch (Exception e){
            e.printStackTrace();
        }

    }


    @Override
    protected void onDestroy() {
        videoThread = null;
        super.onDestroy();
    }

    public void render(View view) {
        Intent intent = new Intent(this, PlayActivity.class);
        startActivity(intent);
    }

    public static class VideoThread extends Thread {
        @Override
        public void run() {
            super.run();
            String input = Environment.getExternalStorageDirectory().getAbsolutePath() + "/input.mp4";
            String output = Environment.getExternalStorageDirectory().getAbsolutePath() + "/out.yuv";
            VideoUtils.decode(input, output);
            Log.i("Activity","编码完成");
        }
    }
}
