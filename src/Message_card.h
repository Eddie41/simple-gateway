#ifndef __MESSAGE_CARD_H_20160415__
#define __MESSAGE_CARD_H_20160415__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����12010�ű���
 * ʹ�ÿ���
 */
class MsgCard:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgCard(){};
    MsgCard(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgCard(){};
};




#endif
