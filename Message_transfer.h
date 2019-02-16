#ifndef __MESSAGE_TRANSFER_H_20160409__
#define __MESSAGE_TRANSFER_H_20160409__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理30000号报文
 * 转发（消息给客户端）
 */
class MsgTransfer:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgTransfer(){};
    MsgTransfer(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgTransfer(){};
};




#endif
