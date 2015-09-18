#include "net/http/HttpResponse.h"
#include "net/http/HttpKeyValue.h"
#include "base/FileUtil.h"
#include "net/ByteBuffer.h"
NAMESPACE_ZL_NET_START

HttpResponse::HttpResponse(bool closeConn/* = true*/)
    : statusCode_(HttpStatusOk), closeConnection_(closeConn)
{
}
HttpResponse::~HttpResponse()
{
}

/// complie:
/// status line      \r\n
/// response header  \r\n
///                  \r\n
/// response body
void HttpResponse::compileToBuffer(ByteBuffer* output) const
{
    HttpKeyValue *ptable = HttpKeyValue::getInstancePtr();

    //status line : HttpVer / StatusCode / ReasonPhrase
    char buf[128] = {0};
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", statusCode_, 
                ptable->getStatusDesc(statusCode_).c_str());

    output->write(buf, strlen(buf));

    // respone headers
    output->write("Server: ");       output->write(serverName_);   output->write("\r\n");
    output->write("Content-Type: "); output->write(contentType_);  output->write("\r\n");

    if (closeConnection_)
    {
        output->write("Connection: close\r\n");
    }
    else
    {
        output->write("Connection: Keep-Alive\r\n");
    }

    for (std::map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        if(it->second.empty())
            continue;
        output->write(it->first);
        output->write(": ");
        output->write(it->second);
        output->write("\r\n");
    }

    // response body, maybe
    if(!body_.empty())
    {
        memset(buf, 0, 128);
        snprintf(buf, sizeof buf, "Content-Length: %d\r\n", static_cast<int>(body_.size()));
        output->write(buf, strlen(buf));

        output->write("\r\n");    //消息头和消息体之间有一个空行
        output->write(body_);
    }

    //const string& s =  output->toString();
    //printf("[[%d][\n%s]]\n", (int)s.size(), s.c_str());
}

NAMESPACE_ZL_NET_END
