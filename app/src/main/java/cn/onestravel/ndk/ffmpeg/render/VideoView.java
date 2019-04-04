package cn.onestravel.ndk.ffmpeg.render;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * @author onestravel
 * @version 1.0.0
 * @name cn.onestravel.ndk.ffmpeg.render.VideoView
 * @description //TODO
 * @createTime 2019/3/29 15:39
 */
public class VideoView extends SurfaceView {
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

    public void init(){
        //初始化SurfaceView的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }
}
