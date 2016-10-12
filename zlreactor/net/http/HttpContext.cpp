#include "zlreactor/net/http/HttpContext.h"
#include "zlreactor/net/ByteBuffer.h"
#include "zlreactor/base/Timestamp.h"
#include "zlreactor/base/StringUtil.h"
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
bool HttpContext::parseRequest(ByteBuffer *buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (this->expectRequestLine())
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);  // 解析请求行
                if (ok)
                {
                    this->request().setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2);
                    this->receiveRequestLine();     // 请求行解析完成，下一步要解析消息头中的参数
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
        else if (this->expectHeaders())     // 解析消息头中的参数
        {
            //printf("context->expectHeaders() [%p]\n", this);
            const char* crlf = buf->findCRLF();
            if (crlf)    //按行添加消息头中的参数
            {
                //const char *colon = std::find(buf->peek(), crlf, ':'); //一行一行遍历
                if(!processReqestHeader(buf->peek(), crlf))  // 消息头解析完成
                {
                    // empty line, end of header
                    this->receiveHeaders();     //下一步应该按get/post来区分是否解析消息体
                    hasMore = !this->gotAll();
                    //printf("parse headers [%d][%d]\n", hasMore, state_);
                    //const map<string, string>& headers = this->request().headers();
                    //for(map<string, string>::const_iterator it = headers.begin(); it!=headers.end(); ++it)
                    //    printf("HttpContext::parseRequest headers [%s = %s]\n", it->first.c_str(), it->second.c_str());
                }
                buf->retrieveUntil(crlf + 2);
                //printf("context->expectHeaders() [%s]", buf->toString().c_str());
                //if(this->expectBody())   // 如果是解析header完成，且需要解析body，过滤掉下面的空白行（\r\n）
                //     buf->retrieve(2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if (this->expectBody())       // 解析消息体  // FIXME:
        {
            int bufsize = static_cast<int>(buf->readableBytes());
            string value = request_.getHeader("Content-Length");
            assert(!value.empty());
            int content_len = zl::base::strTo<int>(value);
            printf("context->expectBody() [%p][%d][%d]\n", this, bufsize, content_len);
            printf("context->expectBody() [%s]", buf->toString().c_str());

            if(bufsize >= content_len)
            {
                this->receiveBody();
                assert(gotAll());
                printf("parse all data from request[%d]", state_);
                hasMore = false;
            }
            else  // FIXME : 如果对方发送数据不够，或者有意不再发送数据，会有问题
            {
                break;
            }
        }
        else  // gotAll
        {
            assert(0 && "There is no more data for receiving");
        }
    }
    return ok;
}

/// request line: httpmethod path httpversion
bool HttpContext::processRequestLine(const char *begin, const char *end)
{
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    HttpRequest& request = this->request();
    string method(start, space);
    if (space != end && request.setMethod(method))
    {
        // parse url path
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

            // parse http version
            start = space + 1;
            if(end - start != 8)  // neither HTTP/1.1 nor HTTP/1.0
            {
                  return false;
            }

            if(std::equal(start, end, "HTTP/1.1"))
                request.setVersion(HTTP_VERSION_1_1);
            else if(std::equal(start, end, "HTTP/1.0"))
                request.setVersion(HTTP_VERSION_1_0);

            return true;
        }
    }
    return false;
}

/// request header
bool HttpContext::processReqestHeader(const char *begin, const char *end)
{
    //while(true)    //TODO
    {
        const char *colon = std::find(begin, end, ':'); //一行一行遍历
        if (colon != end)
        {
            string field(begin, colon);
            ++colon;
            while (colon < end && isspace(*colon))
            {
                ++colon;
            }
            string value(colon, end);
            while (!value.empty() && isspace(value[value.size()-1]))
            {
                value.resize(value.size()-1);
            }
            request_.addHeader(field, value);
            return true;
        }
        else
        {
            // empty line, end of header
            return false;
        }
    }
}

NAMESPACE_ZL_NET_END
