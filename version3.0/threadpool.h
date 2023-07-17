#ifndef THREADPOOL
#define THREADPOOL
#include<pthread.h>
#include<stdlib.h>
#include<cstdio>
#include<string.h>
#include<memory>
#include <functional>
using namespace std;

typedef struct Task
{
    std::function<void(std::shared_ptr<void>)> fun;
    std::shared_ptr<void> args;
}task;

struct threadpool_t
{   pthread_t*threadid;
    task*taskQ;
    int queueCapacity;
    int queuesize;
    int numThread;
    int first;
    int tail;
    pthread_mutex_t mutex_pool;
    pthread_cond_t  notfull;
    pthread_cond_t  notempty;
    int shutdown;
};
typedef struct threadpool_t threadpool;


threadpool*threadpool_create(int numThread,int queueCapacity);
void*worker(void*arg);
int threadpool_add(threadpool* pool,function<void(shared_ptr<void>args)> fun,shared_ptr<void>args);
int threadpool_destory(threadpool*pool);
int threadpool_free(threadpool*pool);
int threadpool_exit(threadpool*pool);
#endif

