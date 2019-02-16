#include <iostream>
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

using namespace std;

#define MAX_EPOLL_EVENT 1000
#define EPOLL_SIZE 10000
#define MAX_BUFF_SIZE 1000

// 设置fd为非阻塞
void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        printf("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        printf("fcntl(sock,SETFL,opts)");
        exit(1);
    }
};


int epoll_fd;

void respond(int fd)
{
	int ret=0;
	char buff[MAX_BUFF_SIZE] = {0};

    cout << "responding:" << fd << endl;
	{
		if (fd <= 0)
		{
		    return;
		}

		//memset(buff, 0x00, 4096);
        bzero(buff, MAX_BUFF_SIZE);
		ret = recv( fd, buff, MAX_BUFF_SIZE, 0 );
		if (ret <= 0)
		{
            cout << "Invoke read() failed. fd:" << fd << endl;

			close(fd);
			return;
		}

		fprintf(stderr, " read msg: %s\n", buff);
        sleep(3);
        return ;

		ret = write(fd, "hello", 6);
		if (ret < 0)
		{
		    fprintf(stderr, "Invoke wirte() failed errno is %d\n", errno);
		}

		//struct epoll_event ev;
		//ev.data.fd = fd;
		//ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
		//epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev);
		////close(fd);
	}
}

int main(void)
{
    int pid = getpid();
    cout << "pid : " << pid <<endl;

    int ret, epollFd, socketfd_ser;
    sockaddr_in ser_addr;
    int port = 23333;
    struct epoll_event  ev;

    // 启动socket监听端口
    {
        // 1. 创建socket
        socketfd_ser = socket( AF_INET, SOCK_STREAM, 0 );
        if(socketfd_ser < 0)
        {
            printf("create socket error");
            exit(2);
        }
        printf("create socket %d\n", socketfd_ser);
        int reuse = 1;
        setsockopt(socketfd_ser,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

        // 2. 初始化地址
        bzero(&ser_addr, sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        ser_addr.sin_port   = htons( port );

        // 3. 绑定到socket
        ret = bind( socketfd_ser, (sockaddr *)&ser_addr, sizeof(ser_addr) );
        if( ret < 0 )
        {
            printf( "bind socket error" );
            exit(3);
        }

        // 4. listen
        ret = listen( socketfd_ser, SOMAXCONN ); // [XXX]
        if( ret < 0 )
        {
            printf( "listen socket error" );
            exit(4);
        }
    }
    
    // 设置为非阻塞
    setnonblocking(socketfd_ser); 
    
    // 创建epoll
    {
        epollFd = epoll_create(EPOLL_SIZE);
    }
    
    // 注册epoll
    {
        // 设置与要处理的事件相关的文件描述符
        ev.data.fd = socketfd_ser;
        // 设置要处理的事件类型
        ev.events = EPOLLIN | EPOLLET;

        epoll_ctl(epollFd, EPOLL_CTL_ADD, socketfd_ser, &ev);
    }
    
    printf("NET init suc\n");
    
    int socketfd_cli;
    sockaddr_in cli_addr;
    int len_cli_addr;
    int event_size = 0;
    struct epoll_event events[MAX_EPOLL_EVENT];
    
	while(1)
	{
        event_size = epoll_wait( epollFd, events, MAX_EPOLL_EVENT, 500 ); // [XXX]

        for(int i=0; i<event_size; ++i)
        {
            // 关闭和挂起的事件优先处理
            if( (events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) )
            {
                cout << "[EVENT:ERROR_OR_HUP] fd:" << events[i].data.fd << endl;
            
                if( events[i].data.fd == socketfd_ser ) // 如果是服务自己有问题
                {
                    printf("listen fd gets some error");
                    return -1; //退出服务
                }
                else    // 其他的关闭fd
                {
                    cout << "close fd:" << events[i].data.fd << endl;
                    close( events[i].data.fd );
                    continue;
                }
            }

            // 处理其他事件
            if( events[i].data.fd == socketfd_ser ) // 新晋客户端
            {
                
                // 接收到请求
                while((socketfd_cli = accept( socketfd_ser, (sockaddr *)&cli_addr, (socklen_t *)&len_cli_addr )) > 0)
                {
                    // 设置为非阻塞
                    setnonblocking(socketfd_cli);
                    
                    // 注册epoll
                    {
                        // 设置与要处理的事件相关的文件描述符
                        ev.data.fd = socketfd_cli;
                        // 设置要处理的事件类型
                        ev.events = EPOLLIN | EPOLLET;

                        epoll_ctl(epollFd, EPOLL_CTL_ADD, socketfd_cli, &ev);
                    }
                    
                    cout <<  "[ACCEPT] [fd:" << socketfd_cli <<  "] IP " << inet_ntoa(cli_addr.sin_addr) << endl;
                }
            }
            else if( events[i].events & EPOLLIN ) // 读事件
            {
                cout << "[EVENT:READ] fd:" << events[i].data.fd << endl;
            
                int &sockfd = events[i].data.fd;
                if(sockfd < 0)
                {
                    printf( "epoll event get error fd" );
                    continue;
                }
                
                // 添加到容器里面（供工作线程处理）
                respond(events[i].data.fd);
            }
            else if( events[i].events & EPOLLOUT ) // 写事件
            {
                printf("[EVENT:WRITE] [fd:%d]", events[i].data.fd);
            }
            else
            {
                printf("[EVENT:UNKNOWN] [fd:%d]", events[i].data.fd);
                continue;
            }
            
        }

	}

	exit(0);
}

