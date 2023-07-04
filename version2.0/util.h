#ifndef UTIL
#define UTIL
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>

#include"epoll.h"


ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, void *buff, size_t n);
void handle_for_sigpipe();
int setnonblocking(int sockfd);
int initserver(int port);
#endif