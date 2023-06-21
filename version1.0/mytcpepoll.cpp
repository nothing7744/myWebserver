#include"util.h"
#include "request.h"
#include"epoll.h"
#include "threadpool.h"

#include<iostream>
#include<string>
const std::string PATH = "/";
#define MAXEVENTS 100

const int THREADPOOL_THREAD_NUM = 4;
const int QUEUE_CAPAITY = 65535;
const int TIMER_TIME_OUT = 500;


void acceptConnection(int listen_fd, int epoll_fd);
void handle_events(int epoll_fd, int listen_fd, struct epoll_event* events, int events_num,threadpool* tp);
void myHandler(void *args);

int main(int argc,char *argv[])
{
  handle_for_sigpipe();
  if (argc != 2)
  {
    printf("usage:./tcpepoll port\n"); return -1;
  }
  int listensock = initserver(atoi(argv[1]));
  if (listensock < 0)
  {
    printf("initserver() failed.\n"); return -1;
  }
  printf("listensock=%d\n",listensock);

  threadpool *pool = threadpool_create(THREADPOOL_THREAD_NUM,QUEUE_CAPAITY);
  // char buffer[1024];
  // memset(buffer,0,sizeof(buffer));
  int epollfd = epoll_init();
  struct epoll_event ev;
  // ev.data.fd = listensock;                          
  ev.events = EPOLLIN|EPOLLET; 
  requestData *req = new requestData();
  req->setFd(listensock);
  epoll_add(epollfd,listensock, static_cast<void*>(req),ev);

  while (1)
  { 
    // struct epoll_event events[MAXEVENTS]; 
    int infds = my_epoll_wait(epollfd,&ev,MAXEVENTS,-1);
    if (infds < 0)
    {
      printf("epoll_wait() failed.\n"); perror("epoll_wait()"); break;
    }
    if (infds == 0)
    {
      printf("epoll_wait() timeout.\n"); continue;
    }
  handle_events(epollfd,listensock,&ev,infds,pool);
}
  // close(epollfd);
  return 0;
}


void acceptConnection(int listen_fd, int epoll_fd){
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = 0;
    int accept_fd = 0;
    // printf("listen_fd:%d\n",listen_fd);
    if((accept_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0)
    {
        /*
        // TCP的保活机制默认是关闭的
        int optval = 0;
        socklen_t len_optval = 4;
        getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        cout << "optval ==" << optval << endl;
        */
        // 设为非阻塞模式
        int ret = setnonblocking(accept_fd);
        printf("accept_fd:%d\n",accept_fd);
        if (ret < 0)
        {
            perror("Set non block failed!");
            return;
        }
        // printf("listen_fd:%d\n",listen_fd);
        requestData *req_info = new requestData(epoll_fd, accept_fd,PATH);
        // 文件描述符可以读，边缘触发(Edge Triggered)模式，保证一个socket连接在任一时刻只被一个线程处理
        struct epoll_event _epo_event;
        _epo_event.events = EPOLLIN|EPOLLET|EPOLLONESHOT;
        epoll_add(epoll_fd, accept_fd, static_cast<void*>(req_info),_epo_event);
        // 新增时间信息
        // mytimer *mtimer = new mytimer(req_info, TIMER_TIME_OUT);
        // req_info->addTimer(mtimer);
        // pthread_mutex_lock(&qlock);
        // myTimerQueue.push(mtimer);
        // pthread_mutex_unlock(&qlock);
    }
}

void myHandler(void *args)
{
    requestData *req_data = (requestData*)args;
    req_data->handleRequest();
}


void handle_events(int epoll_fd, int listen_fd, struct epoll_event* events, int events_num, threadpool* tp)
{
    for(int i = 0; i < events_num; i++)
    {
        // 获取有事件产生的描述符
        requestData* req = (requestData*)(events[i].data.ptr);
        int fd = req->getFd();
        // int fd=events[i].data.fd;
        // 有事件发生的描述符为监听描述符
        if(fd == listen_fd)
        {
            //cout << "This is listen_fd" << endl;
            acceptConnection(listen_fd, epoll_fd);
        }
        else
        {
            // 排除错误事件
            // if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)
            //     || (!(events[i].events & EPOLLIN)))
        if (events[i].events & EPOLLERR||events[i].events & EPOLLHUP||!(events[i].events & EPOLLIN))
            {
                printf("error event\n");
                delete req;
                continue;
            }
            // 将请求任务加入到线程池中
            // 加入线程池之前将Timer和request分离
            // req->seperateTimer();
            //data.ptr是给用户自已使用的，epoll不关心里面的内容
            int rc = threadpool_add(tp, myHandler, events[i].data.ptr);
        }
    }
}
