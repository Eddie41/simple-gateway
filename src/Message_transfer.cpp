#include "Message_transfer.h"


/*
 * 处理30000号报文
 * 转发（消息给客户端）
 */
MsgTransfer::MsgTransfer(const int &fd, \
                   const int &cmd, \
                   char *msg, \
                   const int &msg_size):Message(fd, cmd, msg, msg_size)
{}

int MsgTransfer::doRequest()
{
    int ret;
    int flag=CMD_SIZE;

    // 获取客户端的fd
    int fd_client = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+flag];
        }

        fd_client = *( (int *)data );
    }
    flag += INT_SIZE;
    
    // 获取token大小
    int token_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+flag];
        }

        token_size = *( (int *)data );
    }
    flag += INT_SIZE;
    
    // 获取token
    char token[token_size + 1];
    {
        for(int i=0; i<token_size; i++)
        {
            token[i] = m_msg[i+flag];
        }
        token[token_size] = '\0';
    }
    flag += token_size;

    // 获取msg大小
    int msg_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+flag];
        }

        msg_size = *( (int *)data );
    }
    flag += INT_SIZE;
    
    // 获取msg
    char msg[msg_size];
    {
        for(int i=0; i<msg_size; i++)
        {
            msg[i] = m_msg[i+flag];
        }
    }
    flag += msg_size;
    
    // 验证fd和token
    {
        ret = Net::checkFdAndToken( fd_client, token );
        if(ret < 0)
        {
            LOG(ERROR) << s_print_head << "fd2token检查失败[fd:" << fd_client <<
                "][token:" << token << "]" ;
            return ret;
        }
    }
    
    // 转发给客户端
    {
        ret = send2client( fd_client, msg, msg_size );
        //ret = send( fd_client, msg, msg_size, 0 );
        //LOG(INFO) << s_print_head << "转发成功 MSGSIZE:"<< msg_size << ", ret:"<< ret;
    }

    // 主动关闭请求端
    {
        Net::delFd(m_fd);
    }

    return 1;
}

