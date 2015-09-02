#include <iostream>
#include <string>
#include "base/ConfigFile.h"
using namespace std;

void test_ConfigFile()
{
    zl::base::ConfigFile config("test.conf", "=", "#");
    std::string serverIP = config.read<std::string>("ServerIP", "127.0.0.1");    // if not exist, set default 127.0.0.1
    short serverPort = config.read<short>("ServerPort", 80);                     // if not exist, set default 80
    std::cout << serverIP << " : " << serverPort << "\n";

    int logsize = config.read<int>("LogSize");   // if happen more than once, get the last one
    std::cout << logsize << "\n";

    int  threads = config.read<int>("threads");  // if not exist, throw an exception
}
int main()
{
    test_ConfigFile();
    system("pause");
}