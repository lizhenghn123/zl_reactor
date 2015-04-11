#include <iostream>
#include "thread/Atomic.h"
using namespace std;


int main()
{
	{
		zl::thread::Atomic<int> ato;
		ato++;
		++ato;
		ato.fetchAndInc(10);
		ato += 100;
		ato -= 3;
		std::cout << ato.value() << "\n";

		long t = ato.decAndFetch(3);
		std::cout << ato << "\t" << t << "\n";
	}
	{
		std::cout << "========================================\n";
		//zl::thread::Atomic<int> ato;
        zl::thread::AtomicInt32 ato;

		std::cout << ato.fetchAndInc(2) << "\t" ;
		std::cout << ato.value() << "\n";

		std::cout << ato.fetchAndDec(3) << "\t" ;
		std::cout << ato.value() << "\n";
	}
	{
		std::cout << "========================================\n";
		zl::thread::Atomic<int> ato;

		std::cout << ato.incAndFetch(2) << "\t" ;
		std::cout << ato << "\n";
	}
    {
        std::cout << "========================================\n";
        zl::thread::Atomic<bool> ato;
        cout << ato << "\n" ;
        cout << ato.test_and_set() << "\n";
        cout << ato << "\n" ;

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
	return 0;
}
