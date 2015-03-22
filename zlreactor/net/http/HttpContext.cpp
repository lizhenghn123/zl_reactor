#include "net/http/HttpContext.h"
#include "net/NetBuffer.h"
#include "base/Timestamp.h"
using zl::base::Timestamp;
NAMESPACE_ZL_NET_START

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

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    HttpRequest& request = this->request();
    string method(start, space);
    if (space != end && request.setHttpMethod(method))
    {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                string u(start, question);
                request.setHttpUrl(u);
                u.assign(question, space);
                request.setHttpQuery(u);
            }
            else
            {
                string u(start, question);
                request.setHttpUrl(u);
            }

            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    request.setHttpVersion(HTTP_VERSION_1_1);
                }
                else if (*(end-1) == '0')
                {
                    request.setHttpVersion(HTTP_VERSION_1_0);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    printf("-----%d %s %d----\n", request.getHttpMethod(), request.getHttpUrl().c_str(), request.getHttpVersion());
    return succeed;
}

NAMESPACE_ZL_NET_END