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

bool HttpResponse::compile()
{
	//HttpKeyValue *ptable = HttpKeyValue::GetInstancePtr();

 //   string header;
 //   header.clear();

 //   char status[8];
 //   sprintf(status, "%d", GetStatusCode());

 //   /* HttpVer / StatusCode / ReasonPhrase */
 //   header += GetHttpVersion() + " ";
 //   header += string(status) + " ";
	//header += ptable->GetReasonDesc(GetStatusCode()) + CRLF;

 //   /* Server */
	//header += "Server:" + GetServerName() + CRLF;

 //   /* Content-type */
 //   header += "Content-type:" + GetContentType();

 //   /* Content-length */
 //   char contentLength[32];
 //   sprintf(contentLength, "%ul", GetContentLength());
	//header += "Content-length:" + string(contentLength) + CRLF;

 //   /* Connection */
	//header += "Connection:" + GetConnectionType() + CRLF;

 //   /* CrLf */
	//header += CRLF;

	//ReadBoby();

    return true;
}

void HttpResponse::readBoby()
{
	//const std::string filepath = location_.GetPath();
	//if (!IsFileExist(filepath.c_str()))
	//	return ;

	//std::string filebuf;
	//GetFileData(filepath.c_str(), filebuf);
	//SetDocument(filebuf);
}


void HttpResponse::appendToBuffer(NetBuffer* output) const
{
    char buf[32] = {0};
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
    printf("------%s-----\n", buf);
    output->write(buf);
    output->write("OK", 2);         //statusMessage_
    output->write("\r\n", 2);

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

    output->write("\r\n");
    output->write(body_);
    output->write("hello world", 11);

    const string s =  output->toString();
    printf("[[%d][%s]]\n", s.size(), s.c_str());
    std::cout << s << "\n";
}

NAMESPACE_ZL_NET_END