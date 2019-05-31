package cn.onestravel.ndk.ffmpeg.render;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
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

    private DisplayMetrics displayMetrics;
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
        DisplayMetrics dm = new DisplayMetrics();
        dm = getContext().getResources().getDisplayMetrics();
        mScreenWidth = dm.widthPixels;
        mScreenHeight = dm.heightPixels;


    }


    public void resetSize(int mediaWidth,int mediaHeight) {
        this.mediaWidth = mediaWidth;
        this.mediaHeight = mediaHeight;
        requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        float areaWH = 0.0f;
        int svWidth = 0;
        int svHeight = 0;
        int height;
        if (!isLand) {
            // 竖屏16:9
            height = (int) (mScreenWidth / SHOW_SCALE);
            areaWH = SHOW_SCALE;
        } else {
            //横屏按照手机屏幕宽高计算比例
            height = mScreenHeight;
            areaWH = mScreenWidth / mScreenHeight;
        }
        float mediaWH = mediaWidth * 1.0f / mediaHeight;
        if (areaWH > mediaWH) {
            //直接放会矮胖
             svWidth = (int) (height * mediaWH);
        }
        if (areaWH < mediaWH) {
            //直接放会瘦高。
             svHeight = (int) (mScreenWidth / mediaWH);
        }
        int widthModel = MeasureSpec.getMode(widthMeasureSpec);
        int heightModel = MeasureSpec.getMode(heightMeasureSpec);
        widthMeasureSpec = MeasureSpec.makeMeasureSpec(svWidth,widthModel);
        heightMeasureSpec = MeasureSpec.makeMeasureSpec(svHeight,heightModel);
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }
}
