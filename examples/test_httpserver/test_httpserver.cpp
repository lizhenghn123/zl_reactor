#include <iostream>
#include <string>
#include "net/EventLoop.h"
#include "net/http/HttpServer.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "base/FileUtil.h"
#include "base/Logger.h"
using namespace std;
using namespace zl::net;

// ab -k -n 10000 -c 2000 http://127.0.0.1:8888/index.html

bool do_print = false;

HttpServer* g_httpserver;

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

std::string g_indexData;

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
    else if (url == "/index.html")
    {
        resp->setStatusCode(HttpStatusOk);
        resp->setContentType("text/plain");
        if (0)   // 每一次都从本地路径读取
        {
            string path(g_httpserver->rootDir() + url);
            string data;
            zl::FileUtil::readFile(path.c_str(), data);
            resp->setBody(data);
        }
        else    // 提前预读取
        {
            resp->setBody(g_indexData);
        }
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
    LOG_DISABLE_ALL;

    EventLoop loop;
    HttpServer server(&loop, InetAddress("127.0.0.1", 8888), "myHttpServer");
    //server.setMultiReactorThreads(2);
    server.setRootDir("/var/www/html");
    server.setDefaultPage("index.html");

    server.setCallback(HttpGet, doGet);
    server.setCallback(HttpPost, doPost);

    g_httpserver = &server;

    string path(g_httpserver->rootDir() + "/index.html");
    zl::FileUtil::readFile(path.c_str(), g_indexData);

    server.start();
    loop.loop();

    return 0;
}
