#ifndef __MESSAGE_CHOOSEFREECARD_H_201600726__
#define __MESSAGE_CHOOSEFREECARD_H_201600726__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����11003�ű���
 * ѡ����ѿ���
 */
class MsgChooseFreeCard:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgChooseFreeCard(){};
    MsgChooseFreeCard(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgChooseFreeCard(){};
};




#endif
