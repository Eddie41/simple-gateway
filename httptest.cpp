#include <iostream>
#include <string>
#include <stdio.h>
#include <curl/curl.h>

// http://my.oschina.net/mowind/blog/349606

using namespace std;


size_t http_data_writer(void* data, size_t size, size_t nmemb, void* content)
{
    long totalSize = size*nmemb;
    std::string* symbolBuffer = (std::string*)content;
    if(symbolBuffer)
    {
        symbolBuffer->append((char *)data, ((char*)data)+totalSize);
    }
    return totalSize;
};

int main(int argc, char *argv[])
{

    int ret;
    string strData;
    char url[4096] = "http://192.168.0.235:8787/demo/hello?greeting=cd&name=ef";
    
    cout << sizeof(int) << endl;
    cout << sizeof(unsigned int) << endl;
    
    return 1;
    
    CURL *pCurl = NULL; // 不能在多线程中共用这个对象
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL); // 只能被调用一次，除非cleanup。因为不是线程安全，所以不要放在线程中执行
    
    {
        pCurl = curl_easy_init();
        if (NULL != pCurl)
        {

            curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10); // 设置超时时间为10秒
            curl_easy_setopt(pCurl, CURLOPT_URL, url);

            // 设置http发送的内容类型为JSON
            /*
            curl_slist *plist = curl_slist_append(NULL,
                "Accept: application/json;Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);
            */

            // 设置回调函数
            curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, http_data_writer);
            //设置写数据
            curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&strData);

            // 执行Perform the request, res will get the return code
            res = curl_easy_perform(pCurl);
            cout << "返回信息:" << strData.c_str() << endl;

            if (res != CURLE_OK)
            {
                cout << "curl_easy_perform() 失败:" << curl_easy_strerror(res) << endl;
                //return strData.c_str();
            }

            curl_easy_cleanup(pCurl);
        }
    }
    
    curl_global_cleanup();
    
    
    return 0;
}
