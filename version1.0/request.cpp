#include "request.h"
#include "epoll.h"
#include "util.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include<iostream>
#include <queue>
#include<string>

requestData::requestData(): 
    now_read_pos(0), state(STATE_PARSE_URI),
    keep_alive(false), againTimes(0), timer(NULL)
{
    printf("requestData constructed !\n");
}

requestData::requestData(int _epollfd, int _fd, std::string _path):
    now_read_pos(0), state(STATE_PARSE_URI),// h_state(h_start), 
    keep_alive(false), againTimes(0), timer(NULL),
    path(_path), fd(_fd), epollfd(_epollfd)
{}

requestData::~requestData()
{
    printf("~requestData()\n");
    struct epoll_event ev;
    // 超时的一定都是读请求，没有"被动"写。
    //ev.events=NULL也可以
    ev.events = EPOLLIN;
    ev.data.ptr = (void*)this;
    //删除该socket
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
    if (timer != NULL)
    {
        // timer->clearReq();
        timer = NULL;
    }
    close(fd);
}


int requestData::getFd()
{
    return fd;
}
void requestData::setFd(int _fd)
{
    fd = _fd;
}

//处理数据的函数(这个函数是如何处理数据的)
void requestData::handleRequest()
{
    char buff[MAX_BUFF];
    bool isError = false;
    while (true)
    {   memset(buff,0,sizeof(buff));
        int read_num = readn(fd, buff, MAX_BUFF);
        if (read_num < 0)
        {
            perror("1");
            isError = true;
            break;
        }
        else if (read_num == 0)
        {
            // 有请求出现但是读不到数据，可能是Request Aborted，或者来自网络的数据没有达到等原因
            perror("read_num == 0");//" "部分的内容加上错误的原因
            isError = true;
            break;
        }
        else{
        printf("receive %s\n",buff);
        memset(buff,0,sizeof(buff));
        sprintf(buff,"hello,client");
        write(fd,buff,MAX_BUFF);
        break;
        }

        }
    __uint32_t _epo_event = EPOLLIN |EPOLLET|EPOLLONESHOT;
    if(isError){
        delete this;
        return;
    }
    int ret = epoll_mod(epollfd, fd, static_cast<void*>(this), _epo_event);
    if (ret < 0)
    {
        // 返回错误处理(调用delete this会执行析构函数)
        delete this;
        return;
     }
    //  delete this;
     return;
}

