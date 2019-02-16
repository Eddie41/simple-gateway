#ifndef __MESSAGE_H_20160228__
#define __MESSAGE_H_20160228__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "NetWorker.h"

using namespace std;

class Message
{
public:
    static int init(const string &cws_addr);
    
public:
    // 工厂模式，返回对应报文的处理对象
    static Message *New(const int &fd, const int &cmd, char *msg, const int &msg_size);
    // 处理请求
    virtual int doRequest(){return -1;};
    
public:
    int send2client(const int &fd, char *msg, const int &msg_size);
    
public:
    Message(){};
    Message(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~Message(){};

public:
    static string m_cws_addr; // cws服务的地址
    static string m_cws_key; // cws服务的公钥
    
public:
    string s_print_head;
    int m_cmd;              // CMD
    int m_fd;               // 客户端FD
    char *m_msg;            // 客户端报文消息（全部）
    int m_msgSize;          // 客户端报文消息的大小
};




#endif
