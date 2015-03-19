#include "HttpRequest.h"
//#include <regex>
#include <sstream>
NAMESPACE_ZL_NET_START

HttpRequest::HttpRequest()
{
}

HttpRequest::HttpRequest(const std::string& header)
{
    setHeader(header);

    parseHeader();
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::parseHeader()
{
    //解析Http消息头的第一行，即请求行，Method Location HttpVer ： GET /index.html HTTP/1.1
    std::vector<std::string> token;
    auto start = header_.begin();
    for (auto it = header_.begin(); it != header_.end(); ++it)
    {
        if (*it == ' ' || *it == '\r' || *it == '\0')
        {
            std::string tmp;
            copy(start, it, std::back_inserter(tmp));
            token.push_back( tmp );

            start = ++it;
        }
        if( *it == '\n' )
            break;
    }

    auto& method = token[0];
    std::transform(method.begin(), method.end(), method.begin(), ::tolower);
    if(strcmp(method.c_str(), "get") == 0)
        setMethod(HttpMethod::HttpGet);
    else if (strcmp(method.c_str(), "post") == 0)
        setMethod(HttpMethod::HttpPost);
    else if (strcmp(method.c_str(), "put") == 0)
        setMethod(HttpMethod::HttpPut);
    else if (strcmp(method.c_str(), "delete") == 0)
        setMethod(HttpMethod::HttpDelete);

    //location.SetRequestURI(token[1]); //location，即是请求的文件路径

    //HttpVersion，Client发过来的http协议版本号
    setVersion((token[2] == "HTTP/1.1" ? HttpVersion::HTTP_VERSION_1_1 : HttpVersion::HTTP_VERSION_1_0));

    ///* 解析剩余的选项行 */
    //std::regex expr("([a-zA-Z_-]*)\\s?:\\s?(.*)");
    //std::smatch match;
    //std::string request = header_;
    //while(std::regex_search(request, match, expr))
    //{
    //    std::string key = match[1].str();
    //    std::string value = match[2].str();
    //    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    //    headers_.insert(std::make_pair(key, value));

    //    request = match.suffix().str();
    //}

    return true;
}

NAMESPACE_ZL_NET_END
