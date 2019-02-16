#ifndef __MESSAGE_LOGIN_H_20160228__
#define __MESSAGE_LOGIN_H_20160228__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����10001�ű����Լ���Ӧ��20001�ű���
 * ��¼
 */
class MsgLogin:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgLogin(){};
    MsgLogin(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgLogin(){};
};




#endif
