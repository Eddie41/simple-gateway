#ifndef __HTTPCLIENT_H_20160302__
#define __HTTPCLIENT_H_20160302__

#include <iostream>
#include <string>
#include <errno.h>
#include <curl/curl.h>
#include "common.h"

using namespace std;

class HttpClient
{
public:
    static int init();
    static int clean();
    
public:
    static int post(const string &url, string &resp);

private:
    HttpClient(){};

};




#endif
