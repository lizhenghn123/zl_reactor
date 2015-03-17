#include "net/http/HttpServer.h"
NAMESPACE_ZL_NET_START

HttpServer::HttpServer(EventLoop *loop, const InetAddress& listenAddr, const string& servername/* = "HttpServer"*/)
    : TcpServer(loop, listenAddr, servername)
{
    setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{

}

void HttpServer::onMessage(const TcpConnectionPtr& conn, NetBuffer *buf, Timestamp receiveTime)
{

}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{

}

//int HttpServer::SendString(ZL_SOCKET socket, const std::string& str)
//{
//    return Send(socket, (void*)str.c_str(), str.length());
//}
//
//bool HttpServer::OnConnect(ClientData client)
//{
//	std::string header;
//	std::string document;
//	bool received = false;
//
//	//下面这个while循环是接收http请求头的，写得不好。
//	while (!received)
//	{
//		char buf[INBUF_SIZE + 1];
//		int length = Recv(client.socket_, buf, INBUF_SIZE, false);
//		if (length == -1)
//			break;
//		buf[length] = '\0';
//
//		header += buf;
//
//		/* \r\n\r\n ?? -> ?? ?? */
//		int headerLength = header.length();
//
//		if (headerLength >= 3)
//		{
//			const char *cstr = header.c_str();
//			int sidx = max((headerLength - length) - 3, 0);
//			int eidx = headerLength;
//
//			for (int i = sidx; i < eidx; i++)
//			{
//				int j;
//				for (j = 0; j < 4; j++)
//				{
//					if (cstr[i + j] != DOUBLE_CRLF[j])
//						break;
//				}
//
//				if (j == 4)     // 遇到\r\n\r\n时表示读完消息头了
//				{
//					received = true;
//
//					document = header.substr(i + 4);
//					header = header.erase(i + 4);
//
//					break;
//				}
//			}
//		}
//	}
//
//	requestCounter.fetch_add(1);
//
//	printf("=====\n%s\n=====\n", header.c_str());
//	char* dddd = const_cast<char*>(header.c_str());
//	/* get the method */
//	char *token = strtok(dddd, " ");
//	char *uri = strtok(0, " ");
//	char file[64];
//	sprintf(file, ".%s", uri);
//
//	//if( ! request.empty() ){
//	HttpRequest request(header);
//
//	unsigned long targetLength = request.GetContentLength();
//	received = false;
//
//	if (targetLength > 0)  // 根据消息头中解析出来的消息体长度，继续读取数据
//	{
//		while (!received)
//		{
//			char buf[INBUF_SIZE + 1];
//			int length = Recv(client.socket_, buf, INBUF_SIZE, false);
//			if (length == -1) break;
//			buf[length] = '\0';
//
//			document += buf;
//			if (document.length() >= targetLength)
//			{
//				received = true;
//			}
//		}
//		//printf("contentLEngth : %lu\n\n", request.getContentLength());
//		//printf("%s\n", document.c_str() );
//	}
//
//	HttpResponse response;
//	//response.SetConnectionType("close");
//	response.SetConnectionType(request.GetConnection()); // close or keep-alive
//	response.SetHttpVersion(HTTP_VERSION_1_1);
//	response.SetStatusCode(HttpStatusCode::HttpStatusOk);
//	response.SetServerName("hi");
//	response.SetLocation(request.GetLocation());
//	response.Compile();
//	//response.SetDocument("<h1>hi</h1>");
//
//	SendResponse(client.socket_, response);
//
//	/*
//	rootRouter.route(
//	header.location->begin(), header );
//	*/
//	return true;
//}
//
//bool HttpServer::SendResponse(SOCKET socket, HttpResponse& response)
//{
//    SendString(socket, response.GetHeader());
//    SendString(socket, response.GetDocument());
//
//    Close(socket);
//
//    return true;
//}

NAMESPACE_ZL_NET_END
