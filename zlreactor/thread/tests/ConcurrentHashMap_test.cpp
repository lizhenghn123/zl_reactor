#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "thread/ConcurrentHashMap.h"
using namespace std;
using namespace zl::thread;
// 模仿java.util.concurrent库并发hashmap及Mircosoft的concurrent_unordered_map而写的一个简单并发hashmap（分段锁）
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/ConcurrentMap.html
// http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/ConcurrentHashMap.html

class Tester
{
public:
    Tester(int i = 0) : num_(i)
    {}

    void print()
    {
        cout << this << "\t" << num_ << "\n";
    }

    int num_;
};

struct TesterComparor : public std::binary_function<Tester*, Tester*, bool>
{
    bool operator()(const Tester* lhs, const Tester* rhs) const
    {
        return true;
    }
};

struct StringNoCaseComparor : public std::binary_function<string, string, bool>
{
    bool operator()(const string& lhs, const string& rhs) const
    {
        string l(lhs), r(rhs);
        transform(lhs.begin(), lhs.end(), l.begin(), ::toupper);
        transform(rhs.begin(), rhs.end(), r.begin(), ::toupper);
        return l == r;
    }
};

struct StringNoCaseHash
{
    size_t operator()(const string& key) const
    {
        string s(key);
        transform(s.begin(), s.end(), s.begin(), ::toupper);
        return std::hash<std::string>()(s);
    }
};

int main()
{
    {
        ConcurrentHashMap<string, int> chm;
        chm.put("s1", 1);
        chm.put("s1", 2);
        chm.putIfAbsent("s1", 3);
        assert(chm.get("s1") == 2);

        chm.putIfAbsent("S1", 4);
        assert(chm.get("S1") == 4);
        assert(chm.get("s1") == 2);
    }
    {
        unordered_map<string, int, std::hash<std::string>, StringNoCaseComparor> um;
        um["s1"] = 1;
        um["S1"] = 2;
        //assert(um["s1"] == um["S1"]);         // VS2013 is OK, GCC 4.8.2 is ERROR!
        cout << um["s1"] << "\t" << um["S1"] << "\n";

        std::hash<std::string> hs;
        cout << hs("s1") << "\n";
        cout << hs("S1") << "\n";
        ConcurrentHashMap<string, int, StringNoCaseHash, StringNoCaseComparor> chm;
        chm.put("s1", 1);
        chm.put("s1", 2);
        chm.putIfAbsent("s1", 3);
        assert(chm.get("s1") == 2);

        chm.putIfAbsent("S1", 4);
        cout << chm.get("S1") << "\n";
        cout << chm.get("s1") << "\n";
        chm.put("S1", 222);
        cout << chm.get("S1") << "\n";
        cout << chm.get("s1") << "\n";
        assert(chm.get("S1") == 222);
        assert(chm.get("s1") == 222);
    }
    {
        ConcurrentHashMap<Tester*, int> chm;
        chm.put(new Tester(123), 124);
        chm.put(new Tester(2345), 2346);
        chm.put(new Tester(11), 12);
    }
    {
        ConcurrentHashMap<int, string> chm;
        chm.put(1, "hello");
        cout << chm.get(1) << "\n";
        chm.put(1, "111111");
        cout << chm.get(1) << "\n";
        chm.remove(1);
        cout << chm.get(1) << "\n";
        chm.putIfAbsent(1, "222222");
        cout << chm.get(1) << "\n";
        chm.putIfAbsent(1, "333333333");
        cout << chm.get(1) << "\n";
    }

}