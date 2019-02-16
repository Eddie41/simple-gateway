#ifndef __NETWORKER_H_20160227__
#define __NETWORKER_H_20160227__

#include "common.h"
#include "Net.h"
#include <pthread.h>
#include <string.h>

using namespace std;

#define THREAD_MAX 100
#define MAX_BUFF_SIZE 1000
#define INT_SIZE 4
#define CMD_SIZE 4
#define TIMESTAMP_SIZE 4
#define CK_SIZE 16

class NetWorker
{
public:
    // 初始化工作者线程
    static int init(const int &workerNumber);
    // 唤醒工作线程
    static int wake();
    // 等待工作线程退出
    static int join();
private:
    static void *loopJob(void *args);
    // 等待条件被唤醒
    static int wait(const int &waitTime);
    // 读取请求并处理
    static int doJob(const int &socketfd_cli);
    static int doJob2(const int &socketfd_cli, char *msg, const int &msg_size);
private:
    static pthread_t m_tidList[THREAD_MAX];  // 工作线程ID容器
    static int m_workerNumber; // 工作线程个数
    static pthread_cond_t m_cond; // 工作通知条件变量
    static pthread_mutex_t m_mutex; // 工作通知互斥锁
    
private:
    static string md5key;   // 公用秘钥（不会放在配置文件里面）
};

#endif
