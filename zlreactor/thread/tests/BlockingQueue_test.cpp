#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <iterator>
#include "thread/Thread.h"
#include "thread/BlockingQueue.h"
#include "thread/BoundedBlockingQueue.h"
#include "thread/ThreadPool.h"
#include "thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::thread;

class TestBlockingQueue
{
public:
    TestBlockingQueue(int thread_num)
        : latch_(thread_num)
    {
        for(int i=0; i<thread_num; ++i)
        {
            char name[32];
            ZL_SNPRINTF(name, sizeof(name), "thread_%d", i);
            threads_.push_back(new Thread(std::bind(&TestBlockingQueue::threadFunction, this), name));
            //cout << "create thread : " << name << "\n";
        }
    }
    void run(int loop)
    {
        for(int i = 0; i<loop; ++i)
        {
            queue_.push(i+1);
        }
    }
    void joinAll()
    {
        queue_.stop();
        latch_.wait();
    }
private:
    void threadFunction()
    {
        int one;
        while(queue_.pop(one))
        {
            char result[256];
            ZL_SNPRINTF(result, sizeof(result), "thread_%d_get_%d\n", this_thread::get_id().tid(), one);
            std::cout << result;
            this_thread::sleep_for(chrono::milliseconds(200));
        }
        latch_.countDown();
    }
private:
    zl::thread::BlockingQueue<int> queue_;
    std::vector<zl::thread::Thread*> threads_;
    zl::thread::CountDownLatch latch_;
};

class TestBoundedBlockingQueue
{
public:
    TestBoundedBlockingQueue(int queue_size, int thread_num)
        : queue_(queue_size), latch_(thread_num)
    {
        for(int i=0; i<thread_num; ++i)
        {
            char name[32];
            ZL_SNPRINTF(name, sizeof(name), "thread_%d", i);
            threads_.push_back(new Thread(std::bind(&TestBoundedBlockingQueue::threadFunction, this), name));
        }
    }
    void run(int loop)
    {
        for(int i = 0; i<loop; ++i)
        {
            queue_.push(i+1);
        }
    }
    void joinAll()
    {
        for (size_t i = 0; i < threads_.size(); ++i)
        {
            queue_.push(-1);
        }
        latch_.wait();
    }
private:
    void threadFunction()
    {
        bool running = true;
        while(running)
        {
            int one = queue_.pop();
            char result[256];
            ZL_SNPRINTF(result, sizeof(result), "thread_%d_get_%d\n", this_thread::get_id().tid(), one);
            std::cout << result;
            this_thread::sleep_for(chrono::milliseconds(200));
            running = (one != -1);
        }
        latch_.countDown();
    }
private:
    zl::thread::BoundedBlockingQueue<int> queue_;
    std::vector<zl::thread::Thread*> threads_;
    zl::thread::CountDownLatch latch_;
};

void test_blockingqueue()
{
    cout << "test_blockingqueue 1......\n";
    {
        TestBlockingQueue tbq(3);
        tbq.run(13);
        this_thread::sleep_for(chrono::milliseconds(1000));
        tbq.joinAll();
    }
    cout << "test_blockingqueue 2......\n";
    {
        zl::thread::BlockingQueue<int, std::queue<int>, zl::thread::tagFIFO> queue;
        queue.push(1);
        queue.push(3);
        queue.push(2);
        queue.push(0);

        int p = -1;
        while (true)
        {
            if (queue.try_pop(p))
                std::cout << p << "\n";
            else
                break;
        }
        std::cout << "================\n";
    }
    cout << "test_blockingqueue 3......\n";
    {
        zl::thread::BlockingQueue<int, std::stack<int>, zl::thread::tagFILO> queue;
        queue.push(1);
        queue.push(3);
        queue.push(2);
        queue.push(0);

        int p = -1;
        while (true)
        {
            if (queue.try_pop(p))
                std::cout << p << "\n";
            else
                break;
        }
        std::cout << "================\n";
    }
    cout << "test_blockingqueue 4......\n";
    {
        zl::thread::BlockingQueue<int, std::priority_queue<int>, zl::thread::tagPRIO> queue;
        //zl::BlockingQueue<int> queue;
        queue.push(1);
        queue.push(3);
        queue.push(2);
        queue.push(0);

        int p = -1;
        while (true)
        {
            if (queue.try_pop(p))
                std::cout << p << "\n";
            else
                break;
        }
        std::cout << "================\n";
    }
    cout << "test_blockingqueue 5......\n";
    {
        zl::thread::BlockingQueue<int> queue;

        for (int i = 0; i<100; ++i)
             queue.push(i);
        std::cout << queue.pop() << "\n";

        std::vector<int> vec;
        if(queue.pop(vec, 10))
             std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
        cout << "\n";
        std::cout << queue.pop() << "\n";
        std::cout << "current size " << queue.size() << "\n";
        std::cout << "================\n";
    }
}

void test_boundleblockingqueue()
{
    cout << "test_boundleblockingqueue 1......\n";
    {
        TestBoundedBlockingQueue tbq(5, 3);
        tbq.run(13);
        this_thread::sleep_for(chrono::milliseconds(1000));
        tbq.joinAll();
    }
    cout << "test_boundleblockingqueue 2......\n";
    {
        zl::thread::BoundedBlockingQueue<int, std::priority_queue<int>, zl::thread::tagPRIO> queue(3);
        queue.push(1);
        queue.push(3);
        queue.push(2);
        queue.push(0); // block, queue.Size() > 3;
    }
}

int main()
{
    test_blockingqueue();

    test_boundleblockingqueue();

    return 0;
}
