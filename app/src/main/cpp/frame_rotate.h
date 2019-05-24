//
// Created by Administrator on 2019/4/25.
//
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#ifndef FFMPEGDECODEDEMO_FRAME_ROTATE_H
#define FFMPEGDECODEDEMO_FRAME_ROTATE_H
enum VideoRotationMode;




int Rotate(const AVFrame *src, AVFrame **dst) ;


void Rotate90(const AVFrame *src, AVFrame *dst) ;


void Rotate180(const AVFrame *src, AVFrame *dst) ;


void Rotate270(const AVFrame *src, AVFrame *dst) ;

#endif //FFMPEGDECODEDEMO_FRAME_ROTATE_H
