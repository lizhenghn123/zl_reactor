#include <iostream>
#include <assert.h>
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "net/http/HttpContext.h"
#include "net/ByteBuffer.h"
#include "base/FileUtil.h"
using namespace std;
using namespace zl::net;

#define ZL_CHECK_TRUE(expr)           assert(expr)
#define ZL_CHECK_EQUAL(expr1, expr2)  assert(expr1 == expr2)

void testParseRequest1()
{
	ByteBuffer input;
	input.write("GET /index.html HTTP/1.1\r\n"
                "Host: www.baidu.com\r\n"
                "\r\n");
	HttpContext context;
	ZL_CHECK_TRUE(context.parseRequest(&input, Timestamp::now()));
	ZL_CHECK_TRUE(context.gotAll());
	const HttpRequest& request = context.request();
	ZL_CHECK_EQUAL(request.method(), HttpGet);
	ZL_CHECK_EQUAL(request.path(), string("/index.html"));
	ZL_CHECK_EQUAL(request.version(), HTTP_VERSION_1_1);
	ZL_CHECK_EQUAL(request.getHeader("Host"), string("www.baidu.com"));
	ZL_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
}

void testParseRequest2()
{
	string all("GET /index.html HTTP/1.1\r\n"
               "Host: www.baidu.com\r\n"
               "\r\n");

    for(size_t sz1 = 0; sz1 < all.size(); ++sz1)
	{
		HttpContext context;
		ByteBuffer input;
		input.write(all.c_str(), sz1);
		ZL_CHECK_TRUE(context.parseRequest(&input, Timestamp::now()));
		ZL_CHECK_TRUE(!context.gotAll());

		size_t sz2 = all.size() - sz1;
		input.write(all.c_str() + sz1, sz2);
		ZL_CHECK_TRUE(context.parseRequest(&input, Timestamp::now()));
		ZL_CHECK_TRUE(context.gotAll());
		const HttpRequest& request = context.request();
		ZL_CHECK_EQUAL(request.method(), HttpGet);
		ZL_CHECK_EQUAL(request.path(), string("/index.html"));
		ZL_CHECK_EQUAL(request.version(), HTTP_VERSION_1_1);
		ZL_CHECK_EQUAL(request.getHeader("Host"), string("www.baidu.com"));
		ZL_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
  }
}

void testParseRequest3()
{
	ByteBuffer input;
	input.write("GET /index.html HTTP/1.1\r\n"
                "Host: www.baidu.com\r\n"
                "User-Agent:\r\n"
                "Accept-Encoding: utf-8\r\n"
                "\r\n");

	HttpContext context;
	ZL_CHECK_TRUE(context.parseRequest(&input, Timestamp::now()));
	ZL_CHECK_TRUE(context.gotAll());
	const HttpRequest& request = context.request();
	ZL_CHECK_EQUAL(request.method(), HttpGet);
	ZL_CHECK_EQUAL(request.path(), string("/index.html"));
	ZL_CHECK_EQUAL(request.version(), HTTP_VERSION_1_1);
	ZL_CHECK_EQUAL(request.getHeader("Host"), string("www.baidu.com"));
	ZL_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
	ZL_CHECK_EQUAL(request.getHeader("Accept-Encoding"), string("utf-8"));
}

void testParseRequestPost()
{
	ByteBuffer input;
	input.write("POST /index.html HTTP/1.1\r\n"
                "Host: www.baidu.com\r\n"
                "User-Agent:\r\n"
                "Accept-Encoding: utf-8\r\n"
                "Content-Length: 59\r\n"
                "\r\n"
                "I want to get index.html.\r\n"        // 27
                "Can you give me?\r\n"                 // 18
                "Thank a lot!\r\n"                     // 14
                );
    //cout << input.toString() << "\n\n";

	HttpContext context;

	ZL_CHECK_TRUE(context.parseRequest(&input, Timestamp::now()));
	ZL_CHECK_TRUE(context.gotAll());
	const HttpRequest& request = context.request();
	ZL_CHECK_EQUAL(request.method(), HttpPost);
	ZL_CHECK_EQUAL(request.path(), string("/index.html"));
	ZL_CHECK_EQUAL(request.version(), HTTP_VERSION_1_1);
	ZL_CHECK_EQUAL(request.getHeader("Host"), string("www.baidu.com"));
	ZL_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
	ZL_CHECK_EQUAL(request.getHeader("Accept-Encoding"), string("utf-8"));
    
}

int main()
{
    //testParseRequest1();        cout << "----------------------------\n";

    //testParseRequest2();        cout << "----------------------------\n";

    //testParseRequest3();        cout << "----------------------------\n";

    testParseRequestPost();     cout << "----------------------------\n";
    return 0;
}
