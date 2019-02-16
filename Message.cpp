#include "Message.h"
#include "Message_login.h"
#include "Message_connect.h"
#include "Message_connecttable.h"
#include "Message_transfer.h"
#include "Message_newgame.h"
#include "Message_newpve.h"
#include "Message_newpvp.h"
#include "Message_chest.h"
#include "Message_card.h"
#include "Message_discard.h"
#include "Message_gameinfo.h"
#include "Message_choosefreecard.h"
#include "Message_ping.h"

string Message::m_cws_addr;
string Message::m_cws_key = "starwarsstarwars";

int Message::init( const string &cws_addr )
{
    m_cws_addr = cws_addr;

    return 1;
}


// 简单工厂模式
Message *Message::New(const int &fd, const int &cmd, char *msg, const int &msg_size)
{
    //
    if( cmd == 10001 )
    {
        return new MsgLogin(fd, cmd, msg, msg_size);
    }
    else if( cmd == 10002 )
    {
        return new MsgConnect(fd, cmd, msg, msg_size);
    }
    else if( cmd == 10003 )
    {
        return new MsgConnectTable(fd, cmd, msg, msg_size);
    }
    else if( cmd == 11000 )
    {
        return new MsgNewGame(fd, cmd, msg, msg_size);
    }
    else if( cmd == 11001 )
    {
        return new MsgNewPVE(fd, cmd, msg, msg_size);
    }
    else if( cmd == 11002 )
    {
        return new MsgNewPVP(fd, cmd, msg, msg_size);
    }
    else if( cmd == 11005 )
    {
        return new MsgGameinfo(fd, cmd, msg, msg_size);
    }
    else if( cmd == 12000 )
    {
        return new MsgChest(fd, cmd, msg, msg_size);
    }
    else if( cmd == 12010 )
    {
        return new MsgCard(fd, cmd, msg, msg_size);
    }
    else if( cmd == 12011 )
    {
        return new MsgDisCard(fd, cmd, msg, msg_size);
    }
    else if( cmd == 11003 )
    {
        return new MsgChooseFreeCard(fd, cmd, msg, msg_size);
    }
    else if( cmd == 13000 )
    {
        return new MsgPing(fd, cmd, msg, msg_size);
    }
    else if( cmd == 30000 )
    {
        return new MsgTransfer(fd, cmd, msg, msg_size);
    }
    return NULL;
}

Message::Message(const int &fd, const int &cmd, char *msg, const int &msg_size)
{
    this->m_msg         = msg;
    this->m_fd          = fd;
    this->m_cmd         = cmd;
    this->m_msgSize     = msg_size;
    
    char tmp[100] = {0};
    bzero( tmp, 100 );
    sprintf( tmp, "[CMD:%d] [fd:%d] ", m_cmd, m_fd );
    this->s_print_head  = tmp;
}


int Message::send2client(const int &fd, char *msg, const int &msg_size)
{
    int ret;
    
    // 给消息的头部加上消息的长度，之后再发给客户端
    {
        int new_msg_size = INT_SIZE + msg_size;
        char new_msg[ new_msg_size ];
        bzero( new_msg, new_msg_size );

        // 组装消息
        {
            int iPointer  = 0; // 表示当前msg数组里面，已经填充了几个字节
            char *tmp     = NULL;


            // 1. 填充消息的长度
            tmp = (char *)&new_msg_size;
            for(int i=0; i<INT_SIZE; i++)
            {
                new_msg[iPointer+i] = tmp[i];
            }
            iPointer += INT_SIZE;


            // 2. 填充正文
            for(int i=0; i<msg_size; i++)
            {
                new_msg[iPointer+i] = msg[i];
            }
            iPointer += msg_size;


        }
        

        // 发送消息
        {
            ret = send( fd, new_msg, new_msg_size, 0 );
            LOG(INFO) << s_print_head << "发送消息 to [fd:" << fd << "], [length:"<< msg_size << "], [ret:" << ret << "]";
        }

            
    }
    
    
    return ret;
}


