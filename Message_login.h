#ifndef __MESSAGE_LOGIN_H_20160228__
#define __MESSAGE_LOGIN_H_20160228__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理10001号报文以及对应的20001号报文
 * 登录
 */
class MsgLogin:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgLogin(){};
    MsgLogin(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgLogin(){};
};




#endif
