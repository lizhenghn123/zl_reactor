/*************************************************************************
	File Name   : BlockingQueueWorker_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年06月24日 星期二 20时12分34秒
 ************************************************************************/
#include <iostream>
#include "thread/Thread.h"
#include "thread/Mutex.h"
#include "thread/Condition.h"
#include "thread/ThreadPool.h"
#include "thread/BlockingQueue.h"
#include "thread/BoundedBlockingQueue.h"
#include "thread/BlockingQueueWorker.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

template < typename Queue >
class Test_BlockingQueueWorker
{
public:
    Test_BlockingQueueWorker(Queue& queue, int threadsNum)
        : queue_(queue),
          worker_(queue_, threadsNum)
    {
        
    }
    ~Test_BlockingQueueWorker()
    {
        
    }

    template < typename Func>
    void start(Func f)
    {
        worker_.start(f);
        printf("Test_BlockingQueueWorker starting......\n");
    }
private:
    Queue&                      queue_;
    BlockingQueueWorker<Queue>  worker_;
};

bool g_testBlockingQueue = false;

BlockingQueue<int> g_blockQueue;
BoundedBlockingQueue<int> g_boundBlockQueue(100);

void processer(int i)
{
    printf("thread consumer[%ld] gett item[%d]\n", this_thread::get_id().tid(), i);
}
void producer(int i)
{
    int loop = 20;
    while (loop -- > 0)
    {
        printf("thread producer[%ld] push item[%d]\n", this_thread::get_id().tid(), loop * i);
        if (g_testBlockingQueue)
            g_blockQueue.push(loop * i);
        else
            g_boundBlockQueue.push(loop * i);
        this_thread::sleep(500); // sleep 500ms
    }
    printf("thread producer[%ld] exit\n", this_thread::get_id().tid());
}

void test_BlockingQueueWorker()
{
    const int procuder_thread = 10;
    const int comsumer_thread = 3;
    if (g_testBlockingQueue)
    {
        Test_BlockingQueueWorker< BlockingQueue<int> > tbw(g_blockQueue, comsumer_thread);
        tbw.start(std::bind(processer, std::placeholders::_1));

        ThreadGroup tg;
        for (int i = 1; i <= procuder_thread; i++)
        {
            tg.add_thread(new Thread(std::bind(producer, i)));
        }
        tg.join_all();

        g_blockQueue.stop();

        printf("Test_BlockingQueueWorker< BlockingQueue<int> > over\n");
    }
    else
    {
        Test_BlockingQueueWorker< BoundedBlockingQueue<int> > tbw(g_boundBlockQueue, comsumer_thread);
        tbw.start(std::bind(processer, std::placeholders::_1));

        ThreadGroup tg;
        for (int i = 1; i <= procuder_thread; i++)
        {
            tg.add_thread(new Thread(std::bind(producer, i)));
        }
        tg.join_all();

        g_boundBlockQueue.stop();

        printf("Test_BlockingQueueWorker< BoundedBlockingQueue<int> > over\n");
    }
}

int main()
{
    cout << "###### test BlockingQueueWorker ######\n";
    test_BlockingQueueWorker();
    
    cout << "###### GAME OVER ######\n";
    return 0;
}
