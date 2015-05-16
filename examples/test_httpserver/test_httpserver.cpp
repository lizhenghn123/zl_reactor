#include <iostream>
#include "net/EventLoop.h"
#include "net/http/HttpServer.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "base/FileUtil.h"
using namespace std;
using namespace zl::net;

bool do_print = true;

void printRequestHeaders(const HttpRequest& req)
{
    if(!do_print)  return;

    std::cout << "---------------print request headers---------------\n";
    std::cout << "Headers " << req.method() << " " << req.path() << std::endl;
    
    const std::map<string, string> &headers = req.headers();
    for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "---------------------------------------------------\n";
}

void doGet(const HttpRequest& req, HttpResponse *resp)
{
    assert(req.method() == HttpGet);
    const string& url = req.path();
    resp->setServerName("test_myHttpServer");

    printRequestHeaders(req);

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
        zl::readFile("favicon.ico", picdata);
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

void doPost(const HttpRequest& req, HttpResponse *resp)
{
    assert(req.method() == HttpPost);
    printRequestHeaders(req);

    assert("not implement" && 0);
}

int main()
{
    EventLoop loop;
    HttpServer server(&loop, InetAddress("192.168.14.6", 8888), "myHttpServer");
    server.setMultiReactorThreads(2);
    server.setRootDir("webs");
    server.setDefaultPage("index.html");

    server.setCallback(HttpGet, doGet);
    server.setCallback(HttpPost, doPost);

    server.start();
    loop.loop();

    return 0;
}
