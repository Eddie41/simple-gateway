#include "Message_login.h"


/*
 * 处理10001号报文以及对应的20001号报文
 * 登录
 */
MsgLogin::MsgLogin(const int &fd, \
                   const int &cmd, \
                   char *msg, \
                   const int &msg_size):Message(fd, cmd, msg, msg_size)
{}

int MsgLogin::doRequest()
{
    int ret;
    
    // 判断消息体的大小
    if( m_msgSize <= CMD_SIZE + INT_SIZE + TIMESTAMP_SIZE + CK_SIZE )
    {
        LOG(ERROR) << s_print_head << "[msgsize:"
            << m_msgSize << "] less than minimum!";
        return -1;
    }
    
    // 获取用户名大小
    int username_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+INT_SIZE];
        }

        username_size = *( (int *)data );
        if( m_msgSize != INT_SIZE + INT_SIZE + username_size + INT_SIZE + CK_SIZE )
        {
            LOG(ERROR) << s_print_head << "[username_size:"
                << username_size << "] illegal";
            return -1;
        }
    }
    
    // 获取用户名
    char username[username_size + 1];
    {
        for(int i=0; i<username_size; i++)
        {
            username[i] = m_msg[i+INT_SIZE+INT_SIZE];
        }
        username[username_size] = '\0';
    }
    LOG(INFO) << s_print_head << "username:" << username;

    
    // 请求cws服务做登录
    string url_resp = "";
    {
        //string url = m_cws_addr + "/user/login?u=" + username; // [XXX] 注意可能用户名里面带有引用符号

        string url = m_cws_addr + "/user/login";
        string querystr = "";
        querystr = querystr + "u=" + username;
        url = cwsURL(url, querystr, m_cws_key);

        HttpClient::post( url, url_resp );
        LOG(INFO) << s_print_head << "url返回:" << url_resp; // [XXX] 测试代码
        
        // 首先判断返回格式
        if( url_resp.size() < 5 )
        {
            // 长度太短，错误
            LOG(ERROR) << s_print_head << "[" << username << "]登录失败，URL返回格式错误";
            return -1;
        }
        
        // 然后判断返回码
        string sCode = url_resp.substr(0, 3);
        int iCode    = atoi( sCode.c_str() );
        if(iCode < 200 || iCode > 299)
        {
            // 返回码不是2xx表示请求失败
            LOG(ERROR) << s_print_head << "[" << username << "]登录失败";
            return -1;
        }

        // 截取冒号后面的加密信息
        url_resp = url_resp.substr(5, url_resp.size()-5);
    }
    
    // 返回给客户端
    {
        int url_resp_size = url_resp.size();
        int msg_size      = 4 + 4 + url_resp_size;
        char msg[msg_size];
        bzero( msg, msg_size );

        // 组装消息
        {
            int cmd = 20001;
            int iPointer  = 0; // 表示当前msg数组里面，已经填充了几个字节
            char *tmp     = NULL;


            // 1. 填充CMD
            tmp = (char *)&cmd;
            for(int i=0; i<4; i++)
            {
                msg[iPointer+i] = tmp[i];
            }
            iPointer += 4;
            
            
            // 2. 填充 url_resp 大小
            tmp = (char *)&url_resp_size;
            for(int i=0; i<4; i++)
            {
                msg[iPointer+i] = tmp[i];
            }
            iPointer += 4;
            
            
            // 3. 填充用户名
            for(int i=0; i<url_resp_size; i++)
            {
                msg[iPointer+i] = url_resp[i];
            }
            iPointer += url_resp_size;

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

