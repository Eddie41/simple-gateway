#ifndef __COMMON_H_20140910__
#define __COMMON_H_20140910__

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "time.h"
#include <sys/time.h>
#include "glog/logging.h"

using namespace std;



extern string my_host;
extern string my_port;



string N2S (const int i);

string cwsURL(const string &url, const string &querystr, const string &cwsKey);

int iMD5(const char* key, const int &len, string &sMD5);
ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, const void *ptr, size_t n);


inline string getPackValue(const map<string, string> &pack, const string &key, const string def="")
{
    map<string, string>::const_iterator it = pack.find(key);

    // 当键不存在，或为空时，返回默认值
    if(pack.end() == it || "" == it->second)
    {
        return def;
    }
    return it->second;
}

// 包结构转为字符串
string pack2string(const map<string, string> &pack);
string param2string(const map<string, string> &req, const map<string, string> &resp);
string list2string(const vector< map< string, string > >  &list);



inline vector<string> split_string(const string &ori_str,const string &pattern)
{
    string str = ori_str; // copy

    string::size_type pos;
    vector<string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();
    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(int(pos)<size)
        {
            string s=str.substr(i,pos-i);
            if(s.size() <= 0)
            {
                continue;
            }
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
     }

    return result;
}


namespace HttpUtility
{
   
    typedef unsigned char BYTE;
 
    inline BYTE toHex(const BYTE &x)
    {
        return x > 9 ? x -10 + 'A': x + '0';
    }
    
    inline BYTE toHexLow(const BYTE &x)
    {
        return x > 9 ? x -10 + 'a': x + '0';
    }
    
    inline BYTE fromHex(const BYTE &x)
    {
        return isdigit(x) ? x-'0' : x-'A'+10;
    }
    
    inline BYTE fromHexLow(const BYTE &x)
    {
        return isdigit(x) ? x-'0' : x-'a'+10;
    }
    
    inline string URLEncode(const string &sIn)
    {
        string sOut;
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {      
            BYTE buf[4];
            memset( buf, 0, 4 );
            if( isalnum( (BYTE)sIn[ix] ) )
            {      
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
            //{
            //    buf[0] = '+';
            //}
            else
            {
                buf[0] = '%';
                buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
                buf[2] = toHex( (BYTE)sIn[ix] % 16);
            }
            sOut += (char *)buf;
        }
        return sOut;
    };
    
    inline string URLEncodeLow(const string &sIn)
    {
        string sOut;
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {      
            BYTE buf[4];
            memset( buf, 0, 4 );
            if( isalnum( (BYTE)sIn[ix] ) )
            {      
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
            //{
            //    buf[0] = '+';
            //}
            else
            {
                buf[0] = '%';
                buf[1] = toHexLow( (BYTE)sIn[ix] >> 4 );
                buf[2] = toHexLow( (BYTE)sIn[ix] % 16);
            }
            sOut += (char *)buf;
        }
        return sOut;
    };

    inline string URLDecode(const string &sIn)
    {
        string sOut;
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {
            BYTE ch = 0;
            if(sIn[ix]=='%')
            {
                ch = (fromHex(sIn[ix+1])<<4);
                ch |= fromHex(sIn[ix+2]);
                ix += 2;
            }
            else if(sIn[ix] == '+')
            {
                ch = '+';
            }
            else
            {
                ch = sIn[ix];
            }
            sOut += (char)ch;
        }
        return sOut;
    }
}

// 时间统计 [2009-08-20 13:51:33]
class TimeStst
{
public:
    TimeStst();

    // 两次时间差值
    long GetDiff();

private:
    // 设定开始时间
    void Begin();

    // 设定终止时间
    void End();

    // 计算差值
    int Diff(struct timeval &diff);

private:
    struct timeval m_begin;
    struct timeval m_end;
};

// 自动加解锁
class MutexLock
{
        /* */

public:
        MutexLock (pthread_mutex_t &lock);
        ~MutexLock ();

        /* */

private:
        pthread_mutex_t *mLock;
};

#endif
