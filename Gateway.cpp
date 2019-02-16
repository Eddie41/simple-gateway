#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "glog/logging.h"
#include "SignalDeal.h"
#include "INIReader.h"
#include "common.h"
#include "Net.h"
#include "NetWorker.h"
#include "Message.h"
#include "HttpClient.h"

using namespace std;

void usage(char *argv[])
{
    cout << endl << "[USAGE] " << argv[0] << " -c {file of configure}" << endl;
    cout << "[EXAMPLE] " << argv[0] << " -c ./Gateway.cfg" << endl << endl;

    exit(1);
};


int main(int argc, char *argv[])
{
    if(3 != argc || (strncmp(argv[1], "-c", 2) != 0))
    {
        usage(argv);
    }
    

    int ret;
    int port = 0;
    int workerNumber = 0;
    int pid = getpid();
    string server_name = argv[0];
    string port_str = "";
    string log_dir = "";
    string log_info_name = "";
    string log_error_name = "";
    string cws_addr = "";


    // 1. ��ȡ�����ļ�
    {
        INIReader reader( argv[2] );
        if( reader.ParseError() < 0 )
        {
            cout << "read config error" << endl;
            return 1;
        }

        port         = reader.GetInteger( "SERVER", "listen", 0 );
        port_str     = reader.Get( "SERVER", "listen", "" );
        log_dir      = reader.Get( "SERVER", "log_dir", "" );
        cws_addr     = reader.Get( "CWS", "address", "" );
        workerNumber = reader.GetInteger( "WORKER", "number", 0 );

        if( port == 0 ||
            log_dir == "" ||
            workerNumber == 0 ||
            cws_addr == "")
        {
            cout << "config error" << endl;
            return 1;
        }

        // �Ǽǵ�ȫ�ֱ�������
        my_host = reader.Get( "SERVER", "host", "" );
        my_port = reader.Get( "SERVER", "listen", "" );

    }
    
    // 2. ��ʼ����־
    {
        server_name = server_name + "." + port_str;
        
        log_info_name = log_dir + "/" + server_name + ".INFO.";
        log_error_name = log_dir + "/" + server_name + ".ERROR.";

        FLAGS_stderrthreshold = 4; // ֻ��FATAL����־�Ż������stderr
        FLAGS_logbuflevel = -1; // �������κμ������־

        google::InitGoogleLogging( server_name.c_str() );
        google::SetLogDestination( google::GLOG_WARNING, "" ); // [XXX] �ر�WARNING
        google::SetLogDestination( google::GLOG_INFO, log_info_name.c_str() );
        google::SetLogDestination( google::GLOG_ERROR, log_error_name.c_str() );
        

        LOG(INFO) << "INIT LOG OK. PID : " << pid;
    }
    
    // 3. ��ʼ���źŴ�����
    {
        ret = SignalDeal::init();
        if (ret < 0)
        {
            LOG(ERROR) << "INIT SIGNAL error";
            exit(1);
        }
    }

    // 4. ��ʼ���������
    {
        ret = Net::init(port);
        if (ret < 0)
        {
            LOG(ERROR) << "INIT NET error";
            exit(1);
        }
        
        ret = NetWorker::init(workerNumber);
        if (ret < 0)
        {
            LOG(ERROR) << "INIT NetWorker error";
            exit(1);
        }
    }
    
    
    // 5. ��ʼ����Ϣ����
    {
        ret = Message::init(cws_addr);
        if (ret < 0)
        {
            LOG(ERROR) << "INIT Message error";
            exit(1);
        }
    }
    
    
    // 6. ��ʼ��httpclient
    {
        ret = HttpClient::init();
        if (ret < 0)
        {
            LOG(ERROR) << "INIT HttpClient error";
            exit(1);
        }
    }
    
    
    // 7. ��ʼ����
    {
        Net::run();
    }
    
    // 8. ��β
    {
        // ֹͣ�������
        Net::stop();
        // �ȴ������߳��˳�
        NetWorker::join();
    }
    
    

    LOG(INFO) << "process end!";
    return 0;
}
