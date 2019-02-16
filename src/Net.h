#ifndef __NET_H_20160226__
#define __NET_H_20160226__

#include "common.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <pthread.h>

using namespace std;

#define EPOLL_SIZE 10000
#define MAX_EPOLL_EVENT 1000

class Net
{
public:
    static int init(const int &port);
    static int run();
    static int stop();

public:
    // fd任务队列函数
    static int cntFd();
    static int getFd(int &fd);
    static int delFd(const int &fd);
    static int addFd(const int &fd, bool isWakeUp=true);

public:
    // fd与token关系函数
    static int relateFdAndToken(const int &fd, const string &token);
    static int checkFdAndToken(const int &fd, const string &token);
    static int heartBeat(const int &fd);

private:
    static void *loopHeartBeatCheck(void *args);
    static pthread_t m_HeartBeatTid;  // 心跳监控线程ID
    
private:
    Net(){};

private:
    static int epollFd; // epoll描述符
    static int socketfd_ser; // socket listen服务描述符
    static struct epoll_event  ev; // epoll事件
    static struct epoll_event  events[MAX_EPOLL_EVENT]; // epoll事件
    
private:
    static vector<int> v_fdList; // 客户端fd容器
    static map <int, string> m_fd2token; // FD到客户端
    static map <int, int> m_fd2lastTime; // FD的上次心跳时间
    static pthread_mutex_t m_fdMutex; // fd容器锁
    
};



#endif
