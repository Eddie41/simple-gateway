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
 * ����11005�ű���
 * ��ȡ��Ϸ��Ϣ
 */
class MsgGameinfo:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgGameinfo(){};
    MsgGameinfo(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgGameinfo(){};
};




#endif
