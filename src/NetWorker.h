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
    // ��ʼ���������߳�
    static int init(const int &workerNumber);
    // ���ѹ����߳�
    static int wake();
    // �ȴ������߳��˳�
    static int join();
private:
    static void *loopJob(void *args);
    // �ȴ�����������
    static int wait(const int &waitTime);
    // ��ȡ���󲢴���
    static int doJob(const int &socketfd_cli);
    static int doJob2(const int &socketfd_cli, char *msg, const int &msg_size);
private:
    static pthread_t m_tidList[THREAD_MAX];  // �����߳�ID����
    static int m_workerNumber; // �����̸߳���
    static pthread_cond_t m_cond; // ����֪ͨ��������
    static pthread_mutex_t m_mutex; // ����֪ͨ������
    
private:
    static string md5key;   // ������Կ��������������ļ����棩
};

#endif
