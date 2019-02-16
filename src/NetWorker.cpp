#include "NetWorker.h"
#include "SignalDeal.h"
#include "common.h"
#include "Message.h"

int NetWorker::m_workerNumber = 0; // 工作线程个数
pthread_t NetWorker::m_tidList[THREAD_MAX]; // 工作线程ID容器
pthread_cond_t NetWorker::m_cond;
pthread_mutex_t NetWorker::m_mutex;
string NetWorker::md5key  = "starwarsstarwars";

void *NetWorker::loopJob(void *args)
{
    while( !SignalDeal::exit() )
    {
        int fd, ret;
        
        // 没有工作的时候，就等待被唤醒
        if( Net::cntFd() <= 0 ) wait(3);
        
        // 看有没有请求过来
        if( Net::getFd(fd) <= 0 )
        {
            sleep(1);
            continue;
        }
        
        
        LOG(INFO) << "[WORKER] [START] [fd:" << fd << "]";
        // 开始计时
        TimeStst tStat;
        // 处理请求
        ret = doJob(fd);

        LOG(INFO) << "[WORKER] [FINISH] [fd:" << fd
            << "] cost " << tStat.GetDiff() << " ms";
        if(ret < 0)
        {
            Net::delFd(fd); // [XXX] 关闭客户端连接
            LOG(INFO) << "[CLOSE] [fd:" << fd << "]";
        }
    }

    LOG(INFO) << "worker thread end";
}



// 读取请求并处理
int NetWorker::doJob(const int &fd)
{
    int ret, buf_size;

    char buf[MAX_BUFF_SIZE];
    bzero(buf, MAX_BUFF_SIZE);
    buf_size = recv( fd, buf, MAX_BUFF_SIZE, 0 );
    if(buf_size <= 0)
    {
        LOG(ERROR) << "[READ] [fd:" << fd
            << "] [ret:" << buf_size << "] no data!";
        return -1;
    }
    
    // 头部至少有4个字节表示长度
    if(buf_size <= INT_SIZE)
    {
        LOG(ERROR) << "[READ] [fd:" << fd << "] [buf_size:"
            << buf_size << "] less than minimum!";
        return -1;
    }

    // 前4个字节为指令长度
    int cmd_size = 0;
    {
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = buf[i];
        }

        cmd_size = *( (int *)data );
    }
    
    // 实际的长度比指令的长度要小，丢弃
    //      [XXX] 实际上这里还是等一下最好，因为消息不完整
    if( buf_size < cmd_size )
    {
        LOG(ERROR) << "[READ] [fd:" << fd << "] [buf_size:"
            << buf_size << "] 要小于 [cmd_size:" << cmd_size << "]";
        return -1;
    }

    int read_size = 0;
    for(int flag=0; flag<buf_size; flag+=cmd_size)
    {
        
        // 前4个字节为指令长度
        char data[INT_SIZE];
        for( int i=0; i<INT_SIZE; i++ )
        {
            data[i] = buf[i+flag];
        }
        cmd_size = *( (int *)data );

        
        // 判断一下会不会溢出
        if( read_size + cmd_size > buf_size )
        {
            LOG(ERROR) << "[READ] [fd:" << fd << "] [buf_size:"
                << buf_size << "] 要小于 [cmd_size:" << read_size + cmd_size << "]";
            return -1;
        }
        

        // 读取真正的消息本体
        int msg_size = cmd_size-INT_SIZE;
        char msg[MAX_BUFF_SIZE];
        bzero(msg, MAX_BUFF_SIZE);
        for(int i=0; i<msg_size; i++)
        {
            msg[i] = buf[i+flag+INT_SIZE];
        }
        read_size += cmd_size;
        
        
        ret = doJob2(fd, msg, msg_size);
        if(ret < 0)
        {
            LOG(ERROR) << "[READ] [fd:" << fd << "] doJob2 调用失败:" << ret;
            return ret;
        }
        
    }
    
    return ret;
}

