#ifndef ZL_WEBSOCKET_H
#define ZL_WEBSOCKET_H
#include <string>
#include <vector>

namespace zl
{
namespace net
{
namespace ws
{
	// see rfc6455 : http://tools.ietf.org/rfc/rfc6455.txt

    #define WS_FRAGMENT_FIN (1 << 7)

    enum WsOpcode
    {
        WS_OPCODE_CONTINUE          = 0x0,
        WS_OPCODE_TEXT              = 0x1,
        WS_OPCODE_BINARY            = 0x2,
        WS_OPCODE_CLOSE             = 0x8,
        WS_OPCODE_PING              = 0x9,
        WS_OPCODE_PONG              = 0xa
    };

    enum WsFrameType
    {
        WS_ERROR_FRAME              = 0xFF00,
        WS_INCOMPLETE_FRAME         = 0xFE00,

        WS_OPENING_FRAME            = 0x3300,
        WS_CLOSING_FRAME            = 0x3400,

        WS_INCOMPLETE_TEXT_FRAME    = 0x01,
        WS_INCOMPLETE_BINARY_FRAME  = 0x02,

        WS_TEXT_FRAME               = 0x81,       // 128 + 1 == WS_FRAGMENT_FIN | WS_OPCODE_TEXT
        WS_BINARY_FRAME             = 0x82,       // 128 + 2
        WS_RSV3_FRAME               = 0x83,       // 128 + 3
        WS_RSV4_FRAME               = 0x84,
        WS_RSV5_FRAME               = 0x85,
        WS_RSV6_FRAME               = 0x86,
        WS_RSV7_FRAME               = 0x87,
        WS_CLOSE_FRAME              = 0x88,
        WS_PING_FRAME               = 0x89,
        WS_PONG_FRAME               = 0x8A,
    };

    enum WsCloseReason
    {
        WS_CLOSE_NORMAL             = 1000,
        WS_CLOSE_GOING_AWAY         = 1001,
        WS_CLOSE_PROTOCOL_ERROR     = 1002,
        WS_CLOSE_NOT_ALLOWED        = 1003,
        WS_CLOSE_RESERVED           = 1004,
        WS_CLOSE_NO_CODE            = 1005,
        WS_CLOSE_DIRTY              = 1006,
        WS_CLOSE_WRONG_TYPE         = 1007,
        WS_CLOSE_POLICY_VIOLATION   = 1008,
        WS_CLOSE_MESSAGE_TOO_BIG    = 1009,
        WS_CLOSE_UNEXPECTED_ERROR   = 1011
    };

    #define STATE_SHOULD_CLOSE (1 << 0)
    #define STATE_SENT_CLOSE_FRAME (1 << 1)
    #define STATE_CONNECTING (1 << 2)
    #define STATE_IS_SSL (1 << 3)
    #define STATE_CONNECTED (1 << 4)
    #define STATE_SENDING_FRAGMENT (1 << 5)
    #define STATE_RECEIVING_FRAGMENT (1 << 6)
    #define STATE_RECEIVED_CLOSE_FRAME (1 << 7)
    #define STATE_FAILING_CONNECTION (1 << 8)

    enum WsConnState
    {
        WS_CONN_CONNECTING = 0,
        WS_CONN_OPEN       = 1,
        WS_CONN_CLOSING    = 2,
        WS_COOO_CLOSED     = 3
    };

    enum 
    {
        /// Maximum size of a basic WebSocket payload
        PAYLOAD_SIZE_BASIC = 125,     // 2^7 -1

        /// Maximum size of an extended WebSocket payload (basic payload = 126)
        PAYLOAD_SIZE_EXTENDED = 0xFFFF, // 2^16 - 1, 65535

        /// Maximum size of a jumbo WebSocket payload (basic payload = 127)
        PAYLOAD_SIZE_JUMBO = 0x7FFFFFFFFFFFFFFFLL    //2^63 -1
    };

    enum WsHeaderFlag
    {
        WS_FLAG_NULL            = 0x0,
        WS_FLAG_UPGRAGE         = 0x0001,
        WS_FLAG_CONNECTION      = 0x0002,
        WS_FLAG_HOST            = 0x0004,
        WS_FLAG_ORIGIN          = 0x0008,
        WS_FLAG_SEC_ORIGIN      = 0x0010,
        WS_FLAG_SEC_KEY         = 0x0020,
        WS_FLAG_SEC_VERSION     = 0x0040,
        WS_FLAG_SEC_KEY1        = 0x0080,
        WS_FLAG_SEC_KEY2        = 0x0100,
        WS_FLAG_SEC_PROTOCOL    = 0x0200,
    };

    class WsConnection
    {
    public:
        WsConnection()
        {
            handShaked_ = false;
            state_ = WS_CONN_CONNECTING;
        }
        ~WsConnection()
        {
        }

    public:
        void setHandshaked(bool yes) { handShaked_ = yes;}
        bool handshaked() const      { return handShaked_; }

        void setConnState(WsConnState state) { state_ = state; }
        WsConnState connState() const { return state_; }

        void setPath(const std::string& s) { path_ = s; }
        const std::string& path() const    { return path_; }

        void setProtocol(const std::string& s) { protocol_ = s; }
        const std::string& protocol() const    { return protocol_; }

    private:
        bool                handShaked_;
        WsConnState         state_;
        std::string         path_;
        std::string         protocol_;
    };


    uint16_t ntoh16(uint16_t);
    uint16_t hton16(uint16_t);
    uint64_t ntoh64(uint64_t);
    uint64_t hton64(uint64_t);

    /// 设置请求server的handshake消息
    std::string makeHandshakeRequest(const std::string& url);

    /// 设置返回给client的握手响应
    /// seckey : client的"Sec-WebSocket-Key"对应值
    std::string makeHandshakeResponse(const char* seckey);

    /// 将从client发过来的数据帧进行解码
    /// inbuf      : 接收到的client发送的数据
    /// insize     : 接收到的数据大小
    /// outbuf     : 解码缓冲区
    /// return     : WsFrameType
    WsFrameType decodeFrame(const char* inbuf, int insize, std::vector<char>* outbuf);

    /// 将client发送给server的数据进行编码（加密）
    /// msg        : 发回给server的数据
    /// msglen     : 发回的数据大小
    /// outbuf     : 编码缓冲区
    /// outsize    : 编码缓冲区大小（建议outsize > insize + 10）
    int  encodeFrameByClient(WsFrameType frame_type, const char* msg, int msgsize, char* outbuf, int outsize);

    /// 将发回给client的数据进行编码
    /// msg        : 发回给client的数据
    /// msglen     : 发回的数据大小
    /// outbuf     : 编码缓冲区
    /// outsize    : 编码缓冲区大小（建议outsize > insize + 10）
    int encodeFrame(WsFrameType frame_type, const char* msg, int msgsize, char* outbuf, int outsize);

}  }  }  // namespace zl { namespace net { namespace ws {
#endif  /* ZL_WEBSOCKET_UTIL_H */
