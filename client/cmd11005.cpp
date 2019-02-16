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

// 获取游戏信息
int main(int argc, char **argv)
{
    int issend = 1;

    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int fd;
    int ret;
    char buff[1000]={0};
    string token = "327145fab8ffe603";
    string tableid = "77571ff3ae27e366";
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

    

    // 建立连接 10003
    {
        // 0. 确定消息体的大小
        int token_size    = token.size();
        int tableid_size  = tableid.size();
        int msg_size      = 4 + 4 + token_size + 4 + tableid_size + 4 + 16;
        char msg[msg_size];
        bzero( msg, msg_size );

        int cmd = 10003;
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
        
        // 6. 填充时间戳
        tmp = (char *)&timestamp;
        for(int i=0; i<4; i++)
        {
            msg[iPointer+i] = tmp[i];
        }
        iPointer += 4;


        // 7. 计算CK
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


        // 8. 填充CK
        for(int i=0; i<16; i++)
        {
            msg[iPointer+i] = CK[i];
        }
        iPointer += 16;
        
        // 9. 发送消息
        printf("writing\n");
        write(fd, msg, msg_size);
        
    }
    

    // 读取服务端的返回 20003
    {

        char msg[MAX_BUFF_SIZE];
        bzero(msg, MAX_BUFF_SIZE);
        
        printf("reading\n");
        int msg_size = recv( fd, msg, MAX_BUFF_SIZE, 0 );
        if(msg_size <= 0)
        {
            cout << "no data!" << endl;
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
        if( cmd != 20003 )
        {
            cout << "不是想要的回复~" << endl;
            return 1;
        }
        
        cout << "建立连接成功" << endl;
        
    }

    
    
    // 获取游戏信息 11005
    {
        // 0. 确定消息体的大小
        int token_size = token.size();
        int tableid_size = tableid.size();
        int msg_size = 4 + 4 + token_size + 4 + tableid_size + 4 + 16;
        char msg[msg_size];
        bzero( msg, msg_size );

        int cmd = 11005;
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
    


    // 读取服务端的返回(返回游戏信息)
    {

        char msg[MAX_BUFF_SIZE];
        bzero(msg, MAX_BUFF_SIZE);
        
        printf("reading\n");
        int msg_size = recv( fd, msg, MAX_BUFF_SIZE, 0 );
        if(msg_size <= 0)
        {
            cout << "no data!" << endl;
            return -1;
        }
        cout << "respond size " << msg_size << endl;

        
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
        cout << "[READ] [fd:" << fd << "] CMD " << cmd << endl;
        if( cmd != 21005 )
        {
            cout << "不是想要的回复~" << endl;
            close(fd);
            return 1;
        }

        
        // 4个字节为 剩余时间
        int seat = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i+flag];
            }
            
            seat = *( (int *)data );
        }
        flag += INT_SIZE;
        cout << "[READ] [fd:" << fd << "] [SEAT:" <<
            seat << "]" << endl;
        

        // 4个字节为seat
        int seat = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i+flag];
            }
            
            seat = *( (int *)data );
        }
        flag += INT_SIZE;
        cout << "[READ] [fd:" << fd << "] [SEAT:" <<
            seat << "]" << endl;

            
        // 4个字节为 card_number
        int card_number = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i+flag];
            }
            
            card_number = *( (int *)data );
        }
        flag += INT_SIZE;
        cout << "[READ] [fd:" << fd << "] [cards:" << card_number << "]" << endl;

        for(int i=0; i<card_number; i++ )
        {

            // 4个字节为 cardindex
            int cardindex = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                cardindex = *( (int *)data );
            }
            flag += INT_SIZE;


            // 4个字节为 cardid
            int cardid = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                cardid = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "----[card" << i << ":" << cardid << ":" << cardindex << "]" << endl;
        }

        // 4个字节为 player_number
        int player_number = 0;
        {
            char data[INT_SIZE];
            for( int i=0; i<INT_SIZE; i++ )
            {
                data[i] = msg[i+flag];
            }
            
            player_number = *( (int *)data );
        }
        flag += INT_SIZE;
        cout << "[READ] [fd:" << fd << "] [players:" << player_number << "]" << endl;
        cout << "===========[PLAYER INFO BELOW]==========" << endl;

        for(int i=0; i<player_number; i++)
        {
            // 4个字节为 seat
            int seat = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                seat = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "[player] sit:" << seat << endl;

            // 4个字节为 username长度
            int username_size = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                username_size = *( (int *)data );
            }
            flag += INT_SIZE;
            

            // 读取 username
            char username[username_size + 1];
            {
                for( int i=0; i<username_size; i++ )
                {
                    username[i] = msg[i+flag];
                }
                
                username[username_size] = '\0';
            }
            flag += username_size;
            cout << "[player] username:" << username << endl;
            
            // 4个字节为 gems
            int gems = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                gems = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "[player] gems:" << gems << endl;

            // 4个字节为 BP
            int BP = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                BP = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "[player] BP:" << BP << endl;

            // 4个字节为 卡牌数
            int card_number2 = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                card_number2 = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "[player] card_number:" << card_number2 << endl;

            // 4个字节为 角色数
            int role_number = 0;
            {
                char data[INT_SIZE];
                for( int i=0; i<INT_SIZE; i++ )
                {
                    data[i] = msg[i+flag];
                }
                
                role_number = *( (int *)data );
            }
            flag += INT_SIZE;
            cout << "[player] role_number:" << role_number << endl;

            for( int j=0; j<role_number; j++ )
            {
                // 4个字节为 角色编号
                int roleid = 0;
                {
                    char data[INT_SIZE];
                    for( int i=0; i<INT_SIZE; i++ )
                    {
                        data[i] = msg[i+flag];
                    }
                    
                    roleid = *( (int *)data );
                }
                flag += INT_SIZE;

                // 2个字节为 扑克
                char poker[2 + 1];
                {
                    for( int i=0; i<2; i++ )
                    {
                        poker[i] = msg[i+flag];
                    }
                    
                    poker[2] = '\0';
                }
                flag += 2;
                cout << "[player] [roleid:" << roleid << "][poker:"<<poker<<"]" << endl;

            }

            cout << "=======" << endl;
        }

    }




    close(fd); // 友好退出

    return 0;
}


