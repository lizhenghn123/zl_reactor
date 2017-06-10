// ***********************************************************************
// Filename         : WebSocketClient.h
// Author           : LIZHENG
// Description      : WebSocketClient
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_NET_WEBSOCKET_CLIENT_H
#define ZL_NET_WEBSOCKET_CLIENT_H
#include <string>
#include "zlreactor/Define.h"
#include "zlreactor/net/CallBacks.h"
#include "zlreactor/net/http/HttpProtocol.h"
#include "zlreactor/net/websocket/WebSocket.h"

namespace zl
{
namespace net
{
    class EventLoop;
    class HttpRequest;
    class HttpResponse;
    class InetAddress;
    class TcpClient;

namespace ws
{

class WsClient
{
public:
    typedef std::function<void (const TcpConnectionPtr&)> OnOpenCallback;
    typedef std::function<void (const TcpConnectionPtr&)> OnCloseCallback;
    typedef std::function<void (const TcpConnectionPtr&, const std::vector<char>& , Timestamp)> OnMessageCallback;

public:
    WsClient(EventLoop *loop, const InetAddress& serverAddr, const std::string& url, const std::string& cilentname = "WsClient");
    ~WsClient();

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

    void connect();
    void sendText(const TcpConnectionPtr& conn, const char* data, size_t size);
    void sendBinary(const TcpConnectionPtr& conn, const char* data, size_t size);
    void send(const TcpConnectionPtr& conn, const char* data, size_t size, WsFrameType type = WS_TEXT_FRAME);
    void close(const TcpConnectionPtr& conn, WsCloseReason code = WS_CLOSE_NORMAL, const char* reason = NULL);

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp receiveTime);
    void sendHandshake(const TcpConnectionPtr& conn);
    int  parseHandshakeResponse(const TcpConnectionPtr& conn, ByteBuffer* buf);

private:
    WsClient(const WsClient&);
    WsClient& operator = (const WsClient&);

    OnOpenCallback       onopen_;
    OnCloseCallback      onclose_;
    OnMessageCallback    onmessage_;

    zl::net::TcpClient*  client_;
    std::string          url_;
};

}  }  }  // namespace zl { namespace net { namespace ws {
#endif  /* ZL_NET_WEBSOCKET_CLIENT_H */
