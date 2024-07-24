/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <cctype>
#include <fstream>
#include <string>
#include <set>
#include <numeric>
#include <algorithm>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h"
#include "soundex.h"
#include "map.h"
#include "set.h"

using namespace std;

/* 这个函数用来将原始输入中的所有非字母字符去除。
 */
string lettersOnly(string s) {
    string result{};
    for (int i = 0; i < s.length(); ++i) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

/* 这个函数将字符串内所有字母全部转为大写，
 * 最后发现根本不需要自己写。
 *
*/
void toUpperCase(std::string& s) {
    for(auto &ch : s) {
        if(std::islower(ch)) {
            ch -= 32;
        }
    }
    std::cout << s << '\n';
}

/*
 * 本函数作为实际的映射执行函数，返回后字符串全部都是数字。
 */
void mapping(std::string& s) {
    static Map<char,int> encode = {
        {'A',0},
        {'B',1},
        {'C',2},
        {'D',3},
        {'E',0},
        {'F',1},
        {'G',2},
        {'H',0},
        {'I',0},
        {'J',2},
        {'K',2},
        {'L',4},
        {'M',5},
        {'N',5},
        {'O',0},
        {'P',1},
        {'Q',2},
        {'R',6},
        {'S',2},
        {'T',3},
        {'U',0},
        {'V',1},
        {'W',0},
        {'X',2},
        {'Y',0},
        {'Z',2},
        };
    for(int i = 0;i < s.size();++i) {
        s[i] = encode.get(s[i]) + '0';
    }
}

/*
 * 本函数用来对连续的数字做去重，然后将首个数字替换为大写字母。
 */
void coalesceAndReplace(std::string& s,char initial) {
    std::string re{};
    re.push_back(*s.begin());
    for(int i = 0;i < s.length();++i) {
        if(s[i] != s[i + 1] && s[i + 1] != '\0')
            re.push_back(s[i + 1]);
    }
    *re.begin() = initial;
    s = re;
}

/*
 * 本函数用来去除 s 中所有的 0，
 * 并且通过填充或者截断的方式将长度固定到 4 字符。
 */
void delZeroAndStapleLength(std::string& s) {
    std::string re;
    std::copy_if(s.begin(),s.end(),std::back_inserter(re),[](char x) {
        return x != '0';
    });

    if(re.length() > 4) { // 截断
        re.resize(4);
        s = re;
        return ;
    }
    if(re.length() < 4) { // 填充
        int padding = 4 - re.length();
        std::string temp(padding,'0');
        re.append(temp);
        s = re;
        return ;
    }
    s = re;
}


/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
string soundex(string s) {

    s = lettersOnly(s); // 去掉所有非字母字符
    if(s.empty()) // 若结果为空串，那么直接返回空串，不影响后续进行。
        return "";

    toUpperCaseInPlace(s);
    char initial = s[0]; // 保存大写首字母

    mapping(s); // 映射替换
    coalesceAndReplace(s,initial); // 去重，替换首字母

    delZeroAndStapleLength(s);
    return s;
}

/*
 * 本函数负责在数据库中遍历姓氏并计算编码，查询是否匹配。
 */
void inquiry(const Vector<string>& database,string dst) {
    Vector<string> re{};
    for(const auto& name:database) {
        auto encoded = soundex(name);
        if(encoded == dst)
            re.add(name);
    }
    if(re.isEmpty()) {
        std::cout << "No such surname recorded in database. Try again?\n";
    } else {
        re.sort();
        std::cout << "Matches from database: " << re << '\n';
    }
}


/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        allNames = readLines(in);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl;

    // The names read from file are now stored in Vector allNames
    while(1) {
        std::string input{};
        input = getLine("Enter a surname (RETURN to quit): ");
        if(input.empty())
            break;

        std::string encoded = soundex(input);
        if(!encoded.empty()) {
            std::cout << "Soundex code is " << encoded << '\n';
            inquiry(allNames,encoded);
        } else {
            std::cout << "Invalid Surname. Please input again.\n";
        }

        std::cout << std::endl;
    }
}


/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}

PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}

STUDENT_TEST("Some edge cases of lettersOnly:") {
    //EXPECT_EQUAL(lettersOnly("73189"),"");
    //EXPECT_EQUAL(lettersOnly(""),"");
    //EXPECT_EQUAL(toUpperCase("suIren"),"SUIREN");
    //EXPECT_EQUAL(soundex("Suiren"),"200605");
    //EXPECT_EQUAL(soundex("Suiren"),"S0605");
    //EXPECT_EQUAL(soundex("Suiren"),"S650");
}
