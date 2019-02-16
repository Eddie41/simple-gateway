#ifndef __MESSAGE_NEWPVP_H_20160926__
#define __MESSAGE_NEWPVP_H_20160926__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理11002号报文
 * 新建游戏
 */
class MsgNewPVP:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgNewPVP(){};
    MsgNewPVP(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgNewPVP(){};
};




#endif
