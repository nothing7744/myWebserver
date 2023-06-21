#ifndef REQUEST
#define REQUEST

#include <pthread.h>
#include<string>
#include<unordered_map>
// using namespace std;

const int STATE_PARSE_URI = 1;
const int STATE_PARSE_HEADERS = 2;
const int STATE_RECV_BODY = 3;
const int STATE_ANALYSIS = 4;
const int STATE_FINISH = 5;

const int MAX_BUFF = 1024;

// 有请求出现但是读不到数据,可能是Request Aborted,
// 或者来自网络的数据没有达到等原因,
// 对这样的请求尝试超过一定的次数就抛弃
const int AGAIN_MAX_TIMES = 200;

const int PARSE_URI_AGAIN = -1;
const int PARSE_URI_ERROR = -2;
const int PARSE_URI_SUCCESS = 0;

const int PARSE_HEADER_AGAIN = -1;
const int PARSE_HEADER_ERROR = -2;
const int PARSE_HEADER_SUCCESS = 0;

const int ANALYSIS_ERROR = -2;
const int ANALYSIS_SUCCESS = 0;

const int METHOD_POST = 1;
const int METHOD_GET = 2;
const int HTTP_10 = 1;
const int HTTP_11 = 2;

const int EPOLL_WAIT_TIME = 500;
struct mytimer;
struct requestData;

struct mytimer
{
    bool deleted;
//size_t表示的是无符号整数,在32位系统是unsigned int,在64位系统是unsigned long
    size_t expired_time;
    requestData *request_data;

    mytimer(requestData *_request_data, int timeout);
    ~mytimer();
    void update(int timeout);
    bool isvalid();
    void clearReq();
    void setDeleted();
    bool isDeleted() const;
    size_t getExpTime() const;
};

struct requestData
{
private:
    int againTimes;
    std::string path;
    int fd;
    int epollfd;
    // content的内容用完就清
    std::string content;
    int method;
    int HTTPversion;
    std::string file_name;
    int now_read_pos;
    int state;
    int h_state;
    bool isfinish;
    bool keep_alive;
    std::unordered_map<std::string, std::string> headers;
    mytimer *timer;

// private:
//     int parse_URI();
//     int parse_Headers();
//     int analysisRequest();


public:

    requestData();
    requestData(int _epollfd, int _fd, std::string _path);
    int getFd();
    void setFd(int fd);
    ~requestData();
    void handleRequest();
    
};
#endif
