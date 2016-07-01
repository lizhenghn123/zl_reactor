// ***********************************************************************
// Filename         : WebSocketServer.h
// Author           : LIZHENG
// Description      : WebSocketServer
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_NET_WEBSOCKET_SERVER_H
#define ZL_NET_WEBSOCKET_SERVER_H
#include <string>
#include "zlreactor/Define.h"
#include "zlreactor/net/TcpServer.h"
#include "zlreactor/net/CallBacks.h"
#include "zlreactor/net/http/HttpProtocol.h"
#include "zlreactor/net/websocket/WebSocket.h"
using std::string;

namespace zl
{
namespace net
{
    class EventLoop;
    class HttpRequest;
    class HttpResponse;
    class InetAddress;

namespace ws
{

class WsServer : public zl::net::TcpServer
{
public:
    typedef std::function<void (const TcpConnectionPtr&)> OnOpenCallback;
    typedef std::function<void (const TcpConnectionPtr&)> OnCloseCallback;
    typedef std::function<void (const TcpConnectionPtr&, const std::vector<char>& , Timestamp)> OnMessageCallback;

public:
    WsServer(EventLoop *loop, const InetAddress& listenAddr, const std::string& servername = "WsServer");
    ~WsServer();

public:
    void setOnOpen(const OnOpenCallback& cb)
    {
        onopen_ = cb;
    }

    void setOnClose(const OnCloseCallback& cb)
    {
        onclose_ = cb;
    }

    void setOnMessage(const OnMessageCallback& cb)
    {
        onmessage_ = cb;
    }

    void sendText(const TcpConnectionPtr& conn, const char* data, size_t size);
    void sendBinary(const TcpConnectionPtr& conn, const char* data, size_t size);
    void send(const TcpConnectionPtr& conn, const char* data, size_t size, WsFrameType type = WS_TEXT_FRAME);
    void close(const TcpConnectionPtr& conn, WsCloseReason code = WS_CLOSE_NORMAL, const char* reason = NULL);

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp receiveTime);
    void handshake(const TcpConnectionPtr& conn, ByteBuffer *buf, Timestamp receiveTime);

private:
    WsServer(const WsServer&);
    WsServer& operator = (const WsServer&);

    OnOpenCallback       onopen_;
    OnCloseCallback      onclose_;
    OnMessageCallback    onmessage_;
};

}  }  }  // namespace zl { namespace net { namespace ws {
#endif  /* ZL_NET_WEBSOCKET_SERVER_H */
