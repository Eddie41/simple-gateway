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
 * ����10002�ű����Լ���Ӧ��20002�ű���
 * ��������
 */
class MsgConnect:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgConnect(){};
    MsgConnect(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgConnect(){};
};




#endif
