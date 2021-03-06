#include "Message_choosefreecard.h"


/*
 * 处理11003号报文
 * 选择免费卡牌
 */
MsgChooseFreeCard::MsgChooseFreeCard(const int &fd, \
                   const int &cmd, \
                   char *msg, \
                   const int &msg_size):Message(fd, cmd, msg, msg_size)
{}

int MsgChooseFreeCard::doRequest()
{
    int ret;
    int flag = CMD_SIZE;
    
    // 判断消息体的大小
    // 使用卡牌：11003+长度+TOKEN+长度+游戏ID+卡牌ID
    if( m_msgSize <= CMD_SIZE + INT_SIZE + INT_SIZE +\
        INT_SIZE + TIMESTAMP_SIZE + CK_SIZE )
    {
        LOG(ERROR) << s_print_head << "[msgsize:"
            << m_msgSize << "] less than minimum!";
        return -1;
    }
    
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
    LOG(INFO) << s_print_head << "token:" << token;

    // 获取tableid大小
    int tableid_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+flag];
        }

        tableid_size = *( (int *)data );
    }
    flag += INT_SIZE;
    
    // 获取tableid
    char tableid[tableid_size + 1];
    {
        for(int i=0; i<tableid_size; i++)
        {
            tableid[i] = m_msg[i+flag];
        }
        tableid[tableid_size] = '\0';
    }
    flag += tableid_size;
    LOG(INFO) << s_print_head << "tableid:" << tableid;


    // 获取卡牌ID
    int cardid = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = m_msg[i+flag];
        }

        cardid = *( (int *)data );
    }
    flag += INT_SIZE;
    
    // 请求cws服务
    string url_resp = "";
    {

        string url = m_cws_addr + "/psgame/chooseFreeCard";
        string querystr = "";
        querystr = querystr + "tk=" + token +\
                    "&tableid=" + tableid +\
                    "&cardid=" + N2S(cardid);
        url = cwsURL(url, querystr, m_cws_key);

        HttpClient::post( url, url_resp );
        LOG(INFO) << s_print_head << "url返回:" << url_resp; // [XXX] 测试代码
        
        // 首先判断返回格式
        if( url_resp.size() < 5 )
        {
            // 长度太短，错误
            LOG(ERROR) << s_print_head << "[" << token << "]URL返回格式错误";
            return -1;
        }

        // 然后判断返回码
        string sCode = url_resp.substr(0, 3);
        int iCode    = atoi( sCode.c_str() );
        if(iCode < 200 || iCode > 299)
        {
            // 返回码不是2xx表示请求失败
            LOG(ERROR) << s_print_head << "[" << token << "]URL调用失败[" << iCode << "]";
            return -1;
        }

    }
    
    // 返回给客户端
    {
    }
    
    
    return 1;
}

