package cn.onestravel.ndk.ffmpeg.render;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

/**
 * @author onestravel
 * @version 1.0.0
 * @name cn.onestravel.ndk.ffmpeg.render.VideoView
 * @description //TODO
 * @createTime 2019/3/29 15:39
 */
public class VideoView extends SurfaceView {
    public static final float SHOW_SCALE = 16 * 1.0f / 9;

    private SurfaceView mSurfaceView;
    private SurfaceHolder mHolder;

    //屏幕宽度
    private int mScreenWidth;
    //屏幕高度
    private int mScreenHeight;
    //记录现在的播放位置
    private int mCurrentPos;
    private boolean isLand;

    private int mediaWidth;
    private int mediaHeight;


    public VideoView(Context context) {
        super(context);
        init();
    }

    public VideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public VideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public void init() {
        //初始化SurfaceView的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
        holder.setKeepScreenOn(true);
        holder.setFixedSize(960,450);
        DisplayMetrics dm = new DisplayMetrics();
        dm = getContext().getResources().getDisplayMetrics();
        mScreenWidth = dm.widthPixels;
        mScreenHeight = dm.heightPixels;
        this.mediaWidth = mScreenWidth;
        this.mediaHeight = mScreenHeight;

    }


    public void resetSize(int mediaWidth,int mediaHeight) {
        this.mediaWidth = mediaWidth;
        this.mediaHeight = mediaHeight;
        int videoWidth = mediaWidth;
        int videoHeight = mediaHeight;

        //根据视频尺寸去计算->视频可以在sufaceView中放大的最大倍数。
        float max;
        if (getResources().getConfiguration().orientation== ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) {
            //竖屏模式下按视频宽度计算放大倍数值
            max = Math.max((float) videoWidth / (float) mScreenWidth,(float) videoHeight / (float) mediaHeight);
        } else{
            //横屏模式下按视频高度计算放大倍数值
            max = Math.max(((float) videoWidth/(float) mediaHeight),(float) videoHeight/(float) mScreenWidth);
        }

        //视频宽高分别/最大倍数值 计算出放大后的视频尺寸
        videoWidth = (int) Math.ceil((float) videoWidth / max);
        videoHeight = (int) Math.ceil((float) videoHeight / max);

        //无法直接设置视频尺寸，将计算出的视频尺寸设置到surfaceView 让视频自动填充。
//        setLayoutParams(new FrameLayout.LayoutParams(videoWidth, videoHeight));
        getHolder().setFixedSize(videoWidth,videoHeight);
    }


}
