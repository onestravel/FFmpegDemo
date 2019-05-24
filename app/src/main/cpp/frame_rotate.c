//
// Created by Administrator on 2019/4/25.
//

#include "frame_rotate.h"

enum VideoRotationMode {
    kRotateNone = 0,
    kRotate90 = 90,
    kRotate180 = 180,
    kRotate270 = 270,
};


enum VideoRotationMode rotation_ = kRotateNone;


int Rotate(const AVFrame *src, AVFrame **dst) {
    if (rotation_ == kRotateNone) {
        *dst = src;
        return 0;
    }
    AVFrame *tmp = *dst;
    if (!tmp) {
        tmp = av_frame_alloc();
    }
    int width_ = src->width;
    int height_ = src->height;
// width_、height_ 为src的宽高，若是90或者270则宽高交换值
    if (tmp->width != width_ || tmp->height != height_) {
        av_frame_free(&tmp);
        tmp = av_frame_alloc();
        uint8_t *buffer = (uint8_t *) av_malloc(
                av_image_get_buffer_size(AV_PIX_FMT_YUV420P, width_, height_, 1));
        av_image_fill_arrays(tmp->data, tmp->linesize, buffer, AV_PIX_FMT_YUV420P, width_, height_,
                             1);
    }


    if (rotation_ == kRotate90) {
        Rotate90(src, tmp);
    } else if (rotation_ == kRotate180) {
        Rotate180(src, tmp);
    } else if (rotation_ == kRotate270) {
        Rotate270(src, tmp);
    } else {
        return 0;
    }


    tmp->format = src->format;
    tmp->pts = src->pts;
    tmp->pkt_pts = src->pkt_pts;
    tmp->pkt_dts = src->pkt_dts;
    tmp->key_frame = src->key_frame;


    *dst = tmp;


    return 1;
}


void Rotate90(const AVFrame *src, AVFrame *dst) {
    int half_width = src->width >> 1;
    int half_height = src->height >> 1;


    int size = src->linesize[0] * src->height;
    int half_size = size >> 2;


    for (int j = 0, n = 0; j < src->width; j++) {
        int pos = size;
        for (int i = src->height - 1; i >= 0; i--) {
            pos -= src->linesize[0];
            dst->data[0][n++] = src->data[0][pos + j];
        }
    }


    for (int j = 0, n = 0; j < half_width; j++) {
        int pos = half_size;
        for (int i = half_height - 1; i >= 0; i--) {
            pos -= src->linesize[1];
            dst->data[1][n] = src->data[1][pos + j];
            dst->data[2][n++] = src->data[2][pos + j];
        }
    }


    dst->height = src->width;
    dst->width = src->height;
}


void Rotate180(const AVFrame *src, AVFrame *dst) {
    int half_width = src->width >> 1;
    int half_height = src->height >> 1;


    int pos = src->linesize[0] * src->height;
    for (int i = 0, n = 0; i < src->height; i++) {
        pos -= src->linesize[0];
        for (int j = src->width - 1; j >= 0; j--) {
            dst->data[0][n++] = src->data[0][pos + j];
        }
    }


    pos = src->linesize[0] * src->height >> 2;
    for (int i = 0, n = 0; i < half_height; i++) {
        pos -= src->linesize[1];
        for (int j = half_width - 1; j >= 0; j--) {
            dst->data[1][n] = src->data[1][pos + j];
            dst->data[2][n++] = src->data[2][pos + j];
        }
    }


    dst->width = src->width;
    dst->height = src->height;
}


void Rotate270(const AVFrame *src, AVFrame *dst) {
    int half_width = src->linesize[0] >> 1;
    int half_height = src->height >> 1;


    for (int i = src->width - 1, n = 0; i >= 0; i--) {
        for (int j = 0, pos = 0; j < src->height; j++) {
            dst->data[0][n++] = src->data[0][pos + i];
            pos += src->linesize[0];
        }
    }


    for (int i = (src->width >> 1) - 1, n = 0; i >= 0; i--) {
        for (int j = 0, pos = 0; j < half_height; j++) {
            dst->data[1][n] = src->data[1][pos + i];
            dst->data[2][n++] = src->data[2][pos + i];
            pos += half_width;
        }
    }


    dst->width = src->height;
    dst->height = src->width;
}
