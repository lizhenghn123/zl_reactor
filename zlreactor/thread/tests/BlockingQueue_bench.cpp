#include <stdio.h>
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <iterator>
#include <assert.h>
#include "zlreactor/base/Logger.h"
#include "zlreactor/base/LogFile.h"
#include "zlreactor/base/Timestamp.h"
#include "zlreactor/thread/Thread.h"
#include "zlreactor/thread/Mutex.h"
#include "zlreactor/thread/BlockingQueue.h"
#include "zlreactor/thread/ThreadGroup.h"
#include "zlreactor/thread/CountDownLatch.h"
using namespace std;
using namespace zl;
using namespace zl::base;
using namespace zl::thread;


class BlockingQueueBench
{
public:
    BlockingQueueBench(int numThreads)
        : comsumThreads_(numThreads)
        , latch_(numThreads)
    {
        assert(numThreads > 0 && numThreads < 10000);
        for (int i = 0; i < numThreads; ++i)
        {
            char id[32] = { 0 };

            trdGroup_.create_thread(std::bind(&BlockingQueueBench::consumer, this));
        }
    }

    void run(int jobs)
    {
        latch_.wait();      // �ȵ������̶߳��ѿ�ʼִ���̺߳���

        assert(jobs >= 0);
        jobs_ = jobs;
        prducer(jobs);     // ����jobs������
    }

    void joinAll()
    {
        for (size_t i = 0; i < comsumThreads_; ++i)
        {
            queue_.push(Timestamp::invalid());
        }
        trdGroup_.join_all();
    }

    void dump()
    {
        printf("###################################################\n");
        LOG_INFO("consume threads : %ld,  produce %d jobs\n", comsumThreads_, jobs_);
        size_t total = 0;
        for (auto it = delays_.begin(); it != delays_.end(); ++it)
        {
            //if (it->first < 200 || it->first > 10000)
            LOG_INFO("delay = %lu, count = %d", it->first, it->second);
            total += it->second;
        }
        printf("### %ld ###\n", total);
        printf("###################################################\n");
    }
private:
    void prducer(int jobs, bool useSleep = false)
    {
        for (int i = 0; i < jobs; ++i)
        {
            Timestamp now(Timestamp::now());
            queue_.push(now);
            if (useSleep)   // �Ƿ���Ҫ����������Ϣһ��
                this_thread::sleep_for(chrono::microseconds(1000));
        }
    }

    void consumer()
    {
        latch_.countDown();
        printf("consumer thread[%d] started\n", this_thread::tid());

        std::map<int64_t, int> delays;
        bool running = true;
        while (running)
        {
            Timestamp t(queue_.pop());
            if (t.valid())
            {
                int64_t us = Timestamp::now() - t;  // ����΢��
                ++delays[us];
            }
            else
            {
                running = false;
            }
        }

        LockGuard<Mutex> lock(mutex_);
        printf("consumer thread[%d] stopped\n", this_thread::tid());
        for (auto it = delays.begin(); it != delays.end(); ++it)
        {
            printf("tid = %d, delay = %lu, count = %d\n", this_thread::tid(), it->first, it->second);
            delays_[it->first] += it->second;
        }
    }

private:
    int                      jobs_;
    int                      comsumThreads_;
    BlockingQueue<Timestamp> queue_;
    CountDownLatch           latch_;
    ThreadGroup              trdGroup_;
    Mutex                    mutex_;
    std::map<int64_t, int>   delays_;   // key : microsecond, value : times
};


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("usage : %s <numThreads> <numJobs>\n", argv[0]);
        exit(0);
    }

    int threads = atoi(argv[1]);
    int jobs = atoi(argv[2]);

    LogFile lf;
    BlockingQueueBench bqb(threads);

    bqb.run(jobs);
    bqb.joinAll();
    bqb.dump();

    return 0;
}
