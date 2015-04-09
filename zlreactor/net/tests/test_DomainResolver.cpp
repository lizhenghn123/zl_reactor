#include <iostream>
#include "net/DomainResolver.h"
using namespace std;
using namespace zl;
using namespace zl::net;

void test_query1()
{
    HostEntry host;
    int error;
    if (DomainResolver::query("www.baidu.com", &host, &error))
    {
        std::cout << "Query success:\n" << "name: " << host.name() << "\n";

        std::cout << "Alias:";
        for (size_t i = 0; i < host.aliases().size(); ++i)
            std::cout << " " << host.aliases()[i];
        std::cout << "\n";

        std::cout << "IP:";
        for (size_t i = 0; i < host.addresses().size(); ++i)
            std::cout << " " << host.addresses()[i].ipPort();
        std::cout << "\n";
    }
    else
    {
        std::cout << "Query error: " << DomainResolver::errorString(error) << '\n';
    }
}

void test_query2()
{
    std::vector<InetAddress> addresses;
    int error;
    if (DomainResolver::resolveInetAddress("www.qq.com", &addresses, &error))
    {
        std::cout << "Query success:\nIP:";
        for (size_t i = 0; i < addresses.size(); ++i)
            std::cout << " " << addresses[i].ipPort();
        std::cout << "\n";
    }
    else
    {
        std::cout << "Query error: " << DomainResolver::errorString(error) << '\n';
    }
}

void test_query3()
{
    std::vector<InetAddress> addresses;
    int error;
    if(DomainResolver::resolveInetAddress("non-exist.domain...", &addresses, &error))
    {

    }
    else
    {
        std::cout << "error code = " << error << "\n";
        std::cout << "Query error: " << DomainResolver::errorString(error) << '\n';
    }
}

int main()
{
    test_query1();
    std::cout << "******************************************\n";
    test_query2();
    std::cout << "******************************************\n";
    test_query3();
    std::cout << "******************************************\n";
    return 0;
}
