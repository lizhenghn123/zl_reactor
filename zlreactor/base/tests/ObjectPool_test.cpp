#include <iostream>
#include <string>
#include "base/ObjectPool.h"
#include "base/Singleton.h"
#include "thread/Mutex.h"
#include "thread/FastMutex.h"
using namespace std;

class TO
{
    char c;
    int i;
    float t;
    double d;
    string s;
};

class TO1
{
    char c;
    int i;
    float t;
    double d;
    string s;
    TO *to;
};

template <typename OP>
void dump(const OP& pool)
{
    cout << "------ dump objectpool ------\n";
    cout << "max alloc : " << pool.max_alloc() << "\n";
    cout << "total alloc : " << pool.total() << "\n";
    cout << "avail : " << pool.avail() << "\n";
    //cout << "------ --------------- ------\n";
}

void test_maxalloc(int preAllocNum, int maxAllocNum)
{
    cout << "test_maxalloc: " << preAllocNum << "  " << maxAllocNum << "= = = = = = = = = \n";

    zl::base::ObjectPool<TO1> pool(preAllocNum, maxAllocNum);
    dump(pool);

    TO1* p = NULL;
    for(int i = 0; i < maxAllocNum; ++i)
    {
        p = pool.alloc();
        assert(p);
        dump(pool);
    }

    TO1* p1 = pool.alloc();
    assert(!p1);
    dump(pool);

    pool.free(p);
    TO1* p2 = pool.alloc();
    assert(p2);
    dump(pool);
}

template <typename T, typename LockType>
void test_mutexlock()
{
    cout << "test_mutexlock: " << "= = = = = = = = = \n";
    zl::base::ObjectPool<T, LockType> pool;
    T* t1 = pool.alloc();
    pool.free(t1);
    cout << t1 << "\n";
    T* t2 = pool.alloc();
    cout << t2 << "\n";
    T* t3 = pool.alloc();
    cout << t2 << "\t" << t3 << "\t" << t3 - t2 << "\t" << sizeof(T) << "\n";
    cout << "---------------\n";
}
int main()
{
    test_maxalloc(10, 2);
    test_maxalloc(1, 3);
    test_maxalloc(0, 4);

    test_mutexlock<TO, zl::thread::RecursiveMutex>();
    test_mutexlock<TO1, zl::thread::NullMutex>();
    test_mutexlock<TO1, zl::thread::FastMutex>();

}