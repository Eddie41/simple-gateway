#ifndef __MESSAGE_PING_H_20170316__
#define __MESSAGE_PING_H_20170316__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理13000号报文以及对应的33000号报文
 * 心跳
 */
class MsgPing:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgPing(){};
    MsgPing(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgPing(){};
};




#endif
