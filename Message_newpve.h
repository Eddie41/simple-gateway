#ifndef __MESSAGE_NEWPVE_H_20160926__
#define __MESSAGE_NEWPVE_H_20160926__

#include <iostream>
#include <string>
#include <errno.h>
#include "common.h"
#include "Message.h"
#include "HttpClient.h"


using namespace std;



/*
 * ����11001�ű���
 * �½���Ϸ
 */
class MsgNewPVE:public Message
{
public:
    // ��������
    virtual int doRequest();
public:
    MsgNewPVE(){};
    MsgNewPVE(const int &fd, const int &cmd, char *msg, const int &msg_size);
    virtual ~MsgNewPVE(){};
};




#endif
