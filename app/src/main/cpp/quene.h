//
// Created by Administrator on 2019/4/29.
//

#ifndef FFMPEGDECODEDEMO_QUENE_H
#define FFMPEGDECODEDEMO_QUENE_H
#include <malloc.h>

struct _Queue {
    //长度
    int size;
    //存储任意类型指针数组，总共size个，每一个元素都是 node指针
    void **node;
    //push或者pop元素时需要安照先后顺序，依次执行
    int next_to_write;
    int next_to_read;
};
typedef struct _Queue Queue;

///分配队列中元素的内存的函数
typedef void* (*queue_fill_func)();

//释放队列中元素所占用的空间
typedef void* (*queue_free_func)(void* elem);


/**
 * 初始化队列
 * @param size
 * @return
 */
Queue *queue_init(int size,queue_fill_func fill_func);

/**
 * 销毁队列
 */
void queue_free(Queue *queue,queue_free_func free_func) ;

/**
 * 获取下一个索引位置
 * @return
 */
int queue_get_next(Queue* queue, int current);

/**
 * 入队列
 * @return
 */
void *queue_enqueue(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) ;

/**
 * 出队列
 * @return
 */
void *queue_dequeue(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) ;
#endif //FFMPEGDECODEDEMO_QUENE_H
