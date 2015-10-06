#include <iostream>
#include <assert.h>
#include "base/Singleton.h"
using namespace std;
//using namespace zl;

class Manager : public zl::Singleton<Manager>
{
	DECLARE_SINGLETON_CLASS(Manager);
public:
	void start()
	{
		cout << "start\n";
	}

private:
	Manager() { cout << "Ctor\n"; } 
	~Manager() { cout << "Dtor\n"; } 
	int id;
};

int main()
{
    zl::Singleton<Manager>::getInstancePtr()->start();

    Manager *m1 = zl::Singleton<Manager>::getInstancePtr();
    Manager *m2 = zl::Singleton<Manager>::getInstancePtr();
    Manager *m3 = zl::Singleton<Manager>::getInstancePtr();

    assert(m1 == m2);
    assert(m1 == m3);

	cout << "GAME OVER\n";
}
