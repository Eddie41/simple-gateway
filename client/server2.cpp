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



int epoll_fd;

void respond(int fd)
{
    
	int ret=0;
	char buff[256] = {0};

    fprintf(stderr, "responding \n");
	{
		if (fd <= 0)
		{
		    return;
		}

		//memset(buff, 0x00, 4096);
        bzero(buff, 256);
		ret = read(fd, buff, 255);	
		if (ret <= 0)
		{
			if (ret < 0) fprintf(stderr, "Invoke read() failed errno is %d\n", errno);
			close(fd);
			return;
		}

		fprintf(stderr, " read msg: %s\n", buff);

		ret = write(fd, "hello", 6);
		if (ret < 0)
		{
		    fprintf(stderr, "Invoke wirte() failed errno is %d\n", errno);
		}

		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
		epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&ev);
		//close(fd);
	}
}

int main(void)
{
    int pid = getpid();
    cout << "pid : " << pid <<endl;
    
	int socketfd_ser = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(socketfd_ser,F_SETFL,fcntl(socketfd_ser,F_GETFL)|O_NONBLOCK);

	epoll_fd = epoll_create(5000);
	struct epoll_event ev = {0};
	ev.data.fd = socketfd_ser;
	ev.events = EPOLLIN|EPOLLET;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socketfd_ser, &ev);

	int on = 1;
	setsockopt(socketfd_ser, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(23333);
	bind(socketfd_ser, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(socketfd_ser, 20);

	struct epoll_event events[200];
	int nfds  = 0;
	int connfd;
	while(1)
	{
		nfds = epoll_wait(epoll_fd, events, sizeof(events)/sizeof(events[0]), -1);
		if (nfds > 0)
		{
			for(int ii=0; ii<nfds; ++ii)
			{
				if (events[ii].data.fd == socketfd_ser)
				{
					while((connfd = accept(socketfd_ser, NULL, NULL)) >0)
					{
						fcntl(connfd,F_SETFL,fcntl(connfd,F_GETFL)|O_NONBLOCK);
						struct epoll_event ev;
						ev.data.fd = connfd;
						ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
						epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connfd,&ev);
					}
					if (errno != EWOULDBLOCK && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)
					{
						fprintf(stderr, "Invoke accept() failed errno is %d\n", errno);
						exit(0);
					}
				}
				else if (events[ii].events&EPOLLIN)
				{
					if (events[ii].data.fd < 0)
					{
					    continue;
					}

					respond(events[ii].data.fd);
				}
   				else if (events[ii].events&(EPOLLERR | EPOLLHUP | EPOLLPRI))
   				{
   					fprintf(stderr,"epoll catch EPOLLERR | EPOLLHUP | EPOLLPRI event\n");	
					close(events[ii].data.fd);
   				}
			}
		}
	}

	exit(0);
}

