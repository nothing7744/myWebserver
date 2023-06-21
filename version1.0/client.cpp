#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("usage:./tcpclient ip port\n"); return -1;
  }

  int sockfd;
  struct sockaddr_in servaddr;
  char buf[1024];
 
  if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; }
	
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(atoi(argv[2]));
  servaddr.sin_addr.s_addr=inet_addr(argv[1]);


  if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
  {
    printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
  }

  printf("connect ok.\n");
  
  do{ memset(buf,0,sizeof(buf));
  sprintf(buf,"hello,server");
    if (write(sockfd,buf,strlen(buf)) <=0)
    { 
      printf("write() failed.\n");  close(sockfd);  return -1;
    }
   // printf("write%s\n",buf);
    memset(buf,0,sizeof(buf));
    if (read(sockfd,buf,sizeof(buf)) <=0)
    { 
      printf("read() failed.\n");  close(sockfd);  return -1;
    }
    printf("read %s\n",buf);
    sleep(10);
    }
    while(0);
    close(sockfd);
    return 0;
} 


// bool biz000(int sockfd)  // 发送心跳报文。
// {
//   char buf[1024];    // 存放数据的缓冲区。
//   sprintf(buf,"hello,server");
//     if (write(sockfd,buf,strlen(buf)) <=0)
//     { 
//       printf("write() failed.\n");  close(sockfd);  return -1;
//     }
//     printf("write%s\n",buf);

//     if (read(sockfd,buf,strlen(buf)) <=0)
//     { 
//       printf("read() failed.\n");  close(sockfd);  return -1;
//     }
//     printf("read%s\n",buf);
//   //printf("接收：%s\n",strbuffer);

//   return true;
// }