int NetWorker::doJob2(const int &fd, char *msg, const int &msg_size)
{
    int ret;
    
    // 取消息
    if(msg_size <= 0)
    {
        LOG(ERROR) << "[READ] [fd:" << fd
            << "] [ret:" << msg_size << "] no data!";
        return -1;
    }


    // 消息体至少包括命令(4个字节)
    if(msg_size < CK_SIZE + INT_SIZE + INT_SIZE)
    {
        LOG(ERROR) << "[READ] [fd:" << fd << "] [msgsize:"
            << msg_size << "] less than minimum!";
        return -1;
    }
    LOG(INFO) << "[READ] [fd:" << fd << "] message size " << msg_size;


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
    LOG(INFO) << "[READ] [fd:" << fd << "] CMD " << cmd;


    // 检查CK
    if( cmd != 30000 )
    {
        char raw_msg[msg_size-CK_SIZE+md5key.size()];
        for(int i=0; i<msg_size-CK_SIZE; i++)
        {
            raw_msg[i] = msg[i];
        }
        for(int i=0; i<md5key.size(); i++ )
        {
            raw_msg[ i+msg_size-CK_SIZE ] = md5key[i];
        }

        char   yourCK[CK_SIZE+1];
        for(int i=0; i<CK_SIZE; i++)
        {
            yourCK[i] = msg[i+msg_size-CK_SIZE];
        }
        yourCK[CK_SIZE] = '\0';

        string myCK    = "";
        iMD5( raw_msg, msg_size-CK_SIZE+md5key.size(), myCK );
        myCK = myCK.substr(16, 32);
        if( myCK != string(yourCK) )
        {
            LOG(INFO) << "[READ] [fd:" << fd << "] CK NOT MATCH:["
                << yourCK << "] [" << myCK << "]";
            return -1;
        }
    }



    // 生成对应的处理对象
    Message *MsgDealer = Message::New(fd, cmd, msg, msg_size);
    if(MsgDealer == NULL)
    {
        LOG(ERROR) << "[fd:" << fd << "] NEW Message Handler ERROR";
        return -1;
    }

    try
    {
    
        // 处理请求
        ret = MsgDealer->doRequest();
        if( ret <= 0 )
        {
            LOG(ERROR) << "[fd:" << fd << "] doRequest error";

            delete MsgDealer; // 不要忘记这个
            return ret;
        }
        
        // 然后发送回应
        //MsgDealer->doRespond();

    }
    catch(...)
    {
        LOG(ERROR) << "[fd:" << fd << "] unknown error";
    }
    
    delete MsgDealer;

    return 1;

}


int NetWorker::init(const int &workerNumber)
{
    int ret;
    // 配置的工作线程数过大，取最大值
    if( workerNumber > THREAD_MAX )
    {
        m_workerNumber = THREAD_MAX;
    }
    else
    {
        m_workerNumber = workerNumber;
    }

    // 初始化条件变量和互斥锁
    pthread_cond_init(&m_cond, NULL);
    pthread_mutex_init(&m_mutex, NULL);
    
    // 启动工作线程
    for( int i=0; i < m_workerNumber; i++ )
    {
        ret = pthread_create(&m_tidList[i], NULL, loopJob, NULL);
        if( ret < 0 )
        {
            LOG(ERROR) << "create worker NO." << i << " thread fail";
            return -1;
        }
        LOG(INFO) << "worker NO." << i << " tid " << m_tidList[i] << " start";
    }

    
    LOG(INFO) << "networker init suc";
    return 1;
}

int NetWorker::wait(const int &waitTime)
{
    pthread_mutex_lock(&m_mutex);
    
    struct timespec t;
    t.tv_sec = time(NULL) + waitTime; // 注意这里超时使用的是时间点（不是时间段），搞死了。
    t.tv_nsec = 0;
    pthread_cond_timedwait(&m_cond, &m_mutex, &t);
    // pthread_cond_wait(&m_cond, &m_mutex);

    pthread_mutex_unlock(&m_mutex);
    return 1;
}

int NetWorker::wake()
{
    pthread_cond_broadcast(&m_cond);
    
    LOG(INFO) << "wake up worker";
    return 1;
}

int NetWorker::join()
{
    for(int i=0; i < m_workerNumber; i++)
    {
        pthread_join(m_tidList[i], NULL);
        LOG(INFO) << "worker NO." << i << " tid " << m_tidList[i] << " join";
    }
    
    return 1;
}

