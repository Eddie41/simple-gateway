#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include "MyMD5.h"

#define MAX_BUFF_SIZE 1000
#define INT_SIZE 4
#define CMD_SIZE 4
#define CK_SIZE 16
using namespace std;

// 建立连接，然后收取一切消息
int main(int argc, char **argv)
{
    int issend = 1;

    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int fd;
    int ret;
    char buff[1000]={0};
    string token = "5b0c8fa67cfccdff";

    // 建立SOCKET连接
    if(issend)
    {
        memset(&client_addr, 0, len);
        client_addr.sin_family=AF_INET;
        client_addr.sin_port=htons(23333);
        client_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

        fd=socket(AF_INET, SOCK_STREAM,0);
        ret = connect(fd, (struct sockaddr *)&client_addr, len);
        if (ret < 0)
        {
            fprintf(stderr,"Invoke connect() failed errno is %d\n", errno);
            cout << strerror(errno) << endl;
            exit(0);
        }
    }

    

    
    
    // 组装消息，然后发送
    {
        // 0. 确定消息体的大小
        int token_size = token.size();
        int msg_size      = 4 + 4 + token_size + 4 + 16;
        char msg[msg_size];
        bzero( msg, msg_size );

        int cmd = 10002;
        time_t rawtime;
        time ( &rawtime );
        int timestamp = rawtime;
        int iPointer  = 0; // 表示当前msg数组里面，已经填充了几个字节
        char *tmp     = NULL;
        string md5key = "starwarsstarwars";
        string CK     = "";


        // 1. 填充CMD
        tmp = (char *)&cmd;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;
        
        
        // 2. 填充token大小
        tmp = (char *)&token_size;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;
        
        
        // 3. 填充token
        for(int i=0; i<token_size; i++)
        {
            msg[iPointer+i] = token[i];
        }
        iPointer += token_size;
        
        
        // 4. 填充时间戳
        tmp = (char *)&timestamp;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;


        // 5. 计算CK
        char raw_msg[ iPointer+md5key.size() ];
        for(int i=0; i<iPointer; i++)
        {
            raw_msg[i] = msg[i];
        }
        for(int i=0; i<md5key.size(); i++)
        {
            raw_msg[iPointer+i] = md5key[i];
        }
        iMD5( raw_msg, iPointer+md5key.size(), CK );
        CK = CK.substr(16);
        cout << "CK : " << CK << endl;


        // 6. 填充CK
        for(int i=0; i<16; i++)
        {
            msg[iPointer+i] = CK[i];
        }
        iPointer += 16;
        
        // 7. 发送消息
        printf("writing\n");
        write(fd, msg, msg_size);
        
    }
    

    // 读取服务端的返回
    {

        char msg[MAX_BUFF_SIZE];
        bzero(msg, MAX_BUFF_SIZE);
        
        printf("reading\n");
        int msg_size = recv( fd, msg, MAX_BUFF_SIZE, 0 );
        if(msg_size <= 0)
        {
            cout << "no data!" << endl;
            close(fd); // 友好退出
            return -1;
        }
        cout << "respond size " << msg_size << endl;


        // 前4个字节为指令
        int cmd = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i];
            }
            
            cmd = *( (int *)data );
        }
        cout << "[READ] [fd:" << fd << "] CMD " << cmd << endl;
        if( cmd != 20002 )
        {
            cout << "不是想要的回复~" << endl;
            close(fd); // 友好退出
            return 1;
        }
        
        cout << "建立连接成功" << endl;
        
    }

    // 读取服务端的返回
    for( int i=0; i<99; i++ )
    {

        char msg[MAX_BUFF_SIZE];
        bzero(msg, MAX_BUFF_SIZE);
        
        printf("reading\n");
        int msg_size = recv( fd, msg, MAX_BUFF_SIZE, 0 );
        if(msg_size <= 0)
        {
            cout << "no data!" << endl;
            close(fd); // 友好退出
            return -1;
        }

        int flag = 0;
        // 前4个字节为指令
        int cmd = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i];
            }
            
            cmd = *( (int *)data );
        }
        flag += INT_SIZE;
        cout << "[READ] [fd:" << fd << "] CMD : " << cmd <<
            " [size:" << msg_size <<   "]" << endl;

    }

    close(fd); // 友好退出

    return 0;
}


