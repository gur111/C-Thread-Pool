#ifndef _THPOOL_
#define _THPOOL_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Task {
    void (*f)(void *);

    void *arg;
} Task;


/* =================================== API ======================================= */


typedef struct thpool_ *threadpool;

int ThreadPoolInsertTask(threadpool, Task *);

void thpool_wait(threadpool);

void thpool_pause(threadpool);

void thpool_resume(threadpool);

struct thpool_ *ThreadPoolInit(int num_threads);

void ThreadPoolDestroy(threadpool);


int thpool_num_threads_working(threadpool);

#ifdef __cplusplus
}
#endif

#endif
