#ifndef __MESSAGE_TRANSFER_H_20160409__
#define __MESSAGE_TRANSFER_H_20160409__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����30000�ű���
 * ת������Ϣ���ͻ��ˣ�
 */
class MsgTransfer:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgTransfer(){};
    MsgTransfer(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgTransfer(){};
};




#endif
