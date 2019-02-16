#include <iostream>
#include <string>
#include <stdio.h>
#include "common.h"

// http://my.oschina.net/mowind/blog/349606

using namespace std;

string KEY = "starwarsstarwars";



int main(int argc, char *argv[])
{

    string url = "http://192.168.0.1/user/login";
    string querystr = "greeting=cd&name=ef";

    cout << cwsURL(url, querystr, KEY) << endl;

    return 0;
}
