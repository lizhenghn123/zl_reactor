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
    }
    else
    {
        conn->shutdown();
    }
}

static void printRequestHeaders(const HttpRequest& req)
{
    std::cout << "---------------print request headers---------------\n";
    std::cout << "Headers " << req.method() << " " << req.path() << std::endl;

    const HttpRequest::HeadersMap &headers = req.headers();
    for (HttpRequest::HeadersMap::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "---------------------------------------------------\n";
}

void WsClient::onMessage(const TcpConnectionPtr& conn, ByteBuffer* buf, Timestamp receiveTime)
{
    LOG_INFO("WsClient::onMessage recv data [%d]", conn->fd());
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
        std::vector<char> outbuf;
        LOG_INFO("=========");
        /*
        WsFrameType type = decodeFrameByClient(buf->peek(), buf->readableBytes(), &outbuf);
        std::cout << "getFrame : " << type << "\t" << outbuf.size() << "\t[" << outbuf.data() << "]\n";
        if(type == WS_INCOMPLETE_TEXT_FRAME || type == WS_INCOMPLETE_BINARY_FRAME)
        {
            return;
        }
        else if(type == WS_TEXT_FRAME || type == WS_BINARY_FRAME)
        {
            buf->retrieveAll();
            onmessage_(conn, outbuf, receiveTime);
        }
        else if(type == WS_CLOSE_FRAME)
        {
            conn->shutdown();
            onclose_(conn);
        }
        else
        {
            LOG_WARN("No this [%d] opcode handler", type);
        }
        */
    }
}

/// 发送websocket握手请求
void WsClient::sendHandshake(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        return;
    }
    LOG_INFO("client[%d] request sendHandshake : \n", conn->fd());
    std::string buffer = makeHandshakeRequest(url_);
    LOG_INFO("client[%d] request sendHandshake : (%s)\n", conn->fd(), buffer.c_str());
    conn->send(buffer);
}

int  WsClient::parseHandshakeResponse(const TcpConnectionPtr& conn, ByteBuffer* buf)
{
    const char* doubleCRLF = buf->findDoubleCRLF();
    if(doubleCRLF == NULL)
    {
        if(buf->readableBytes() > 1024 * 1024)  /// 超过 1 KB 仍然找不到 "\r\n\r\n"
        {
            return -1;
        }
    }
    else
    {
        HttpContext context;
        if(!context.parseRequest(buf, zl::base::Timestamp::now()))// 解析失败
        {
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
            LOG_ALERT("1111111111111111111");
            return -1;
        }
        assert(context.gotAll());
        HttpRequest& req = context.request();
        printRequestHeaders(req);
        LOG_INFO("WsServer::onMessage  parse request over.");
        return 0;
    }
    return 1;
}

void WsClient::connect()
{
    client_->enableRetry();
    client_->connect();
}

void WsClient::sendText(const TcpConnectionPtr& conn, const char* data, size_t size)
{
    send(conn, data, size, WS_TEXT_FRAME);
}

void WsClient::sendBinary(const TcpConnectionPtr& conn, const char* data, size_t size)
{
    send(conn, data, size, WS_BINARY_FRAME);
}

void WsClient::close(const TcpConnectionPtr& conn, WsCloseReason code, const char* reason)
{
    size_t len = 0;
    char buf[128];
    unsigned short code_be = hton16(code);
    memcpy(buf, &code_be, 2);
    len += 2;
    if (reason)
    {
        len += ZL_SNPRINTF(buf + 2, 124, "%s", reason);
    }

    send(conn, buf, len, WS_CLOSE_FRAME);
}

void WsClient::send(const TcpConnectionPtr& conn, const char* data, size_t size, WsFrameType type/* = TEXT_FRAME*/)
{
    assert(0 && "需要编码, 不能像serve那样直接发");
    const static size_t max_send_buf_size = 4096;
    if(size < max_send_buf_size)
    {
        char outbuf[max_send_buf_size];
        int encodesize = encodeFrameByClient(type, data, size, outbuf, max_send_buf_size);
        conn->send(outbuf, encodesize);
    }
    else
    {
        LOG_DEBUG("client[%d] data big[%d]", conn->fd(), size);
        std::vector<char> outbuf;
        outbuf.resize(size + 10);
        int encodesize = encodeFrameByClient(type, data, size, &*outbuf.begin(), outbuf.size());
        conn->send(&*outbuf.begin(), encodesize);
    }
}

}  }  }  // namespace zl { namespace net { namespace ws {
