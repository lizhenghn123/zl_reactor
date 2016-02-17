#include <stdio.h>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <deque>
#include <type_traits>
#include "thread/Thread.h"
#include "thread/ConcurrentQueue.h"

// 测试代码参考 https://github.com/shines77/ConcurrentTest.git
// 这里也说明了std::mutex 效率不如CRITICAL_SECTION
// 因为我测试对方用例，单进单出加锁队列操作才200W/s
// 而我实现的并发队列相应条件下能达到700W/s(同一台机器，vs2013 release编译，intel i3@3.30GHz 8GBmemory )
// 而在我虚拟机里，也能超过200W/s

#define USE_MY_THREAD

#ifdef USE_MY_THREAD
typedef zl::thread::Thread TThread;
#else
typedef std::thread		   TThread;
#endif

#if defined(NDEBUG)
#define MAX_MESSAGE_COUNT   (800 * 10000)
#else
#define MAX_MESSAGE_COUNT   (50 * 10000)
#endif

#if defined(NDEBUG)
#define MAX_ITERATIONS      (200 * 100000)
#else
#define MAX_ITERATIONS      (10 * 100000)
#endif


enum
{
    kMaxMessageCount = MAX_MESSAGE_COUNT
};

class Message {
public:
    uint64_t value;

public:
    Message() : value(0) {}
    Message(Message const & src) {
        this->value = src.value;
    }
    Message(Message && src) {
        this->value = src.value;
    }
    ~Message() {}

    Message & operator = (const Message & rhs) {
        this->value = rhs.value;
        return *this;
    }
};

template <typename QueueType, typename MessageType>
void producer_thread_proc(unsigned index, unsigned producers, QueueType * queue)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    //printf("Producer Thread: thread_idx = %d, producers = %d.\n", index, producers);

    unsigned messages = kMaxMessageCount / producers;
    for (unsigned i = 0; i < messages; ++i) {
        message_type * msg = new message_type();
        queue->push(msg);
    }
}

template <typename QueueType, typename MessageType>
void consumer_thread_proc(unsigned index, unsigned consumers, QueueType * queue)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    //printf("Consumer Thread: thread_idx = %d, consumers = %d.\n", index, consumers);

    unsigned messages = kMaxMessageCount / consumers;
    for (unsigned i = 0; i < messages; ++i) {
        message_type * msg = nullptr;
        queue->pop(msg);
    }
}

template <typename QueueType, typename MessageType>
void run_test_threads(unsigned producers, unsigned consumers, size_t initCapacity)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    queue_type queue;
    //queue.resize(initCapacity);

    TThread ** producer_threads = new TThread *[producers];
    TThread ** consumer_threads = new TThread *[consumers];

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            TThread * thread = new TThread(std::bind(producer_thread_proc<queue_type, message_type>,
                i, producers, &queue));
            producer_threads[i] = thread;
        }
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            TThread * thread = new TThread(std::bind(consumer_thread_proc<queue_type, message_type>,
                i, consumers, &queue));
            consumer_threads[i] = thread;
        }
    }

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            producer_threads[i]->join();
        }
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            consumer_threads[i]->join();
        }
    }

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            if (producer_threads[i])
                delete producer_threads[i];
        }
        delete[] producer_threads;
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            if (consumer_threads[i])
                delete consumer_threads[i];
        }
        delete[] consumer_threads;
    }
}

template <typename QueueType, typename MessageType>
void run_test(unsigned producers, unsigned consumers, size_t initCapacity)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    printf("Test for: %s\n", typeid(queue_type).name());
    printf("\n");

    using namespace std::chrono;
    time_point<high_resolution_clock> startime = high_resolution_clock::now();

    run_test_threads<queue_type, message_type>(producers, consumers, initCapacity);

    time_point<high_resolution_clock> endtime = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast< duration<double> >(endtime - startime);

    //printf("\n");
    printf("Elapsed time: %0.3f second(s)\n", elapsed_time.count());
    printf("Throughput:   %0.1f op/sec\n", (double)(kMaxMessageCount * 1) / elapsed_time.count());
    printf("\n");
}


int main()
{
    unsigned producers, consumers;
    producers = 1;
    consumers = 1;

    printf("ConcurrentQueue_bench.\n\n");
    printf("Messages  = %u\n", kMaxMessageCount);
    printf("Producers = %u\n", producers);
    printf("Consumers = %u\n", consumers);
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
 || defined(__amd64__) || defined(__x86_64__)
    printf("x86_64    = true\n");
#else
    printf("x86_64    = false\n");
#endif
    printf("\n");

#if 1
    run_test<zl::thread::FifoConcurrentQueue<Message *>, Message>(producers, consumers, 4096);
    run_test<zl::thread::FiloConcurrentQueue<Message *>, Message>(producers, consumers, 4096);
    run_test<zl::thread::PrioConcurrentQueue<Message *>, Message>(producers, consumers, 4096);
#endif

    system("pause");
}