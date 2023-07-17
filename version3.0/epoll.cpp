#include "epoll.h"
#include "threadpool.h"

//epoll_event*Epoll::events;

std::unordered_map<int,shared_ptr<requestData>>Epoll::fd2req;
//对epoll对应的函数增加了判断
int Epoll::epoll_init()
{
    int epoll_fd = epoll_create(LISTENQ + 1);
    if(epoll_fd == -1)
        return -1;
    //events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    //能处理的最大的epoll
    // events = new epoll_event[MAXEVENTS];
    return epoll_fd;
}


// 注册新描述符
int Epoll::epoll_add(int epoll_fd, int fd, shared_ptr<requestData>request, struct epoll_event  event)
{

    //printf("add to epoll %d\n", fd);
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        return -1;
    }
    fd2req[fd]=request;
    return 0;
}

// 修改描述符状态
int Epoll::epoll_mod(int epoll_fd, int fd, shared_ptr<requestData>request, __uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        perror("epoll_mod error");
        return -1;
    }
    fd2req[fd]=request;
    return 0;
}

// 从epoll中删除描述符
int Epoll::epoll_del(int epoll_fd, int fd, shared_ptr<requestData>request, __uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
        return -1;
    } 
    auto fd_iter = fd2req.find(fd);
    if (fd_iter != fd2req.end())
     fd2req.erase(fd_iter);
    return 0;
}

// 返回活跃事件数
int Epoll::my_epoll_wait(int epoll_fd, struct epoll_event* events, int max_events, int timeout)
{
    int ret_count = epoll_wait(epoll_fd, events, max_events, timeout);
    if (ret_count < 0)
    {
        perror("epoll wait error");
    }
    return ret_count;
}

void Epoll::handle_events(int epoll_fd, int listen_fd, struct epoll_event* events, int events_num, threadpool* tp)
{
//要设计一个哈希表存储所有的requestData类型的事件

    for(int i = 0; i < events_num; i++)
    {
        // 获取有事件产生的描述符
        //requestData* req = (requestData*)(events[i].data.ptr);
        shared_ptr<requestData>req(Epoll::fd2req[events[i].data.fd]);
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
                //delete req;
                continue;
            }
            // 将请求任务加入到线程池中
            // 加入线程池之前将Timer和request分离
            // req->seperateTimer();
            //data.ptr是给用户自已使用的，epoll不关心里面的内容
            
            int rc = threadpool_add(tp, myHandler, Epoll::fd2req[fd]);
        }
    }
}
