#include <pthread.h>
#include <android/log.h>
#include "quene.h"


#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"FFMPEG",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"FFMPEG",FORMAT,##__VA_ARGS__);

/**
 * 初始化队列
 * @param size
 * @return
 */
Queue* queue_init(int size,queue_fill_func fill_func) {
    Queue *queue = malloc(sizeof(Queue));
    queue->size = size;
    queue->next_to_write = 0;
    queue->next_to_read = 0;
    //数组开辟空间
    queue->node = malloc(sizeof(*queue->node) * size);
    //给数组元素开辟空间
    int i;
    for (i = 0; i < size; i++) {
        queue->node[i] = fill_func();
    }
    return queue;
}

/**
 * 销毁队列
 */
void queue_free(Queue *queue,queue_free_func free_func) {
    int i;
    for (i = 0; i < queue->size; i++) {
        free_func((void*)queue->node[i]);
    }
    free(queue->node);
    free(queue);
}

/**
 * 获取下一个索引位置
 * @return
 */
int queue_get_next(Queue* queue, int current){
    return (current + 1) % queue->size;;
}

/**
 * 入队列
 * @return
 */
void *queue_enqueue(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) {
    int current = queue->next_to_write;
    int next_to_write;
    for (; ;) {
        //下一个要读的位置等于下一个要写的位置，等写完，再读
        next_to_write = queue_get_next(queue,current);
        LOGI("queue_enqueue  next_to_write=%d,queue->next_to_read=%d",next_to_write,queue->next_to_read);
        if(next_to_write != queue->next_to_read){
            break;
        }
        //阻塞
        pthread_cond_wait(cond,mutex);
    }
    queue->next_to_write = next_to_write;
    //通知消费线程可以继续操作
    pthread_cond_broadcast(cond);
    return queue->node[current];
}

/**
 * 出队列
 * @return
 */
void *queue_dequeue(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) {
    int current = queue->next_to_read;
    for(;;){
        LOGI("queue_dequeue  queue->next_to_write=%d,queue->next_to_read=%d",queue->next_to_write,queue->next_to_read);
        if(queue->next_to_read!=queue->next_to_write){
            break;
        }
        pthread_cond_wait(cond,mutex);
    }
    queue->next_to_read = queue_get_next(queue,current);
    //通知其他等待线程可以继续操作
    pthread_cond_broadcast(cond);
    return queue->node[current];
}