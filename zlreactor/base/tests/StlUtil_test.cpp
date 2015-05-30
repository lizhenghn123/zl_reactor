/*************************************************************************
	File Name   : StlUtil_test.cpp
	Author      : LIZHENG
	Mail        : lizhenghn@gmail.com
	Created Time: 2015年05月30 星期五 23时44分57秒
 ************************************************************************/
#include <iostream>
#include <stdio.h>
#include "base/StlUtil.h"
using namespace std;
using namespace zl;

inline char* stringAsArray(std::string* str)
{
    // DO NOT USE const_cast<char*>(str->data())!
    return str->empty() ? NULL : &*str->begin();
}

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

int main()
{
    {	 // test vectorAsArray,	
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

    printf("###### GAME OVER ######");
}
