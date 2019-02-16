#ifndef __MESSAGE_CONNECTTABLE_H_20160506__
#define __MESSAGE_CONNECTTABLE_H_20160506__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * 处理10003号报文以及对应的20003号报文
 * 建立连接
 */
class MsgConnectTable:public Message
{
public:
    // 处理请求
    virtual int doRequest();
public:
    MsgConnectTable(){};
    MsgConnectTable(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgConnectTable(){};
};




#endif
