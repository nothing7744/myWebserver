#include "threadpool.h"

threadpool*threadpool_create(int numThread,int queueCap){
    threadpool*pool=(threadpool*)malloc(sizeof(threadpool));
    do{
    if(pool==NULL){
        printf("create pool fail\n");
        break;
    }
    pool->threadid=(pthread_t*)malloc(sizeof(pthread_t)*numThread);
    pool->taskQ=(task*)malloc(sizeof(task)*queueCap);
    //只要使用malloc一定要判断是不是为NULL
    if(pool->threadid==NULL||pool->taskQ==NULL){
        printf("create thread fail\n");
        break;
    }
    // memset(pool->threadid,0,sizeof(pthread_t)*numThread);
    if(pthread_mutex_init(&(pool->mutex_pool),NULL)!=0||pthread_cond_init(&(pool->notfull),NULL)!=0||pthread_cond_init(&(pool->notempty),NULL)!=0){
        printf("mutex or condition init fail\n");
        break;
    }
    pool->numThread=0;
    pool->queueCapacity=queueCap;
    pool->queuesize=0;
    pool->shutdown=0;
    pool->first=0;
    pool->tail=0;
    for(int i=0;i<numThread;++i){
        if(pthread_create(&(pool->threadid[i]),NULL,worker,pool)!=0){
            printf("thread_pool destory\n");
            threadpool_destory(pool);
            return  NULL;
        };
        pool->numThread++;
        // pool->tail++;
    }
    return pool;
    }
    while(0);
//如果产生异常，会break，这时候我们需要处理异常,释放资源
    if(pool!=NULL){
        threadpool_free(pool);
    }
    return NULL;
}


//共享资源需要加锁
void*worker(void*arg){
    threadpool*pool=(threadpool*)arg;
    task Task;
    while(1){
    pthread_mutex_lock(&(pool->mutex_pool));
    while(pool->queuesize==0&&!(pool->shutdown)){
        pthread_cond_wait(&(pool->notempty),&(pool->mutex_pool));
    }
    if(pool->shutdown){
        break;
    }
    Task.function=pool->taskQ[pool->first].function;
    Task.arg=pool->taskQ[pool->first].arg;
    pool->first=(pool->first+1)%pool->queueCapacity;
    pool->queuesize--;
    pthread_mutex_unlock(&(pool->mutex_pool));

    //运行完毕这个线程之后退出该线程
    //go to work
    (*(Task.function))(Task.arg);
    }
    pthread_mutex_unlock(&(pool->mutex_pool));
    pthread_exit(NULL);   //发生错误了将该线程退出
    return NULL;
}

int threadpool_add(threadpool* pool,void(*func)(void* arg),void* arg){
    pthread_mutex_lock(&(pool->mutex_pool));
    while (pool->queuesize==pool->queueCapacity&&!pool->shutdown)
    {
        pthread_cond_wait(&pool->notfull,&(pool->mutex_pool));
    }
    if(pool->shutdown){
        pthread_mutex_unlock(&pool->mutex_pool);
        return -1;
    }
    pool->taskQ[pool->tail].function=func;
    pool->taskQ[pool->tail].arg=arg;
    pool->tail=(pool->tail+1)%pool->queueCapacity;
    ++pool->queuesize;
//pthread_cond_signal不会有惊群效应，只会给一个线程发送信号，接收这个信号的会根据线程的优先级，或者线程等待的时间来接收这个信号
    pthread_cond_signal(&pool->notempty);
    pthread_mutex_unlock(&pool->mutex_pool);
    return 0;
}


int threadpool_destory(threadpool*pool){
    if(pool==NULL) return -1; 
//唤醒线程之后这些线程会自动的结束    pthread_join调用之后要等待该线程执行完毕之后才会继续进行其他线程
    pthread_cond_broadcast(&(pool->notempty));
    pthread_mutex_destroy(&pool->mutex_pool);
    pthread_cond_destroy(&pool->notempty);
    pthread_cond_destroy(&pool->notfull);
    threadpool_free(pool);
    return 0;
}


int threadpool_free(threadpool*pool){
    if(pool&&pool->threadid){
        free(pool->threadid);
    }
    if(pool&&pool->taskQ){
        free(pool->taskQ);
    }
    if(pool){
        free(pool);
    }
    return 0;
}

