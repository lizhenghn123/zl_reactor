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
    void sendPing()
    {
        std::string empty;
        sendData(conn_, WS_OPCODE_PING, empty.size(), empty.begin(), empty.end());
    }
    void sendText(const TcpConnectionPtr& conn, const char* data, size_t size)
    {
        sendData(conn, WS_OPCODE_TEXT, size, data, data + size);
    }
    void sendBinary(const TcpConnectionPtr& conn, const char* data, size_t size)
    {
        sendData(conn, WS_OPCODE_BINARY, size, data, data + size);
    }
    void close(const TcpConnectionPtr& conn, WsCloseReason code = WS_CLOSE_NORMAL, const char* reason = NULL)
    {
        uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00}; // last 4 bytes are a masking key
        std::vector<uint8_t> header(closeFrame, closeFrame+6);
        std::vector<uint8_t> txbuf;
        txbuf.insert(txbuf.end(), header.begin(), header.end());
        send(conn, txbuf);
    }

    
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp receiveTime);
    void sendHandshake(const TcpConnectionPtr& conn);
    int  parseHandshakeResponse(const TcpConnectionPtr& conn, ByteBuffer* buf);

    template<class Iterator>
    void sendData(const TcpConnectionPtr& conn, WsOpcode opcode, uint64_t message_size, Iterator message_begin, Iterator message_end);
    void send(const TcpConnectionPtr& conn, const std::vector<uint8_t>& data);

private:
    WsClient(const WsClient&);
    WsClient& operator= (const WsClient&);

    OnOpenCallback       onopen_;
    OnCloseCallback      onclose_;
    OnMessageCallback    onmessage_;

    zl::net::TcpClient*  client_;
    std::string          url_;
    bool                 useMask_;  /// true
    TcpConnectionPtr     conn_;
};


template<class Iterator>
void WsClient::sendData(const TcpConnectionPtr& conn, WsOpcode opcode, uint64_t message_size, Iterator message_begin, Iterator message_end)
{
    // TODO: Masking key should (must) be derived from a high quality random number generator, 
    // to mitigate attacks on non-WebSocket friendly middleware:
    const static uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };

    std::vector<uint8_t> header;
    header.assign(2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask_ ? 4 : 0), 0);
    header[0] = 0x80 | opcode;
    if (false) { }
    else if (message_size < 126)
    {
        header[1] = (message_size & 0xff) | (useMask_ ? 0x80 : 0);
        if (useMask_)
        {
            header[2] = masking_key[0];
            header[3] = masking_key[1];
            header[4] = masking_key[2];
            header[5] = masking_key[3];
        }
    }
    else if (message_size < 65536)
    {
        header[1] = 126 | (useMask_ ? 0x80 : 0);
        header[2] = (message_size >> 8) & 0xff;
        header[3] = (message_size >> 0) & 0xff;
        if (useMask_)
        {
            header[4] = masking_key[0];
            header[5] = masking_key[1];
            header[6] = masking_key[2];
            header[7] = masking_key[3];
        }
    }
    else   // TODO: run coverage testing here
    {
        header[1] = 127 | (useMask_ ? 0x80 : 0);
        header[2] = (message_size >> 56) & 0xff;
        header[3] = (message_size >> 48) & 0xff;
        header[4] = (message_size >> 40) & 0xff;
        header[5] = (message_size >> 32) & 0xff;
        header[6] = (message_size >> 24) & 0xff;
        header[7] = (message_size >> 16) & 0xff;
        header[8] = (message_size >>  8) & 0xff;
        header[9] = (message_size >>  0) & 0xff;
        if (useMask_)
        {
            header[10] = masking_key[0];
            header[11] = masking_key[1];
            header[12] = masking_key[2];
            header[13] = masking_key[3];
        }
    }

    // N.B. - txbuf will keep growing until it can be transmitted over the socket:
    std::vector<uint8_t> txbuf;
    txbuf.insert(txbuf.end(), header.begin(), header.end());
    txbuf.insert(txbuf.end(), message_begin, message_end);
    if (useMask_)
    {
        for (size_t i = 0; i != message_size; ++i)
        {
            *(txbuf.end() - message_size + i) ^= masking_key[i & 0x3];
        }
    }

    send(conn, txbuf);
}

}  }  }  // namespace zl { namespace net { namespace ws {
#endif  /* ZL_NET_WEBSOCKET_CLIENT_H */
