#include "HttpRequest.h"
#include <string.h>
#include <algorithm>
NAMESPACE_ZL_NET_START

// request line: httpmethod path httpversion
static bool processRequestLine(const char *begin, const char *end, HttpRequest* req)
{
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    string method(start, space);
    if (space != end && req->setMethod(method))
    {
        // parse url path
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                string u(start, question);
                req->setPath(u);
                u.assign(++question, space);  //不包括?
                req->setQuery(u);
            }
            else
            {
                string u(start, question);
                req->setPath(u);
            }

            // parse http version
            start = space + 1;
            if (end - start != 8)  // neither HTTP/1.1 nor HTTP/1.0
            {
                return false;
            }

            if (std::equal(start, end, "HTTP/1.1"))
                req->setVersion(HTTP_VERSION_1_1);
            else if (std::equal(start, end, "HTTP/1.0"))
                req->setVersion(HTTP_VERSION_1_0);

            return true;
        }
    }
    return false;
}

static bool processRequestHeaders(const char *begin, const char* end, HttpRequest* req)
{
    const char *nextCRLF = strstr(begin, CRLF);
    while (nextCRLF && nextCRLF < end)
    {
        const char *colon = std::find(begin, nextCRLF, ':'); //一行一行遍历
        if (colon != nextCRLF)
        {
            string field(begin, colon);
            ++colon;
            while (colon < nextCRLF && isspace(*colon))
            {
                ++colon;
            }
            string value(colon, nextCRLF);
            while (!value.empty() && isspace(value[value.size() - 1]))
            {
                value.resize(value.size() - 1);
            }
            req->addHeader(field, value);
        }
        else
        {
            // empty line, end of header
            return true;
        }

        begin = nextCRLF + 2;
        nextCRLF = strstr(begin, CRLF);
    }
    return true;
}

/*static*/bool HttpRequest::parseRequest(const char* requestLineandHeader, size_t len, HttpRequest* req)
{
    //解析Http消息头的第一行，即请求行，Method Location HttpVer ： GET /index.html HTTP/1.1
    const char* start = requestLineandHeader;
    const char* firstCRLF = strstr(start, CRLF);
    if (!firstCRLF)
        return false;

    if (!processRequestLine(start, firstCRLF, req))
        return false;

    return processRequestHeaders(firstCRLF + 2, requestLineandHeader + len, req);
}

NAMESPACE_ZL_NET_END
