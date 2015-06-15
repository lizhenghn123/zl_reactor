#include <iostream>
#include <list>
#include <map>
#include "net/TcpServer.h"
#include "net/TcpConnection.h"
#include "net/EventLoop.h"
#include "base/Logger.h"
#include "base/Timestamp.h"
#include "thread/Atomic.h"
#include "thread/Mutex.h"
using namespace std;
using namespace zl;
using namespace zl::net;
using namespace zl::thread;

// 定时删除空闲连接
// 采用LRU的思路，如果某个连接收到了数据，就将该连接插入到链表末尾；
// 定时从链表头部检查每个连接的空闲时间是否超过预定的最大空闲时间.
class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr);

    void start()
    {
        server_.setMultiReactorThreads(0);   // 不使用multi reactor
        server_.start();
    }

    void setMaxIdleSecond(double idlesecond) { maxIdleMillSeconds_ = idlesecond * 1000; }
    
private:
    void onTimer();
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time);
    void dump();

private:
	typedef std::weak_ptr<TcpConnection>    WeakTcpConnectionPtr;
	typedef std::list<WeakTcpConnectionPtr> WeakTcpConnectionList;
	struct Node
	{
		Timestamp lastReceiveTime_;
		WeakTcpConnectionList::iterator position_;
	};
	typedef std::map<TcpConnection*, Node>  IteratorMap;

    TcpServer server_;	
	TimerId timeId_;
	int maxIdleMillSeconds_;
	WeakTcpConnectionList connectionList_;
	IteratorMap iterMap_;
    Mutex       mutex_;
};

EchoServer::EchoServer(EventLoop* loop, const InetAddress& listenAddr)
    : server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
					
	timeId_ = loop->addTimer(std::bind(&EchoServer::onTimer, this), 1, true); 
}


void EchoServer::onConnection(const TcpConnectionPtr& conn)
{
    cout  << "EchoServer::onConnection " << conn->peerAddress().ipPort() << " is "
        << (conn->connected() ? "UP" : "DOWN") << "\n"; 
    if (conn->connected()) // connecting
    {
		connectionList_.push_back(conn);
        Node node;
		node.lastReceiveTime_ = Timestamp::now();
		node.position_ = --connectionList_.end();
		iterMap_[conn.get()] = node;
        ZL_ASSERT(iterMap_.size() == connectionList_.size())(iterMap_.size())(connectionList_.size());
    }
    else    // closeing
    {
        IteratorMap::iterator iter = iterMap_.find(conn.get());
		assert(iter != iterMap_.end());
        connectionList_.erase(iter->second.position_);
        iterMap_.erase(iter);
        ZL_ASSERT(iterMap_.size() == connectionList_.size())(iterMap_.size())(connectionList_.size());
    }
}

void EchoServer::onMessage(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time)
{
    //LOG_NOTICE("socket [%d] thread[%ld]", conn->fd(), this_thread::get_id().tid());

    string msg(buf->retrieveAllAsString());
    //cout << "EchoServer::onMessage, fd [" << conn->fd() << "], "<< msg.size() << " bytes, ["
    //    << msg.data() << "], received at " << time.toString() << "\n";
    LOG_NOTICE("socket [%d] get message[%s]", conn->fd(), time.toString().c_str());
    conn->send(msg.data(), msg.size());

    //LockGuard<Mutex> lg(mutex_);  
    IteratorMap::iterator iter = iterMap_.find(conn.get());
	assert(iter != iterMap_.end());
	iter->second.lastReceiveTime_ = time;
	connectionList_.splice(connectionList_.end(), connectionList_, iter->second.position_);
//    dump();
    if(iterMap_.size() != connectionList_.size())
    {
        cout << iterMap_.size() << connectionList_.size() << "\n";
    }
    assert(iterMap_.size() == connectionList_.size());
    ZL_ASSERT(iterMap_.size() == connectionList_.size())(iterMap_.size())(connectionList_.size());
	assert(iter->second.position_ == --connectionList_.end());
	ZL_ASSERT(iter->second.position_ == --connectionList_.end())(iterMap_.size())((connectionList_.size()));
}

void EchoServer::onTimer()
{
	Timestamp now = Timestamp::now();
    for (WeakTcpConnectionList::iterator it = connectionList_.begin(); it != connectionList_.end(); )
	{
		TcpConnectionPtr conn = it->lock();
		if (conn)
		{
			Node node = iterMap_[conn.get()];
            int64_t timeMs = Timestamp::timeDiffMs(now, node.lastReceiveTime_);  // mill seconds
			if (timeMs > maxIdleMillSeconds_)
			{
				if (conn->connected())
				{
					conn->shutdown();
					LOG_NOTICE("One tcp connection expired[%s][%s][%ld], and close it[%d].", now.toString().c_str(), node.lastReceiveTime_.toString().c_str(),timeMs, conn->fd());
				}
			}
			else
			{
                if(timeMs == 0) // 精度不够的原因，很可能时间差极小，此处计算就等于0了
                {
                }
				ZL_ASSERT(timeMs >= 0 && "cannot be this")(timeMs)(now)(node.lastReceiveTime_);
				break;
			}
			
			++it;
		}
		else    // 说明此时该连接已经关闭了
		{
			LOG_NOTICE("One Tcp connection already closed[%d].", conn->fd());
			it = connectionList_.erase(it);
			iterMap_.erase(conn.get());
		}
	}
}
void EchoServer::dump() 
{
    cout << "--------" << iterMap_.size() << "\t" << connectionList_.size() << "-------\n";
    for(WeakTcpConnectionList::const_iterator it = connectionList_.begin(); it != connectionList_.end(); ++it)
    {
        TcpConnectionPtr tcp = it->lock();
		Node node = iterMap_[tcp.get()];
        cout << tcp.get() << "\t" << node.lastReceiveTime_<<"\n";
        //cout << tcp.get() << "\t" << iterMap_[tcp.get()].lastReceiveTime_<<"\n";
    }
}
int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("----------- test idle connection -----------\n");
        printf("usage : %s <max_idle_sconed>\n", argv[0]);
        return 0;
    }

    zl::base::Logger::setLogPriority(zl::base::ZL_LOG_PRIO_NOTICE);

    EventLoop loop;
    InetAddress listenAddr("127.0.0.1", 8888);
    EchoServer server(&loop, listenAddr);
    server.setMaxIdleSecond(atoi(argv[1]));

    server.start();
    loop.loop();
}
