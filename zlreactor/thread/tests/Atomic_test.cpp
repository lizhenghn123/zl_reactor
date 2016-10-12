#include <iostream>
#include <assert.h>
#include "zlreactor/thread/Atomic.h"
using namespace std;


void test_usage()
{
    {
        zl::thread::Atomic<int> ato;
        ato++;
        ++ato;
        ato.getAndAdd(10);
        ato += 100;
        ato -= 3;
        std::cout << ato.value() << "\n";

        long t = ato.subAndGet(3);
        std::cout << ato << "\t" << t << "\n";
    }
    {
        std::cout << "========================================\n";
        //zl::thread::Atomic<int> ato;
        zl::thread::AtomicInt32 ato;

        std::cout << ato.getAndAdd(2) << "\t";
        std::cout << ato.value() << "\n";

        std::cout << ato.addAndGet(3) << "\t";
        std::cout << ato.value() << "\n";
    }
    {
        std::cout << "========================================\n";
        zl::thread::Atomic<bool> ato;
        cout << ato << "\n";
        cout << ato.test_and_set() << "\n";
        cout << ato << "\n";

        ato = true;
        cout << ato << "\n";
        ato = false;
        cout << ato << "\n";

        ato = false;
        ato = false;
        cout << ato << "\n";

        ato = true;
        ato.clear();
        cout << ato << "\n";
    }
}

template < typename T>
void test_atomic()
{
    zl::thread::Atomic<T> ato;

    ato = (T)0;
    ato++;
    assert(ato.value() == 1);

    ato += 3;
    assert(ato.value() == 4);

    ato--;
    --ato;
    assert(ato.value() == 2);
}

template <>
void test_atomic<bool>()
{
    zl::thread::AtomicBool ato;
    ato = true;
    assert(ato.value() == true);
    ato.clear();
    assert(ato.value() == false);
    assert(ato.test_and_set() == false);
    assert(ato.value() == true);
}

int main()
{
    test_usage();

    test_atomic<int>();
    test_atomic<unsigned int>();
    test_atomic<long int>();
    test_atomic<char>();
    test_atomic<long>();
    test_atomic<uint64_t>();
    test_atomic<long long>();
    test_atomic<bool>();

    return 0;
}
