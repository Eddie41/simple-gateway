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

#define MSG "bbbbbbb"

using namespace std;

int main(int argc, char **argv)
{
    int cnt = 1;
    
    if( argc > 1 )cnt = atoi(argv[1]);
    
    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int fd;
    int ret;

    int wr=0;
    int re=0;

    char buff[64]={0};

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
    
    for(int i=0; i<cnt; ++i)
    {
        bzero( buff, 64 );
        printf("writing\n");
        if (write(fd, MSG, strlen(MSG)) >0) ++wr;
        
        printf("reading\n");
        if (read(fd, buff, 64)>0) ++re;

        cout << buff << endl;
        
        sleep(2);
    }
    
    close(fd);
    

    printf("WR=%d\tRE=%d\n", wr, re);
}


