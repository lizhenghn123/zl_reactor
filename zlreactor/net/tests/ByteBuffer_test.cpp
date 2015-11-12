#include <iostream>
#include "net/ByteBuffer.h"
using namespace std;
using zl::net::ByteBuffer;

///see https://github.com/chenshuo/muduo/blob/master/muduo/net/tests/Buffer_unittest.cc
namespace test_ByteBuffer
{
    void testBufferAppendRetrieve()
    {
        ByteBuffer buf;
        assert(buf.readableBytes() == 0);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);

        const string str(200, 'x');
        buf.write(str);
        assert(buf.readableBytes() == str.size());
        assert(buf.writableBytes() == ByteBuffer::kInitialSize - str.size());
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);

        const string str2 =  buf.retrieveAsString(50);
        assert(str2.size() == 50);
        assert(buf.readableBytes() == str.size() - str2.size());
        assert(buf.writableBytes() == ByteBuffer::kInitialSize - str.size());
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend + str2.size());

        buf.write(str);
        assert(buf.readableBytes() == 2*str.size() - str2.size());
        assert(buf.writableBytes() == ByteBuffer::kInitialSize - 2*str.size());
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend + str2.size());

        const string str3 =  buf.retrieveAllAsString();
        assert(str3.size() == 350);
        assert(buf.readableBytes() == 0);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);
    }

    void testBufferGrow()
    {
        ByteBuffer buf;
        buf.write(string(400, 'y'));
        assert(buf.readableBytes() == 400);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-400);

        buf.retrieve(50);
        assert(buf.readableBytes() == 350);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-400);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend+50);

        buf.write(string(1000, 'z'));
        assert(buf.readableBytes() == 1350);
        assert(buf.writableBytes() == 0);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend+50); // FIXME

        buf.retrieveAll();
        assert(buf.readableBytes() == 0);
        assert(buf.writableBytes() == 1400); // FIXME
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);
    }

    void testBufferInsideGrow()
    {
        ByteBuffer buf;
        buf.write(string(800, 'y'));
        assert(buf.readableBytes() == 800);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-800);

        buf.retrieve(500);
        assert(buf.readableBytes()== 300);
        assert(buf.writableBytes()== ByteBuffer::kInitialSize-800);
        assert(buf.prependableBytes()== ByteBuffer::kCheapPrepend+500);

        buf.write(string(300, 'z'));
        assert(buf.readableBytes() == 600);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-600);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);
    }

    void testBufferShrink()
    {
        ByteBuffer buf;
        buf.write(string(2000, 'y'));
        assert(buf.readableBytes() == 2000);
        assert(buf.writableBytes() == 0);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);

        buf.retrieve(1500);
        assert(buf.readableBytes() ==  500);
        assert(buf.writableBytes() ==  0);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend+1500);

        buf.shrink(0);
        assert(buf.readableBytes() == 500);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-500);
        assert(buf.retrieveAllAsString() == string(500, 'y'));
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);
    }

    void testBufferPrepend()
    {
        ByteBuffer buf;
        buf.write(string(200, 'y'));
        assert(buf.readableBytes() == 200);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-200);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend);

        int x = 0;
        buf.prepend(&x, sizeof x);
        assert(buf.readableBytes() == 204);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize-200);
        assert(buf.prependableBytes() == ByteBuffer::kCheapPrepend - 4);
    }

    void testBufferReadInt()
    {
        ByteBuffer buf;
        buf.write("HTTP");

        assert(buf.readableBytes() == 4);
        assert(buf.peek<int8_t>() == 'H');
        assert(buf.peek<int8_t>() == 'H');
        int top16 = buf.peek<short>();
        assert(top16 == 'H'*256 + 'T');
        //assert(top16 == 'H'+ 'T'*256 );
        assert(buf.peek<int32_t>() == top16*65536 + 'T'*256 + 'P');
        //assert(buf.peekInt32() == 'H'+ 'T'*256 + 'T' * 256 * 256 + 'P' * 256 * 256 * 256);

        assert(buf.read<int8_t>() == 'H');
        assert(buf.read<int16_t>() == 'T'*256 + 'T');
        assert(buf.read<int8_t>() == 'P');
        assert(buf.readableBytes() == 0);
        assert(buf.writableBytes() == ByteBuffer::kInitialSize);

        buf.write<int8_t>(-1);
        buf.write<short>(-1);
        buf.write<int32_t>(-1);
        assert(buf.readableBytes() == 7);
        assert(buf.read<int8_t>() == -1);
        assert(buf.read<int32_t>() == -1);
        assert(buf.read<short>() == -1);
        //buf.retrieve<int32_t>();
        //buf.readInt32();
        //buf.retrieve(4);
    }
    void  testBufferReadNum()
    {
        ByteBuffer buf;
        buf.write<int>(1234);
        buf.write<float>(56.123406456);
        buf.write<double>(85679.1234567);
        buf.write<bool>(1);

        int n = buf.read<int>();
        float f = buf.read<float>();
        double d = buf.read<double>();
        bool b = buf.read<bool>();

        cout << n << "\t" << f << "\t" << d << "\t" << b << "\n";
        printf("%d\t%f\t%lf\t%d\n", n, f, d, b);

        double d2 = 23434.23567;
        cout << "-----------" << d2 << "\n";

        {
            ByteBuffer buf;
            buf.write("asdfghj1,.45!");
            std::string s = buf.toString();
            cout << s << "\n";
        }
    }
}

int main()
{
    if( 2 )
    {
        test_ByteBuffer::testBufferAppendRetrieve();
        cout << "------------------\n";
        test_ByteBuffer::testBufferGrow();
        cout << "------------------\n";
        test_ByteBuffer::testBufferInsideGrow();
        cout << "------------------\n";
        test_ByteBuffer::testBufferShrink();
        cout << "------------------\n";
        test_ByteBuffer::testBufferPrepend();
        cout << "------------------\n";
        test_ByteBuffer::testBufferReadInt();
        cout << "------------------\n";
        test_ByteBuffer::testBufferReadNum();
    }

    {
        ByteBuffer buf;
        buf.write(string(100000, 'x'));
        const char* null = NULL;
        assert(buf.findEOL() == null);
        assert(buf.findEOL(buf.peek()+90000)  == null);
    }

    {
        ByteBuffer *output = new ByteBuffer;
        char buf[128] = {0};
        snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", 200, "OK");
        //printf("------%s-----\n", buf);
        output->write(buf, strlen(buf));
        
        // respone headers
        output->write("Server: ");       output->write("test_myHttpServer");   output->write("\r\n");
        output->write("Content-Type: "); output->write("text/html");           output->write("\r\n");

        std::cout << output->toString() << "\n";
    }
}
