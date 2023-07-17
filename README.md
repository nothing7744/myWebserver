# myWebserver

webbench 用来测试

mytcpepoll 作为服务器

使用

./mytcpepoll port          eg ./mytcpepoll 5005

webbench(使用网上很多)      eg webbench http://127.0.0.1:5005/  

版本更新:
version1.0:处理客服端的请求，使用了线程池，io复用，epoll通信，多线程
version2.0:在第一个版本的基础上解析http请求
version3.0:这个版本发费的时间比较长,替换所有的非智能指针为智能指针,同时引入RAII来加锁，对象的生命周期不需要人为进行控制了
