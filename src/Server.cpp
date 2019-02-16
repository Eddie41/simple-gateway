#include "common.h"
#include "MyBox.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include "SignalDeal.h"
#include "Net.h"
#include "NetWorker.h"
#include "EasClient.h"

using namespace MyBox;

void usage(char *argv[])
{
    cout << endl << "[USAGE] " << argv[0] << " --config={file of configure}" << endl;
    cout << "[EXAMPLE] " << argv[0] << " --config=../etc/BGC.cfg" << endl << endl;
    
    exit(1);
};


int main(int argc, char *argv[])
{
    if(2 != argc || (strncmp(argv[1], "--config=", 9) != 0))
    {
        usage(argv);
    }
    string cfgFile = &(argv[1][9]); // �����ļ�·��
    if( cfgFile.size() <= 0 )usage(argv);
    
    try
    {
        g_cfg.open(cfgFile);
    }
    catch(MBException & ex)
    {
        printf("%s:%d: �������ļ�����[%s] \n", __FILE__, __LINE__, ex.what());
        return -10;
    }


    /**
     * ������
     */
    int     ret;
    string  logcnf       = GetCfgValue("LOG", "file", "../log/bonusCommunicator.log");
    string  loglevel     = GetCfgValue("LOG", "level", "DEBUG");
    int     max_log_size = MyBox::S2I( GetCfgValue("LOG", "size", "100") );
    int     port         = MyBox::S2I( GetCfgValue("COMMON", "port", "23333") );
    int     workerNumber = MyBox::S2I( GetCfgValue("COMMON", "worker_num", "2") );
    
    // ��ʼ����־
    MB_LOGGER_INIT(logcnf, MBLogger::str2Level(loglevel),
                MBLogger::LOGF_TIME | MBLogger::LOGF_LEVEL |  MBLogger::LOGF_TID | MBLogger::LOGF_FLASH,
                1024*1024*max_log_size);
    MB_LOGGER_DEBUG("init log suc!");
    
    // ��ʼ���źŴ�����
    ret = SignalDeal::init();
    if (ret < 0)
    {
        MB_LOGGER_ERROR("INIT SIGNAL error");
        exit(1);
    }
    
    // ��ʼ��EAS�ͻ���
    ret = EasClient::init();
    if (ret < 0)
    {
        MB_LOGGER_ERROR("INIT EAS error");
        exit(1);
    }
    

    // ��ʼ���������
    Net::init(port);
    // ��ʼ�������߳�
    NetWorker::init(workerNumber);
    
    // �����������
    Net::run();

    // ֹͣ�������
    Net::stop();
    // �ȴ������߳��˳�
    NetWorker::join();

    MB_LOGGER_INFO("process end!");
    
    return 0;
}
