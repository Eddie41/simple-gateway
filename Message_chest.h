#ifndef __MESSAGE_CHEST_H_20160410__
#define __MESSAGE_CHEST_H_20160410__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����12000�ű���
 * �ñ���
 */
class MsgChest:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgChest(){};
    MsgChest(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgChest(){};
};




#endif
