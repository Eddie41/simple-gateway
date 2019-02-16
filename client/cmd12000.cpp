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

// 抽取宝箱
int main(int argc, char **argv)
{
    int issend = 1;

    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int fd;
    int ret;
    char buff[1000]={0};
    string token = "806c17362a1f118f";
    string tableid = "0c2f30ee75fa18bb";
    int slot = 0;

    if( argc >= 2 )
    {
        slot = atoi( argv[1] );
    }
    cout << "slot : " << slot << endl;

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

    

    
    
    // 取箱子 12000
    {
        // 0. 确定消息体的大小
        int token_size = token.size();
        int tableid_size = tableid.size();
        int msg_size = 4 + 4 + token_size + 4 + tableid_size + 4 + 4 + 16;
        char msg[msg_size];
        bzero( msg, msg_size );

        int cmd = 12000;
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

        
        // 4. 填充tableid大小
        tmp = (char *)&tableid_size;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;
        
        
        // 5. 填充tableid
        for(int i=0; i<tableid_size; i++)
        {
            msg[iPointer+i] = tableid[i];
        }
        iPointer += tableid_size;
        
        // 6. 填充slot
        tmp = (char *)&slot;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;

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
    


    close(fd); // 友好退出

    return 0;
}


