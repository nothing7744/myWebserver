#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

ssize_t readn(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN)
            {
                return readSum;
            }
            else
            {
                return -1;
            }  
        }
        else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    nwritten = 0;
                    continue;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}


//'\0'表示空字符，防止僵尸线程的发生
void handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    //memset(&sa,0,sizeof(a))
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}

int setnonblocking(int sockfd){
  int flag= fcntl(sockfd,F_GETFD,0);
  if(flag<0) return -1;
  if(fcntl(sockfd,F_SETFL,flag|O_NONBLOCK)==-1) return -1;
  return 0;
}

int initserver(int port)
{ 
  if(port<1024||port>65535){
    printf("error port.\n");
    return -1;
  }
  int sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock < 0)
  {
    printf("socket() failed.\n"); return -1;
  }

  int opt = 1; unsigned int len = sizeof(opt);
  
  //消除bind过程中出现的address already in ues
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,len);
 // setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,&opt,len);


  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  if (bind(sock,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
  {
    printf("bind() failed.\n"); close(sock); return -1;
  }

  if (listen(sock,LISTENQ) != 0 )
  {
    printf("listen() failed.\n"); close(sock); return -1;
  }

  return sock;
}
