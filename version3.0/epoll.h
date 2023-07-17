#ifndef EVENTPOLL
#define EVENTPOLL
#include <sys/epoll.h>
#include <errno.h>
#include<unordered_map>
using namespace std;
#include "threadpool.h"
#include "request.h"

const int MAXEVENTS = 5000;
const int LISTENQ = 1024;

//可以将在这些用一个Epoll类分封装起来
class Epoll{
private:
//static epoll_event*events;
static unordered_map<int,shared_ptr<requestData>>fd2req;
public:
static int epoll_init();
static int epoll_add(int epoll_fd, int fd, shared_ptr<requestData>request, struct epoll_event event);
static int epoll_mod(int epoll_fd, int fd, shared_ptr<requestData>request, __uint32_t events);
static int epoll_del(int epoll_fd, int fd, shared_ptr<requestData>request, __uint32_t events);
static int my_epoll_wait(int epoll_fd, struct epoll_event* events, int max_events, int timeout);
static void handle_events(int epoll_fd, int listen_fd, struct epoll_event* events, int events_num, threadpool* tp);
};
#endif
