#include "Net.h"
#include "NetWorker.h"
#include "SignalDeal.h"


int Net::epollFd; // epoll描述符
int Net::socketfd_ser; // socket listen服务描述符
struct epoll_event  Net::ev; // epoll事件
struct epoll_event  Net::events[MAX_EPOLL_EVENT]; // epoll事件
vector<int> Net::v_fdList; // 客户端fd容器
pthread_mutex_t Net::m_fdMutex = PTHREAD_MUTEX_INITIALIZER; // fd容器锁
map <int, string> Net::m_fd2token; // FD到客户端
map <int, int> Net::m_fd2lastTime; // FD的上次心跳时间
pthread_t Net::m_HeartBeatTid; // 心跳监控线程

// 设置fd为非阻塞
void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        LOG(FATAL) << "fcntl(sock,GETFL)";
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        LOG(FATAL) << "fcntl(sock,SETFL,opts)";
        exit(1);
    }
}


int Net::init(const int &port)
{
    int                 ret;
    sockaddr_in         ser_addr;

    // 启动socket监听端口
    {
        // 1. 创建socket
        socketfd_ser = socket( AF_INET, SOCK_STREAM, 0 );
        if(socketfd_ser < 0)
        {
            LOG(FATAL) << "create socket error";
            exit(2);
        }
        int reuse = 1;
        setsockopt(socketfd_ser, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
        setsockopt(socketfd_ser, IPPROTO_TCP, TCP_NODELAY, &reuse, sizeof(int));

        // 2. 初始化地址
        bzero(&ser_addr, sizeof(ser_addr));
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        ser_addr.sin_port   = htons( port );

        // 3. 绑定到socket
        ret = bind( socketfd_ser, (sockaddr *)&ser_addr, sizeof(ser_addr) );
        if( ret < 0 )
        {
            LOG(FATAL) <<  "bind socket error: " << strerror(errno);
            exit(3);
        }

        // 4. listen
        ret = listen( socketfd_ser, SOMAXCONN );
        if( ret < 0 )
        {
            LOG(FATAL) <<  "listen socket error" ;
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


    
    // 启动心跳监控线程
    ret = pthread_create(&m_HeartBeatTid, NULL, loopHeartBeatCheck, NULL);
    if( ret < 0 )
    {
        LOG(ERROR) << "create heart beat thread fail";
        return -1;
    }
    pthread_detach( m_HeartBeatTid ); // 分离
    LOG(INFO) << "heart beat thread tid " << m_HeartBeatTid << " start";

    
    LOG(INFO) << "NET init suc";
    return 1;
}

int Net::run()
{
    LOG(INFO) << "NET start running";

    // 开始接收请求
    while( !SignalDeal::exit() )
    {
        int socketfd_cli;
        sockaddr_in cli_addr;
        int len_cli_addr;

        // 等待
        int event_size = 0;
        event_size = epoll_wait( epollFd, events, MAX_EPOLL_EVENT, 500 );
        //if( event_size > 0 )LOG(INFO) << "epoll gets %d events", event_size);
        
        // 依次处理事件
        for( int i=0; i<event_size; i++ )
        {
            //LOG(INFO) << "server_fd %d, event fd %d", socketfd_ser, events[i].data.fd);
        
            // 关闭和挂起的事件优先处理
            if( (events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) )
            {
                LOG(INFO) << "[EVENT:ERROR_OR_HUP] [fd:" << events[i].data.fd << "]";
            
                if( events[i].data.fd == socketfd_ser ) // 如果是服务自己有问题
                {
                    LOG(ERROR) << "listen fd gets some error";
                    return -1; //退出服务
                }
                else    // 其他的关闭fd
                {
                    LOG(INFO) << "close fd:%d", events[i].data.fd;
                    //close( events[i].data.fd );
                    delFd( events[i].data.fd );
                    continue;
                }
            
            }
        
            // 处理其他事件
            if( events[i].data.fd == socketfd_ser ) // 新晋客户端
            {
                LOG(INFO) << "[EVENT:CLIENT] HERE COMES A PLAYER";
                
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
                    
                    LOG(INFO) <<  "[ACCEPT] [fd:" << socketfd_cli
                        << "] IP " << inet_ntoa(cli_addr.sin_addr);
                }

            }
            else if( events[i].events & EPOLLIN ) // 读事件
            {
                LOG(INFO) << "[EVENT:READ] [fd:" << events[i].data.fd << "]";
            
                int &sockfd = events[i].data.fd;
                if(sockfd < 0)
                {
                    LOG(ERROR) <<  "epoll event get error fd";
                    continue;
                }
                
                // 添加到容器里面（供工作线程处理）
                addFd(events[i].data.fd);

            }
            else if( events[i].events & EPOLLOUT ) // 写事件
            {
                LOG(INFO) << "[EVENT:WRITE] [fd:" << events[i].data.fd << "]";
            }
            else
            {
                LOG(ERROR) << "[EVENT:UNKNOWN] [fd:" << events[i].data.fd << "]";
                continue;
            }
            
        }
        
    }

    LOG(INFO) << "NET run out";
    return 1;
}


int Net::stop()
{
    close(epollFd);
    close(socketfd_ser);

    LOG(INFO) << "NET stop suc";
    return 1;
}

int Net::addFd(const int &fd, bool isWakeUp)
{
    //添加新的读任务
    MutexLock lock(m_fdMutex);

    v_fdList.push_back(fd);
    
    time_t rawtime;
    time ( &rawtime );
    m_fd2lastTime[ fd ] = rawtime;
    
    LOG(INFO) << "add fd " << fd;
    //唤醒所有等待工作线程
    if(isWakeUp)NetWorker::wake();
    
    return 1;
}

//删除FD
int Net::delFd(const int &fd)
{    
    MutexLock lock(m_fdMutex);


    // 1. 删除 fd 到 token 
    map < int, string >::iterator it_f2t = m_fd2token.find( fd );
    if(it_f2t != m_fd2token.end())
    {
        m_fd2token.erase(it_f2t);
        LOG(INFO) << "delete fd from m_fd2token [fd:" << fd << "]";
    }
    
    // 2. 删除 fd 到 心跳时间 
    map < int, int >::iterator it_f2l = m_fd2lastTime.find( fd );
    if(it_f2l != m_fd2lastTime.end())
    {
        m_fd2lastTime.erase(it_f2l);
        LOG(INFO) << "delete fd from m_fd2lastTime [fd:" << fd << "]";
    }


    // 关闭fd
    close(fd); 
    
    LOG(INFO) << "[CLOSE] [fd:" << fd<< "]";
    return 1;
}

int Net::getFd(int &fd)
{
    //获取任务（文件描述符）
    MutexLock lock(m_fdMutex);
    
    if( v_fdList.empty() )
    {
        return 0;
    }
    else
    {
        fd = v_fdList.back();
        v_fdList.pop_back();
        return 1;
    }

    return 0;
}


int Net::cntFd()
{
    return v_fdList.size();
}

/* 下面的函数负责处理token与fd之间的关系 */

//添加fd和token
int Net::relateFdAndToken(const int &fd, const string &token)
{
    if(token.size()<=0) return 0;

    MutexLock lock(m_fdMutex);
    
    // 记录 fd 到 token （多对一）
    m_fd2token[fd] = token;
    
    // 记录 fd 的 上次更新时间
    time_t rawtime;
    time ( &rawtime );
    m_fd2lastTime[ fd ] = rawtime;

    LOG(INFO) << "add token to [fd:" << fd << "][timestamp:" << rawtime << "]";
    return 0;
}

//检查fd和token
int Net::checkFdAndToken(const int &fd, const string &token)
{
    if(token.size()<=0) return -1;

    // 查找
    map < int, string >::iterator it_f2t = m_fd2token.find( fd );
    if(it_f2t != m_fd2token.end())
    {
        string &token_store = it_f2t->second;
        if( token == token_store )
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}


//心跳：刷新fd和token的上次心跳时间
int Net::heartBeat(const int &fd)
{
    MutexLock lock(m_fdMutex);
    
    // 刷新心跳时间
    time_t rawtime;
    time ( &rawtime );
    m_fd2lastTime[ fd ] = rawtime;

    LOG(INFO) << "renew latest heartbeat [fd:" << fd
        << "][timestamp:" << rawtime << "]";

    return 0;
}

// [XXX]待优化，可能在轮转检查 m_fd2lastTime 的过程中，fd被释放了···
void *Net::loopHeartBeatCheck(void *args)
{
    while( !SignalDeal::exit() )
    {
        sleep(10);
        
        // 循环检查fd
        map <int, int>::iterator it_f2t = m_fd2lastTime.begin();
        for(;it_f2t != m_fd2lastTime.end(); it_f2t ++)
        {

            int fd = it_f2t->first;
            int lastHeartBeat = it_f2t->second;  // 上次心跳时间

            time_t rawtime;
            time ( &rawtime );
            int tNow = rawtime; // 当前时间
            
            int T_DEADLINE = 60*5; // 20分钟过期
            if( tNow - lastHeartBeat > T_DEADLINE )
            {
                LOG(ERROR) << "[HEARTBEAT] [LOST] [FD:" << fd
                    << "] last time : " << lastHeartBeat;
                    
                delFd( fd );
                break; // important
            }

        }

        //LOG(INFO) << "[HEARTBEAT] [CHECK]";
    }
    
    LOG(INFO) << "heartbeat thread end";
}


