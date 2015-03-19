#include "net/http/HttpContext.h"
#include "net/NetBuffer.h"
#include "base/Timestamp.h"
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

/// parse:
/// request line     \r\n
/// request header   \r\n
///                  \r\n
/// request body
// 使用chorme浏览器请求127.0.0.1:8888/index.html时，server收到的http消息头
// GET /index.html HTTP/1.1
// Host: 127.0.0.1:8888
// Connection: keep-alive
// Cache-Control: max-age=0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*; q = 0.8
// User - Agent: Mozilla / 5.0 (Windows NT 6.1; WOW64) AppleWebKit / 537.36 (KHTML, like
// Gecko) Chrome / 35.0.1916.153 Safari / 537.36
// Accept - Encoding : gzip, deflate, sdch
// Accept - Language : zh - CN, zh; q = 0.8
// RA - Ver: 2.2.22
// RA - Sid : 7B747245 - 20140622 - 042030 - f79ea7 - 5f07a8
bool HttpContext::parseRequest(NetBuffer *buf, Timestamp receiveTime)
{
    static int count = 0;
    HttpContext *context = this;
    assert(context);
    printf("----------------parseRequest------------[%d]\n", ++count);
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (context->expectRequestLine())
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);  // 解析请求行
                if (ok)
                {
                    //context->request().setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    context->receiveRequestLine();     // 请求行解析完成，下一步要解析消息头中的参数
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (context->expectHeaders())     // 解析消息头中的参数
        {
            printf("context->expectHeaders() [%d]\n", context);
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':'); //一行一行遍历
                if (colon != crlf)
                {
                    context->request().addHeader(buf->peek(), colon, crlf);  //按行添加消息头中的参数
                }
                else
                {
                    // empty line, end of header
                    context->receiveHeaders();     // 消息头解析完成，下一步应该按get/post来区分是否解析消息体
                    hasMore = !context->gotAll();
                    printf("parse headers [%d]\n", hasMore);
                    map<string, string> headers = context->request().headers();
                    for(map<string, string>::iterator it = headers.begin(); it!=headers.end(); ++it)
                        std::cout << it->first << " : " << it->second << "\n";
                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (context->expectBody())       // 解析消息体
        {
            // FIXME:
            printf("context->expectBody() [%d]\n", context);
        }
    }
    return ok;
}

/// request line: httpmethod path httpversion
bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    HttpRequest& request = this->request();
    string method(start, space);
    if (space != end && request.setMethod(method))
    {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                string u(start, question);
                request.setPath(u);
                u.assign(question, space);
                request.setQuery(u);
            }
            else
            {
                string u(start, question);
                request.setPath(u);
            }

            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    request.setVersion(HTTP_VERSION_1_1);
                }
                else if (*(end-1) == '0')
                {
                    request.setVersion(HTTP_VERSION_1_0);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    printf("-----%d %s %d----\n", request.method(), request.path().c_str(), request.version());
    return succeed;
}

/// request header
bool HttpContext::processReqestHeader(const char *begin, const char *end)
{
    return true;
}

NAMESPACE_ZL_NET_END
