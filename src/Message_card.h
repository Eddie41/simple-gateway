#ifndef __MESSAGE_CARD_H_20160415__
#define __MESSAGE_CARD_H_20160415__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理12010号报文
 * 使用卡牌
 */
class MsgCard:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgCard(){};
    MsgCard(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgCard(){};
};




#endif
