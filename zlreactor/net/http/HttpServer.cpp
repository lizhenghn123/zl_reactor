#include "net/http/HttpServer.h"
#include "base/ZLog.h"
#include "net/TcpConnection.h"
#include "net/http/HttpContext.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
using namespace zl::base;
NAMESPACE_ZL_NET_START

HttpServer::HttpServer(EventLoop *loop, const InetAddress& listenAddr, const string& servername/* = "HttpServer"*/)
    : TcpServer(loop, listenAddr, servername)
{
    setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
     LOG_INFO("HttpServer::onConnection get one client %d", conn->fd());
     if (conn->connected())
     {
         conn->setContext(HttpContext());
     }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, Timestamp receiveTime)
{
     LOG_INFO("HttpServer::onConnection recv data [%d][%s]", conn->fd(), buf->toString().c_str());

     HttpContext *context = zl::stl::any_cast<HttpContext>(conn->getMutableContext());
     assert(context);
     if(!context->parseRequest(buf, receiveTime))
     {
         conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
         conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
         conn->shutdown();
     }

     if (context->gotAll())
     {
         LOG_INFO("HttpServer::onMessage  parse request over.");
         response(conn, context->request());
         context->reset();
     }
}

void HttpServer::response(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    //conn->send("HTTP/1.1 200 OK\r\n\r\n");
    //conn->send("hello world");
    //conn->shutdown();

    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" || (req.getHttpVersion() == HTTP_VERSION_1_0 && connection != "Keep-Alive");

    HttpResponse response(close);
    response.setStatusCode(HttpStatusOk);
    response.setServerName("MyHttpServer");

    NetBuffer buf;
    response.appendToBuffer(&buf);
    printf("[%s]\n", buf.toString().c_str());
    conn->send(&buf);
    
    LOG_INFO("HttpServer::response send data [%d]", conn->fd());
    //if (response.closeConnection())
    {
        LOG_INFO("HttpServer::response close this[%d]", conn->fd());
        conn->shutdown();
    }
}

NAMESPACE_ZL_NET_END
