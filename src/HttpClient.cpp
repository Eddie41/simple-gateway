#include "HttpClient.h"

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

int HttpClient::init()
{
    curl_global_init(CURL_GLOBAL_ALL); // 只能被调用一次，除非cleanup
    
    return 1;
}

int HttpClient::clean()
{
    curl_global_cleanup();

    return 1;
}

int HttpClient::post( const string &url, string &resp )
{
    int ret = 1;
    string strData;
    CURL *pCurl = NULL; // 不能在多线程中共用这个对象
    CURLcode res;

    pCurl = curl_easy_init();
    if (NULL != pCurl)
    {

        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 10); // 设置超时时间为10秒
        curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());

        // 设置回调函数
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, http_data_writer);
        //设置写数据
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&resp);

        // 执行Perform the request, res will get the return code
        res = curl_easy_perform(pCurl);
        LOG(INFO) << "返回信息:" << resp.c_str();

        if (res != CURLE_OK)
        {
            LOG(ERROR) << "curl_easy_perform() 失败:" << curl_easy_strerror(res);
            ret = -1;
        }

        curl_easy_cleanup(pCurl); // don't ever forget this
    }
    else
    {
        LOG(ERROR) << "curl_easy_init 失败";
        ret = -1;
    }
    
    return ret;
}

