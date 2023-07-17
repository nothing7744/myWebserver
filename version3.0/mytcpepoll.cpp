#include"util.h"
#include "request.h"
#include"epoll.h"
#include "threadpool.h"

#include<iostream>
#include<string>
#include<queue>
#include<memory>
using namespace std;

#define MAXEVENTS 100

const int THREADPOOL_THREAD_NUM = 4;
const int QUEUE_CAPAITY = 65535;



using namespace std;
//extern std::priority_queue<mytimer*, std::deque<mytimer*>, timerCmp> myTimerQueue;
extern priority_queue<shared_ptr<mytimer>, deque<shared_ptr<mytimer>>, timerCmp> myTimerQueue;



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
  int epollfd = Epoll::epoll_init();
  struct epoll_event ev;
  // ev.data.fd = listensock;                          
  ev.events = EPOLLIN|EPOLLET; 
  //requestData *req = new requestData();
  shared_ptr<requestData> req(make_shared<requestData>());
  req->setFd(listensock);
  Epoll::epoll_add(epollfd,listensock,req,ev);
 // epoll_add(epollfd,listensock, static_cast<void*>(req),ev);

  while (1)
  { 
    // struct epoll_event events[MAXEVENTS]; 
    int infds =Epoll::my_epoll_wait(epollfd,&ev,MAXEVENTS,-1);
    if (infds < 0)
    {
      printf("epoll_wait() failed.\n"); perror("epoll_wait()"); break;
    }
    if (infds == 0)
    {
      printf("epoll_wait() timeout.\n"); continue;
    }
  Epoll::handle_events(epollfd,listensock,&ev,infds,pool);
  handle_expired_event();
}
  // close(epollfd);
  return 0;
}



