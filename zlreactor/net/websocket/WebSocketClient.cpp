#include "zlreactor/net/websocket/WebSocketClient.h"
#include "zlreactor/Define.h"
#include "zlreactor/base/Logger.h"
#include "zlreactor/net/TcpClient.h"
#include "zlreactor/net/TcpConnection.h"
#include "zlreactor/net/http/HttpContext.h"
#include "zlreactor/net/http/HttpRequest.h"
#include "zlreactor/net/http/HttpResponse.h"
#include "zlreactor/net/websocket/WebSocket.h"
#include "zlreactor/base/StringUtil.h"
namespace zl
{
namespace net
{
namespace ws
{
const char* const kWebSocketMagicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const char* const kSecWebSocketKeyHeader = "Sec-WebSocket-Key";
const char* const kSecWebSocketVersionHeader = "Sec-WebSocket-Version";
const char* const kUpgradeHeader = "Upgrade";
const char* const kConnectionHeader = "Connection";
const char* const kSecWebSocketProtocolHeader = "Sec-WebSocket-Protocol";
const char* const kSecWebSocketAccept = "Sec-WebSocket-Accept";

WsClient::WsClient(EventLoop *loop, const InetAddress& serverAddr, const std::string& url, const string& cilentname/* = "WsClient"*/)
    : onopen_(NULL)
    , onclose_(NULL)
    , onmessage_(NULL)
    , url_(url)
    , useMask_(true)
    , conn_(NULL)
{
    client_ = new zl::net::TcpClient(loop, serverAddr, cilentname);
    client_->setConnectionCallback(std::bind(&WsClient::onConnection, this, std::placeholders::_1));
    client_->setMessageCallback(std::bind(&WsClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

WsClient::~WsClient()
{
    delete client_;
}

void WsClient::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO("WsClient::onConnection get one client %d", conn->fd());
    if (conn->connected())
    {
        conn->setContext(WsConnection());
        sendHandshake(conn);    /// 连接成功之后就开始握手
        conn_ = conn;
    }
    else
    {
        conn->shutdown();
    }
}

/// FIXME 这里面对server端的消息解析写的并不好，最好是单独写一个解码工具，用来保存中间状态，避免重复解析
void WsClient::onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp receiveTime)
{
    LOG_INFO("WsClient::onMessage recv data (fd =%d)(size = %d)", conn->fd(), buf->readableBytes());
    WsConnection* wsconn = zl::stl::any_cast<WsConnection>(conn->getMutableContext());
    assert(wsconn);
    if(!wsconn->handshaked())       /// 尚未握手
    {
        // 需要先解析 server端返回的握手信息
        int ret = parseHandshakeResponse(conn, buf);
        if(ret == 0)  /// 握手消息完成解析且正确
        {
            wsconn->setHandshaked(true);
            wsconn->setConnState(WS_CONN_OPEN);
            if(onopen_)
            {
                onopen_(conn);
            }
        }
        else if(ret < 0)
        {
            LOG_WARN("parseHandshakeResponse failure, close it(%d)", ret);
            conn->shutdown();
        }
    }
    else                                /// 已经握手，接下来就是正常收发数据了
    {
        if(buf->readableBytes() < 2)    /// Need at least 2
        {
            return;
        }
        WsHeader ws;
        const uint8_t* data = (const uint8_t*)buf->peek();   // peek, but don't consume
        ws.fin = (data[0] & 0x80) == 0x80;
        ws.opcode = (WsOpcode)(data[0] & 0x0f);
        ws.mask = (data[1] & 0x80) == 0x80;
        ws.N0 = (data[1] & 0x7f);
        ws.header_size = 2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127 ? 8 : 0) + (ws.mask ? 4 : 0);
        int i = 0;
        if (ws.N0 < 126)
        {
            ws.N = ws.N0;
            i = 2;
        }
        else if (ws.N0 == 126)
        {
            ws.N = 0;
            ws.N |= ((uint64_t) data[2]) << 8;
            ws.N |= ((uint64_t) data[3]) << 0;
            i = 4;
        }
        else if (ws.N0 == 127)
        {
            ws.N = 0;
            ws.N |= ((uint64_t) data[2]) << 56;
            ws.N |= ((uint64_t) data[3]) << 48;
            ws.N |= ((uint64_t) data[4]) << 40;
            ws.N |= ((uint64_t) data[5]) << 32;
            ws.N |= ((uint64_t) data[6]) << 24;
            ws.N |= ((uint64_t) data[7]) << 16;
            ws.N |= ((uint64_t) data[8]) << 8;
            ws.N |= ((uint64_t) data[9]) << 0;
            i = 10;
        }
        if (ws.mask)
        {
            ws.masking_key[0] = ((uint8_t) data[i + 0]) << 0;
            ws.masking_key[1] = ((uint8_t) data[i + 1]) << 0;
            ws.masking_key[2] = ((uint8_t) data[i + 2]) << 0;
            ws.masking_key[3] = ((uint8_t) data[i + 3]) << 0;
        }
        else
        {
            ws.masking_key[0] = 0;
            ws.masking_key[1] = 0;
            ws.masking_key[2] = 0;
            ws.masking_key[3] = 0;
        }
        LOG_DEBUG("WsHeader: fin=%d, opcode=%d, mask=%d, N0=%d, N=%d, header_size=%d", 
                    ws.fin,  ws.opcode, ws.mask, ws.N0, ws.N, ws.header_size);

        // We got a whole message, now do something with it:
        if(buf->readableBytes() < ws.header_size + ws.N)   /// 实际数据还未到达
        {
            return;
        }

        std::vector<char> rxbuf;
        rxbuf.reserve(ws.N);
        if (false) { }
        else if (ws.opcode == WS_OPCODE_PONG) { }
        else if (ws.opcode == WS_OPCODE_CLOSE)
        {
            close(conn);
        }
        else if (ws.opcode == WS_OPCODE_PING)
        {
            rxbuf.assign(buf->peek(), buf->peek() + ws.N);
            if (ws.mask)
            {
                for (size_t i = 0; i < ws.N; ++i)
                {
                    rxbuf[i] ^= ws.masking_key[i & 0x3];
                }
            }
            sendData(conn, WS_OPCODE_PONG, rxbuf.size(), rxbuf.begin(), rxbuf.end());
        }
        else if (ws.opcode == WS_OPCODE_TEXT || ws.opcode == WS_OPCODE_BINARY || ws.opcode == WS_OPCODE_CONTINUE)
        {
            LOG_INFO("========= (%d)(%d)(%d)(%s)", ws.header_size, ws.N, buf->readableBytes(), buf->toString().c_str());
            rxbuf.assign(buf->peek() + ws.header_size, buf->peek() + ws.header_size + ws.N);
            LOG_INFO("111111 : %s", rxbuf.data());
            if (ws.mask)
            {
                for (size_t i = 0; i < ws.N; ++i)
                {
                    rxbuf[i] ^= ws.masking_key[i & 0x3];
                }
            }
            if (ws.fin && onmessage_)
            {
                onmessage_(conn, rxbuf, zl::base::Timestamp::now());               
            }
        }
        else
        {
            LOG_ERROR("ERROR: Got unexpected WebSocket message.\n");
            close(conn);
        }
        buf->retrieve(ws.header_size + ws.N);
        LOG_INFO("========= (%s)(%d)", buf->toString().c_str(), buf->readableBytes());
    }
}

/// 发送websocket握手请求
void WsClient::sendHandshake(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        return;
    }
    std::string buffer = makeHandshakeRequest(url_);
    LOG_DEBUG("client[%d] request sendHandshake : (%s)\n", conn->fd(), buffer.c_str());
    conn->send(buffer);
}

int  WsClient::parseHandshakeResponse(const TcpConnectionPtr& conn, ByteBuffer* buf)
{
    const char* doubleCRLF = buf->findDoubleCRLF();
    if(doubleCRLF == NULL)
    {
        if(buf->readableBytes() > 1024 * 1024)  /// 超过 1 KB 仍然找不到 "\r\n\r\n"
        {
            LOG_ERROR("Cannot find the double crlf in server handshake response");
            return -1;
        }
    }
    else
    {
        /// TODO 这里需要解析handshake响应，以判断server端是否能够正常处理
        /***
            HTTP/1.1 101 Switching Protocols
            Upgrade: WebSocket
            Connection: Upgrade
            Sec-WebSocket-Version: 13
            Sec-WebSocket-Accept: tlfzFb2mOM86dj/ZWpxF0VCvy6s=

        ***/
        const std::string response(buf->peek(), doubleCRLF + 4);
        if(strcasestr(response.c_str(), "HTTP/1.1 101 Switching Protocols") == NULL
            || strcasestr(response.c_str(), "Upgrade: WebSocket") == NULL
            || strcasestr(response.c_str(), "Connection: Upgrade") == NULL)
        {
            LOG_ERROR("server handshake response is invalid(%s)", response.c_str());
            return -1;
        }
        
        buf->retrieve(doubleCRLF + 4 - buf->peek());
        //LOG_ALERT("%d, %d, %d", buf->readableBytes(), response.size(), doubleCRLF - buf->peek());
        LOG_INFO("WsClient::onMessage  parse request over.");
        LOG_ALERT("####### %d", buf->readableBytes());
        return 0;
    }
    return 1;
}

void WsClient::connect()
{
    client_->enableRetry();
    client_->connect();
}

void WsClient::send(const TcpConnectionPtr& conn, const std::vector<uint8_t>& data)
{
    conn->send(&*data.begin(), data.size());
}

}  }  }  // namespace zl { namespace net { namespace ws {
