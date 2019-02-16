#ifndef __MESSAGE_NEWGAME_H_20160410__
#define __MESSAGE_NEWGAME_H_20160410__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理11000号报文
 * 新建游戏
 */
class MsgNewGame:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgNewGame(){};
    MsgNewGame(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgNewGame(){};
};




#endif
