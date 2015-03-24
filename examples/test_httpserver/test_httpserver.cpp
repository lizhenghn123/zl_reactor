#include <iostream>
#include "net/EventLoop.h"
#include "net/http/HttpServer.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "base/FileUtil.h"
using namespace std;
using namespace zl::net;

bool do_print = true;

void processHttpRequest(const HttpRequest& req, HttpResponse *resp)
{
    const string& url = req.path(); 

    resp->setServerName("test_myHttpServer");
    
    if (do_print)
    {
        std::cout << "Headers " << req.method() << " " << url << std::endl;
        std::cout << "---------------request headers---------------\n";
        const std::map<string, string> &headers = req.headers();
        for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        {
            std::cout << it->first << ": " << it->second << std::endl;
        }
    }

    if (url == "/")
    {
        resp->setStatusCode(HttpStatusOk);
        resp->setContentType("text/html");
        string now = Timestamp::now().toString();
        resp->setBody("<html><head><title>This is title</title></head>"
            "<body><h1>Hello</h1>Now is " + now + "</body></html>");
    }
    else if (url == "/favicon.ico")
    {
        resp->setStatusCode(HttpStatusOk);
        resp->setContentType("image/png");
        string picdata;
        zl::getFileData("favicon.ico", picdata);
        resp->setBody(picdata);
    }
    else if (url == "/hello")
    {
        resp->setStatusCode(HttpStatusOk);
        resp->setContentType("text/plain");
        resp->addHeader("Server", "test_myHttpServer");
        resp->setBody("hello, world!\n");
    }
    else
    {
        resp->setStatusCode(HttpStatusNotFound);
        resp->setCloseConnection(true);
    }
}

int main()
{
    EventLoop loop;
    HttpServer server(&loop, InetAddress("127.0.0.1", 8888), "myHttpServer");
    server.setThreadNum(2);
    server.setHttpCallback(processHttpRequest);
    server.setRootDir("webs");
    server.setDefaultPage("index.hrml");
    server.start();
    loop.loop();

    return 0;
}