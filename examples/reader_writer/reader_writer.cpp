/*************************************************************************
	File Name   : examples/reader_writer/reader_writer.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2014年12月30日 星期二 19时43分35秒
 ************************************************************************/
#include <stdio.h>
#include <vector>
#include <map>
#include <memory>      // for share_ptr
#include "Define.h"
#include "thread/Thread.h"
#include "thread/RWMutex.h"
#include "thread/Atomic.h"
using namespace std;
using namespace zl::thread;

const static int reader_num = 5;
const static int writer_num = 1;
ZL_STATIC_ASSERT(writer_num == 1);

const static int test_max_loop = 100;

    /***
    读者-写者问题
    注意： 一般来说读者要多于写者（1个写者），如果没有额外措施保证，写者很难争取到锁
    ***/
    // TODO 需要优化，使写者有优先权得到锁
 
namespace test_RWMutex   // rwMutex
{
    class TestRWMutex 
    {
        typedef map<int, int>  Map;
        typedef Map*           MapPtr;
    public:
        TestRWMutex()
        {
            data_ = new Map;
        }
        ~TestRWMutex()
        {
            printf("TestRWMutex : readCount = %d\n", readCount_.value());
        }
        void test()
        {
            Thread  *writeThreads = new Thread(std::bind(&TestRWMutex::writer_thread, this));

            vector<Thread*> readThreads;
            for(int i= 0; i < reader_num; ++i)
            {
                Thread *trd = new Thread(std::bind(&TestRWMutex::reader_thread, this));
                readThreads.push_back(trd);
            }

            // join
            writeThreads->join();
            for(int i= 0; i < reader_num; ++i)
                readThreads[i]->join();
        }

     public:
         MapPtr getData() const
         {
             RWMutexReadLockGuard lock(rwMutex_);
             return data_;
         }
         
         void reader_thread()
         {
             while(1)
             {
                 MapPtr data = getData();
                 readCount_ ++;
                 printf("reader thread[%ld] : read one\n", this_thread::get_id().tid());
                 if(data->size() > test_max_loop)
                 {
                     break;
                 }
             }
         }

         void writer_thread()
         {
		     //pthread_setschedprio(pthread_self( ), SCHED_FIFO);  //提高写线程的优先级
             int count = 0;
             while(1)
             {
                 count ++;
                 RWMutexWriteLockGuard lock(rwMutex_);
                 (*data_)[count] = count * count;
                 printf("writer thread[%ld] : writer one\n", this_thread::get_id().tid());
                 if(count > test_max_loop)
                     break;
             }
         }
    private:
        MapPtr            data_;
        mutable RWMutex   rwMutex_;
        Atomic<int>       readCount_;
    };

    void test_rwmutex()
    {
        TestRWMutex t;
        t.test();
        printf("game over\n");
    }
}

namespace test_Mutex_COW      // mutex + copy on write
{
    /***
    chenshuo : 读写锁的开销比普通mutex要大
        借shared_ptr实现copy-on-write http://blog.csdn.net/Solstice/article/details/3351751
    ***/
    class TestMutexCow
    {
        typedef map<int, int>  Map;
        typedef shared_ptr< Map > MapPtr;

    public:
        TestMutexCow()
        {
            data_.reset(new Map);
        }
        ~TestMutexCow()
        {
            printf("TestMutexCow : readCount = %d\n", readCount_.value());
        }
        void test()
        {
            Thread  *writeThreads = new Thread(std::bind(&TestMutexCow::writer_thread, this));

            vector<Thread*> readThreads;
            for(int i= 0; i < reader_num; ++i)
            {
                Thread *trd = new Thread(std::bind(&TestMutexCow::reader_thread, this));
                readThreads.push_back(trd);
            }

            // join
            writeThreads->join();
            for(int i= 0; i < reader_num; ++i)
                readThreads[i]->join();
        }

    public:
        MapPtr getData() const
        {
            LockGuard<Mutex> lock(mutex_);
            return data_;
        }

        void reader_thread()
        {
            while(1)
            {
                // data 一旦拿到，就不再需要锁了。取数据的时候只有getData()内部有锁，多线程并发读的性能很好。
                MapPtr data = getData();
                readCount_ ++;
                printf("reader thread[%ld] : read one\n", this_thread::get_id().tid());
                if(data->size() > test_max_loop)
                {
                    break;
                }
            }
        }

        void writer_thread()
        {
            int count = 0;
            while(1)
            {
                count ++;
                LockGuard<Mutex> lock(mutex_);
                if (!data_.unique())  //说明有人在读，此时不能直接就地修改，需要创建修改的副本
                {
                    MapPtr newData(new Map(*data_));
                    data_.swap(newData);
                }
                assert(data_.unique());
                (*data_)[count] = count * count;
                printf("writer thread[%ld] : writer one\n", this_thread::get_id().tid());

                if(count > test_max_loop)
                    break;
            }
        }

    private:
        MapPtr            data_;
        mutable Mutex     mutex_;
        Atomic<int>       readCount_;
    };

    void test_mutex_cow()
    {
          TestMutexCow t;
          t.test();
          printf("game over\n");
    }
}

int main()
{
    test_RWMutex::test_rwmutex();
    printf("------------------------------\n");
    test_Mutex_COW::test_mutex_cow();
}
