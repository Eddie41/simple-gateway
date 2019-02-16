#include <pthread.h>
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

#define MSG "aaaaaaaaaa"

using namespace std;

int main(int argc, char **argv)
{

    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int fd;
    int ret;
    char buff[64]={0};

    // 建立SOCKET连接
    {
        memset(&client_addr, 0, len);
        client_addr.sin_family=AF_INET;
        client_addr.sin_port=htons(23333);
        client_addr.sin_addr.s_addr=inet_addr("192.168.0.235");

        fd=socket(AF_INET, SOCK_STREAM,0);
        ret = connect(fd, (struct sockaddr *)&client_addr, len);
        if (ret < 0)
        {
            fprintf(stderr,"Invoke connect() failed errno is %d\n", errno);
            cout << strerror(errno) << endl;
            exit(0);
        }
    }


    bzero( buff, 64 );
    printf("writing\n");
    write(fd, MSG, strlen(MSG));

    sleep(1);

    //printf("reading\n");
    //if (read(fd, buff, 64)>0) ++re;

    //cout << buff << endl;


    close(fd);
    

    printf("WR=%d\tRE=%d\n", wr, re);
}


