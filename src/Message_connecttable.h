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
 * ����10003�ű����Լ���Ӧ��20003�ű���
 * ��������
 */
class MsgConnectTable:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgConnectTable(){};
    MsgConnectTable(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgConnectTable(){};
};




#endif
