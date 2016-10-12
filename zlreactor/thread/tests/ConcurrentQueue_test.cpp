#include <iostream>
#include <vector>
#include <assert.h>
#include "zlreactor/thread/ConcurrentQueue.h"
using namespace std;
using namespace zl::thread;

// 模仿java.util.concurrent库并发容器及Mircosoft的并发容器而写的一个简单并发队列
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/LinkedBlockingQueue.html
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/ConcurrentLinkedQueue.html
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/PriorityBlockingQueue.html
int main()
{
    {
        FifoConcurrentQueue<int> cq;
        cq.push(3);
        cq.push(1);
        cq.push(2);

        int i;
        cq.pop(i);
        assert(i == 3);
        cq.pop(i);
        assert(i == 1);
        cq.pop(i);
        assert(i == 2);
    }

    {
        FiloConcurrentQueue<int> cq;
        cq.push(3);
        cq.push(1);
        cq.push(2);

        int i;
        cq.pop(i);
        assert(i == 2);
        cq.pop(i);
        assert(i == 1);
        cq.pop(i);
        assert(i == 3);
    }

    {
        PrioConcurrentQueue<int> cq;
        cq.push(3);
        cq.push(1);
        cq.push(2);

        int i;
        cq.pop(i);
        assert(i == 3);
        cq.pop(i);
        assert(i == 2);
        cq.pop(i);
        assert(i == 1);
    }
    {
        ConcurrentQueue<int, std::priority_queue<int, std::vector<int>, std::greater<int> >, ConcurrentQueueTraits::tagPRIO> cq;
        cq.push(3);
        cq.push(1);
        cq.push(2);

        int i;
        cq.pop(i);
        assert(i == 1);
        cq.pop(i);
        assert(i == 2);
        cq.pop(i);
        assert(i == 3);
    }

}