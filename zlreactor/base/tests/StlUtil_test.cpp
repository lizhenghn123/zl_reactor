/*************************************************************************
	File Name   : StlUtil_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月30 星期五 23时44分57秒
 ************************************************************************/
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <map>
#include <string>
#include "zlreactor/base/StlUtil.h"
#include "zlreactor/base/StlUtil.h"
using namespace std;
using namespace zl;
using namespace zl::base;

#define EXPECT_EQ(e1, e2)   assert(e1 == e2)
#define EXPECT_TRUE(e)      assert(e)
#define EXPECT_FALSE(e)     assert(!e)

class Int
{
public:
    Int(int i) : num(i)
    {
        cout << "construct " << this << "\t" << num << "\n";
    }
    ~Int()
    {
        cout << "destruct  " << this << "\t" << num << "\n";
    }

    friend bool operator<(Int lhs, Int rhs)
    {
        return lhs.num < rhs.num;
    }
private:
    int num;
};

// see https://github.com/chen3feng/toft/blob/master/base/stl_util_test.cpp
void test_MapUtil()
{
    {
        std::map<std::string, int> c;
        c["hello"] = 1;

        EXPECT_EQ(NULL, findPtrOrNull(c, "he"));
        EXPECT_TRUE(findPtrOrNull(c, "hello") != NULL);
        EXPECT_EQ(1, *findPtrOrNull(c, "hello"));

        EXPECT_FALSE(insertIfNotExist(&c, "hello", 2));
        EXPECT_FALSE(hasKey(c, "world"));
        EXPECT_TRUE(insertIfNotExist(&c, "world", 2));
        EXPECT_TRUE(hasKey(c, "world"));
        EXPECT_TRUE(findPtrOrNull(c, "world") != NULL);
        EXPECT_EQ(2, *findPtrOrNull(c, "world"));

        EXPECT_FALSE(hasKey(c, "china"));
        EXPECT_TRUE(insertOrReplace(&c, "china", 3));
        EXPECT_TRUE(findPtrOrNull(c, "china") != NULL);
        EXPECT_EQ(3, *findPtrOrNull(c, "china"));
        EXPECT_FALSE(insertOrReplace(&c, "china", 4));
        EXPECT_TRUE(findPtrOrNull(c, "china") != NULL);
        EXPECT_EQ(4, *findPtrOrNull(c, "china"));
    }
    {
        std::map<std::string, int*> ptrc;
        int val = 10;
        EXPECT_TRUE(insertIfNotExist(&ptrc, "a",  &val));
        EXPECT_TRUE(hasKey(ptrc, "a"));
        EXPECT_EQ(NULL, findOrNull(ptrc, "b"));
        EXPECT_TRUE(findOrNull(ptrc, "a") != NULL);
        EXPECT_EQ(val, *findOrNull(ptrc, "a"));
    }
    //TEST(MapUtilTest, findOrDefault)
    {
        std::map<std::string, int> c;
        c["hello"] = 1;
        EXPECT_EQ(1, findOrDefault(c, "hello", 0));
        EXPECT_EQ(-1, findOrDefault(c, "world", -1));
    }

    //TEST(MapUtilTest, hasKey) 
    {
        std::map<std::string, int> c;
        c["hello"] = 1;
        EXPECT_TRUE(hasKey(c, "hello"));
        EXPECT_FALSE(hasKey(c, "world"));
    }
    //TEST(MapUtilTest, hasKeyValuePair)
    {
        std::map<std::string, int> c;
        c["hello"] = 1;
        EXPECT_TRUE(hasKeyValuePair(c, "hello", 1));
        EXPECT_FALSE(hasKeyValuePair(c, "hello", 0));
        EXPECT_FALSE(hasKeyValuePair(c, "world", 1));
    }

    //TEST(MapUtilTest, insertOrReplace)
    {
        std::map<std::string, int> c;
        EXPECT_TRUE(insertOrReplace(&c, "a",  1));
        EXPECT_EQ(1, c["a"]);
        EXPECT_FALSE(insertOrReplace(&c, "a",  2));
        EXPECT_EQ(2, c["a"]);
    }  
    //TEST(MapUtilTest, InsertOrReplaceWithOldValue)
    {
        std::map<std::string, int> c;
        EXPECT_TRUE(insertOrReplace(&c, "a",  1));
        int old = 0;
        EXPECT_FALSE(insertOrReplace(&c, "a",  2, &old));
        EXPECT_EQ(2, c["a"]);
        EXPECT_EQ(1, old);
    }
}

int main()
{
    {    // test vectorAsArray
        cout << "##################### 1 ####################\n";
        vector<int> v1 = { 1, 2, 3 };
        int *p1 = vectorAsArray(&v1);
        assert(p1 != NULL);
        p1++;
        *p1 = 9;

        vector<Int> v2;
        Int *p2 = vectorAsArray(&v2);
        assert(p2 == NULL);
    }
    {   // test clearMemory,
        cout << "##################### 2 ####################\n";
        vector<Int*> v1 = { new Int(1), new Int(2), new Int(3), new Int(4) };
        //deleteContainerPoInter(v1.begin(), v1.end()); // or deleteElems(&v1)
        clearMemory(&v1);   // 仅仅清理内存，但并不释放v1中的指针，因此必须在调用前先delete指针
        cout << "---------------\n";

        vector<Int*> v2 = { new Int(1), new Int(2), new Int(3), new Int(4) };
        deleteElements(&v2);
        clearMemory(&v1); //clearIfBig(&v2);
        cout << "---------------\n";

        vector<Int*> v3 = { new Int(1), new Int(2), new Int(3), new Int(4) };
        ElementDeleter< vector<Int*> > ed(&v3);   // RAII 
    }

    {
        cout << "##################### 3 ####################\n";
        {
            map<Int, Int*> m1 = { { 1, new Int(2) }, { 2, new Int(3) }, { 3, new Int(03) } };
            deleteContainerPairSecondPointer(m1.begin(), m1.end());
            cout << "---------------\n";
        }
        {
            map<int, Int*> m2 = { { 1, new Int(2) }, { 2, new Int(3) }, { 3, new Int(03) } };
            deleteValues(&m2);
            cout << "---------------\n";
        }
        {
            map<int, Int*> m3 = { { 1, new Int(2) }, { 2, new Int(3) }, { 3, new Int(03) } };
            ValueDeleter< map<int, Int*> > vd(&m3);
            cout << "---------------\n";
        }
        {
            map<Int*, int> m4 = { { new Int(2), 1 }, { new Int(3), 5 }, { new Int(4), 8 } };
            deleteContainerPairFirstPointer(m4.begin(), m4.end());
            cout << "---------------\n";
        }
        {
            map<Int*, int> m5 = { { new Int(2), 1 }, { new Int(3), 5 }, { new Int(4), 8 } };
            deleteKeys(&m5);
            cout << "---------------\n";
        }
        {
            map<Int*, Int> m6 = { { new Int(2), 1 }, { new Int(3), 5 }, { new Int(4), 8 } };
            //KeyDeleter< map<Int*, Int> > kd(&m6);
            KeyDeleter< decltype(m6) > kd1(&m6);
        }
    }
    {
        cout << "##################### 4 ####################\n";
        string s("hello world");
        char *p = const_cast<char*>(s.data());
        p++;
        *p = '$';
        cout << s << "\n";

        p = stringAsArray(&s);
        p++;
        p++;
        *p = '%';
        cout << s << "\n";
    }
    {
        cout << "##################### test_MapUtil ####################\n";
        test_MapUtil();
    }
    printf("###### GAME OVER ######");
}
