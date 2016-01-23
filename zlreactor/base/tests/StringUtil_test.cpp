#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <string.h>
#include <assert.h>
#include "base/StringUtil.h"
using namespace std;
using namespace zl;
using namespace zl::base;

void trim_test(std::string src, const std::string& dest, const char* delim = " ")
{
    cout << "origin: [" << src << "], delim: [" << delim << "]\n";
    string src2(src);
    cout << "[" << trimRight(src, delim) << "]\n";
    cout << "[" << trimLeft(src, delim) << "]\n";
    cout << "[" << trim(src2, delim) << "]\n";
    assert(dest == trim(src2, delim));
}

int main()
{
    {
        cout << "trim test : \n";
        trim_test("  hello world!   ", "hello world!");
        trim_test("hello world!   ", "hello world!");
        trim_test("  hello world!", "hello world!");
        trim_test("%%%%g%hello world %!%%%%%%%", "g%hello world %!", "%");
        trim_test("@#@#@#@ hello world!@#@#@#", " hello world!", "@#");
        trim_test("u@#@#@#@ hello world!@#@#@#", "u@#@#@#@ hello world!", "@#");
        trim_test("@#@#@#@ hello world!@#@#@######@#", " hello world!", "@#");
        trim_test("@#@#@#@@ hello world!@#@#@#g#", " hello world!@#@#@#g", "@#");

        string s("@#@#@#@ hello world!@#@#@##");
        cout << s << "\t" << s.size() << "\n";
        cout << s.find("@") << "\n";
        cout << s.find_last_not_of("@#") << "\n";
        cout << s.find_first_not_of("@ #") << "\n";  // 返回的是第一不是”@ #“串中任一字符的位置

        cout << "\n\n";
    }

    string str1 = zl::base::toStr(100.123);
    //wstring wstr1 = zl::toStr<double, wchar_t>(100.123);
    cout << str1 << "\n";
    //wcout << wstr1 << "\n";

    //cout << zl::strTo<float, char>(str1) << "\n";
    cout << zl::base::strTo<int>(str1) << "\n";

    string str2 = " fg t5rt $%^   !  ";
    zl::base::trim(str2);  // "fg t5rt $%^   !"
    cout << str2 << "\n";
    str2 = " fg t5rt $%^   !  ";
    zl::base::erase(str2, ' ');  // "fgt5rt$%^!"
    cout << str2 << "\n";

    str2 = "ttdgfh t34 0-tt";
    zl::base::erase(str2, 't');   // dgfh 34 0-
    cout << str2 << "\n";

    str2 = "ttd gfht t34 0-tt";
    zl::base::replaceAll(str2, "tt", "123");   // 123dgfh t34 0-123
    cout << str2 << "\n";

    std::vector<std::string> vec;
    str2 = "ttd gfht t34 0-tt";
    zl::base::split(str2, vec, std::string("t"), true);   // **d gfh* *34 0-***
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<string>(std::cout, "*"));
    cout << "\n";

    vec.clear();
    str2 = "ttd gfht t34 0-tt";
    zl::base::split(str2, vec, std::string("t"), false); // d gfh* *34 0-*
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<string>(std::cout, "*"));
    cout << "\n";

    cout << zl::base::join(vec, "##") << "\n";

    list<string> ls={"hello", "world", "github"};
    cout << zl::base::join(ls, "-") << "\n";

    string s;
    zl::base::stringFormat(&s, "%d-%s-%f", 1234, "hello", "43.21");
    cout << s << "\n";

    s = zl::base::stringFormat("%s-%d\n", "world", 33);
    cout << s ;

    char data[20];
    ::memset(data, 0, 20);
    for(char c = 'A'; c < 'A' + 20 - 1; c++)
        data[c - 'A'] = c;
    s = zl::base::stringFormat("%s", data);
    cout << s << "\n";

    return 0;
}
