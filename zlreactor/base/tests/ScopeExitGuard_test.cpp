#include <iostream>
#include <assert.h>
#include <vector>
#include <exception>
#include "zlreactor/base/ScopeExitGuard.h"
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
        ScopeExitGuard seg([&] { fclose(file); });
        fwrite("123", 1, 3, file);
    }
    {
        int *p = new int;
        ScopeExitGuard seg([&] { delete p; p = NULL; });
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
            auto scope = makeScopeExitGuard(std::bind(deletep3, p3, *p3));
        }
    }
    {
        try
        {
            test *p = new test;
            //ON_SCOPE_EXIT([&] { delete p; p = NULL; });
            ScopeExitGuard seg(std::bind(safe_delete, p));
            p->process(true);
            throw std::exception();
        }
        catch(...)
        {
            cout << "get exception, now p must be deconstruct!!!\n";
        }
    }
}

void test_differentWaysToBind()
{
    cout << "##############  test_differentWaysToBind  ##############\n";
    vector<int> v;
    void (vector<int>::*push_back)(int const&) = &vector<int>::push_back;

    v.push_back(1);
    {
        // binding to member function.
        ScopeExitGuard g = makeScopeExitGuard(std::bind(&vector<int>::pop_back, &v));
    }
    assert(0 == v.size());

    {
        // bind member function with args. v is passed-by-value!
        ScopeExitGuard g = makeScopeExitGuard(std::bind(push_back, v, 2));
    }
    assert(0 == v.size()); // push_back happened on a copy of v... fail!

    // pass in an argument by pointer so to avoid copy.
    {
        ScopeExitGuard g = makeScopeExitGuard(std::bind(push_back, &v, 4));
    }
    assert(1 == v.size());

    {
        // pass in an argument by reference so to avoid copy.
        ScopeExitGuard g = makeScopeExitGuard(std::bind(push_back, std::ref(v), 4));
    }
    assert(2 == v.size());

    // lambda with a reference to v
    {
        ScopeExitGuard g = makeScopeExitGuard([&] { v.push_back(5); });
    }
    assert(3 == v.size());

    // lambda with a copy of v
    {
        ScopeExitGuard g = makeScopeExitGuard([v]() mutable { v.push_back(6); });
    }
    assert(3 == v.size());
}

namespace test_rb
{
    bool init1(test *t) { return true; }
    bool init2(test *t) { return true; }
    bool init3(test *t) { return true; }
    bool init4(test *t) { return true; }

    test* getOneofTestInstance()
    {
        test *t = new test;
        ScopeExitGuard onSuccessRollback([&] { delete t; t = NULL; });

        if(!init1(t))     // init1 failure
            return NULL;  // 直接return，t 会自动析构
        if(!init2(t))     // init2 failure
            return NULL;
        if(!init3(t) || !init4(t))  // init3 or init4 failure
            return NULL;

        // now t must be available
        onSuccessRollback.dismiss(); // dismiss， t不会析构，直接返回
        return t;
    }
    void test_rollback()
    {
        cout << "===========rollback 1===========\n";
        while(1)
        {
            test t;
            ScopeExitGuard onFailureRollback([&] { t.reset(); });
            if(t.process(false) == false) // fail
                break;
            onFailureRollback.dismiss();
            break;
        }
        cout << "===========rollback 2===========\n";
        while(1)
        {
            test t;
            ScopeExitGuard onFailureRollback([&] { t.reset(); });
            if(t.process(true) == false) // fail
                break;
            onFailureRollback.dismiss();
            break;
        }

        cout << "===========rollback 3===========\n";
        test *t = getOneofTestInstance();
        ScopeExitGuard autodeleter([&] { delete t; t = NULL; });
        // do something for t
    }
}
namespace test_rb2
{
    void testUndoAction(bool failure)
    {
        vector<int64_t> v;
        {
            v.push_back(1);

            // The guard is triggered to undo the insertion unless dismiss() is called.
            ScopeExitGuard guard = makeScopeExitGuard([&] { v.pop_back(); });

            // Do some action; Use the failure argument to pretend
            // if it failed or succeeded.

            // if there was no failure, dismiss the undo guard action.
            if (!failure)
            {
                guard.dismiss();
            }
        } // all stack allocated in the mini-scope will be destroyed here.

        if (failure)
        {
            assert(0 == v.size()); // the action failed => undo insertion
        }
        else
        {
            assert(1 == v.size()); // the action succeeded => keep insertion
        }
    }

    void test_rollback()
    {
        testUndoAction(false);
        testUndoAction(true);
    }
}

int main()
{
    test_scopeexitguard();

    test_differentWaysToBind();

    test_rb::test_rollback();

    test_rb2::test_rollback();

    return 0;
}
