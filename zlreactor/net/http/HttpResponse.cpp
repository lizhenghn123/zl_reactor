#include "net/http/HttpResponse.h"
#include "net/http/HttpKeyValue.h"
#include "base/FileUtil.h"
#include "net/NetBuffer.h"
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
void HttpResponse::compileToBuffer(NetBuffer* output) const
{
    HttpKeyValue *ptable = HttpKeyValue::getInstancePtr();

    //status line : HttpVer / StatusCode / ReasonPhrase
    char buf[128] = {0};
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", statusCode_, 
                ptable->getStatusDesc(statusCode_).c_str());
    printf("------%s-----\n", buf);
    output->write(buf);

    // respone headers
    output->write("Server: ");       output->write(serverName_);   output->write("\r\n");
    output->write("Content-Type: "); output->write(contentType_);  output->write("\r\n");

    if (closeConnection_)
    {
        output->write("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
        output->write(buf);
        output->write("Connection: Keep-Alive\r\n");
    }

    for (std::map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        output->write(it->first);
        output->write(": ");
        output->write(it->second);
        output->write("\r\n");
    }

    // response body, maybe
    output->write("\r\n");       // 
    output->write(body_);
    output->write("hello world", 11);

    const string s =  output->toString();
    printf("[[%d][%s]]\n", s.size(), s.c_str());
    std::cout << s << "\n";
}

NAMESPACE_ZL_NET_END