#ifndef __MESSAGE_CONNECT_H_20160406__
#define __MESSAGE_CONNECT_H_20160406__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理10002号报文以及对应的20002号报文
 * 建立连接
 */
class MsgConnect:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgConnect(){};
    MsgConnect(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgConnect(){};
};




#endif
