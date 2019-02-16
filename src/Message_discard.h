#ifndef __MESSAGE_DISCARD_H_201600720__
#define __MESSAGE_DISCARD_H_201600720__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理12011号报文
 * 丢弃卡牌
 */
class MsgDisCard:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgDisCard(){};
    MsgDisCard(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgDisCard(){};
};




#endif
