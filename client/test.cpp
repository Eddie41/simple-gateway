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

int main(int argc, char **argv)
{

    int ret;
    string username = "中文";

    cout << username.size() << endl;
    // 组装消息，然后发送
    {
        string CK     = "";

        // 5. 计算CK
        char raw_msg[ 6 ];
        for(int i=0; i<6; i++)
        {
            raw_msg[i] = username[i];
        }
        iMD5( raw_msg, 6, CK );
        //CK = CK.substr(16);
        cout << "CK : " << CK << endl;
        
    }
    
    return 0;
}


