#ifndef __MESSAGE_NEWPVP_H_20160926__
#define __MESSAGE_NEWPVP_H_20160926__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����11002�ű���
 * �½���Ϸ
 */
class MsgNewPVP:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgNewPVP(){};
    MsgNewPVP(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgNewPVP(){};
};




#endif
