#include "Message_connect.h"


/*
 * 处理10002号报文以及对应的20002号报文
 * 建立连接
 */
MsgConnect::MsgConnect(const int &fd, \
                   const int &cmd, \
                   char *msg, \
                   const int &msg_size):Message(fd, cmd, msg, msg_size)
{}

int MsgConnect::doRequest()
{
    int ret;
    
    // 判断消息体的大小
    if( m_msgSize <= CMD_SIZE + INT_SIZE + TIMESTAMP_SIZE + CK_SIZE )
    {
        LOG(ERROR) << s_print_head << "[msgsize:"
            << m_msgSize << "] less than minimum!";
        return -1;
    }
    
    // 获取TOKEN大小
    int token_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+INT_SIZE];
        }

        token_size = *( (int *)data );
        if( m_msgSize != INT_SIZE + INT_SIZE + token_size + INT_SIZE + CK_SIZE )
        {
            LOG(ERROR) << s_print_head << "[token_size:"
                << token_size << "] illegal";
            return -1;
        }
    }
    
    // 获取token
    char token[token_size + 1];
    {
        for(int i=0; i<token_size; i++)
        {
            token[i] = m_msg[i+CMD_SIZE+INT_SIZE];
        }
        token[token_size] = '\0';
    }
    LOG(INFO) << s_print_head << "token:" << token;

    
    // 请求cws服务
    string url_resp = "";
    {

        string url = m_cws_addr + "/user/connect";
        string querystr = "";
        querystr = querystr + "tk=" + token +\
                        "&ip=" + my_host +\
                        "&port=" + my_port +\
                        "&fd=" + N2S(m_fd);
        url = cwsURL(url, querystr, m_cws_key);

        HttpClient::post( url, url_resp );
        LOG(INFO) << s_print_head << "url返回:" << url_resp; // [XXX] 测试代码
        
        // 首先判断返回格式
        if( url_resp.size() < 5 )
        {
            // 长度太短，错误
            LOG(ERROR) << s_print_head << "建立连接失败，URL返回格式错误";
            return -1;
        }
        
        // 然后判断返回码
        string sCode = url_resp.substr(0, 3);
        int iCode    = atoi( sCode.c_str() );
        if(iCode < 200 || iCode > 299)
        {
            // 返回码不是2xx表示请求失败
            LOG(ERROR) << s_print_head << "建立连接失败";
            return -1;
        }

    }

    // 登记fd和token的关联关系
    {
        Net::relateFdAndToken(m_fd, token);
    }
    
    // 返回给客户端
    {
        int msg_size = 4;
        char msg[msg_size];
        bzero( msg, msg_size );

        // 组装消息
        {
            int cmd = 20002;
            int iPointer  = 0; // 表示当前msg数组里面，已经填充了几个字节
            char *tmp     = NULL;

            // 1. 填充CMD
            tmp = (char *)&cmd;
            for(int i=0; i<4; i++)
            {
                msg[iPointer+i] = tmp[i];
            }
            iPointer += 4;

        }
        

        // 发送消息
        {
            //ret = send( m_fd, msg, msg_size, 0 );
            ret = send2client( m_fd, msg, msg_size );
            //LOG(INFO) << s_print_head << "RESPOND TOKEN, length "<< msg_size << ", ret "<< ret;
        }


    }
    
    
    return 1;
}


