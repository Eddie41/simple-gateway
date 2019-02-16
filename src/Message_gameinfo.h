#ifndef __MESSAGE_GAMEINFO_H_20160416__
#define __MESSAGE_GAMEINFO_H_20160416__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理11005号报文
 * 获取游戏信息
 */
class MsgGameinfo:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgGameinfo(){};
    MsgGameinfo(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgGameinfo(){};
};




#endif
