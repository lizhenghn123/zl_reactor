#include "net/http/HttpResponse.h"
#include "net/http/HttpKeyValue.h"
#include "base/FileUtil.h"
NAMESPACE_ZL_NET_START

HttpResponse::HttpResponse()
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

NAMESPACE_ZL_NET_END