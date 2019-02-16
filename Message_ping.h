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
 * ����13000�ű����Լ���Ӧ��33000�ű���
 * ����
 */
class MsgPing:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgPing(){};
    MsgPing(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgPing(){};
};




#endif
