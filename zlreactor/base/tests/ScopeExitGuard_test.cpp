#include <iostream>
#include <Windows.h>
#include <assert.h>
#include "base/ScopeExitGuard.h"
using namespace std;
using namespace zl::base;

class test
{
public:
    test() : res(0)
    {
        cout << "test(), now value is " << res << "\n";
    }
    ~test()
    {
        cout << "~test(), now value is " << res << "\n";
    }

    void reset()
    {
        res = 0;
    }

    bool process(bool b)
    {
        if(b)
        {
            res += 1;
            cout << "test::process ok\n";
        }
        else
        {
            res = -1;
            cout << "test::process fail\n";
        }
        return b;
    }

private:
    int res ;
};

void safe_delete(test * t)
{
    delete t; t = 0;
}

void deletep1(int *p)
{
    cout << "deletep1" << "\t" << *p << "\n";
    delete p;
}

int deletep2(int *p)
{
    cout << "deletep2" << "\t" << *p << "\n";
    delete p;
    return 0;
}

int deletep3(int *p, int n)
{
    cout << "deletep3" << "\t" << *p << "\n";
    assert(*p == n);
    delete p;
    return 0;
}

void test_scopeexitguard()
{
    {
        //std::function<int ()> func =std::bind(fclose, file);    // ok
        //std::function<void ()> func =std::bind(fclose, file);   // error
        //std::function<void()> func = [&] { fclose(file); };     // ok
        FILE *file = fopen("f.log", "w");
        ScopeExitGuard<> seg([&] { fclose(file); });
        fwrite("123", 1, 3, file);
    }
    {
        int *p = new int;
        ScopeExitGuard<> seg([&] { delete p; p = NULL; });
        *p = 5;
    }
    {
        int *p = new int;
        ON_SCOPE_EXIT([&] { delete p; p = NULL; });
        *p = 5;
    }
    {
        int *p1 = new int(1111);
        {
            ON_SCOPE_EXIT(std::bind(deletep1, p1));
        }
        int *p2 = new int(2222);
        {
            auto scope = makeScopeExitGuard(std::bind(deletep2, p2));
        }
        int *p3 = new int(33333);
        {
            std::function<int(int*,int)> hhhh = std::bind(deletep3, p3, *p3);
            ScopeExitGuard< std::function<int(int*,int)> > seg(std::bind(deletep3, p3, *p3));
            auto scope = makeScopeExitGuard(std::bind(deletep3, p3, *p3));
        }
    }
    {
        test *p = new test;
        //ON_SCOPE_EXIT([&] { delete p; p = NULL; });
        ScopeExitGuard<> seg(std::bind(safe_delete, p));
        p->process(true);
        throw;
    }
}

void test_rollback()
{
    cout << "===========rollback 1===========\n";
    while(1)
    {
        test t;
        ScopeExitGuard<> onFailureRollback([&] { t.reset(); });
        if(t.process(false) == false) // fail
            break;
        onFailureRollback.dismiss();
        break;
    }
    cout << "===========rollback 2===========\n";
    while(1)
    {
        test t;
        ScopeExitGuard<> onFailureRollback([&] { t.reset(); });
        if(t.process(true) == false) // fail
            break;
        onFailureRollback.dismiss();
        break;
    }
}

int main()
{
    test_scopeexitguard();

    test_rollback();

    return 0;
}