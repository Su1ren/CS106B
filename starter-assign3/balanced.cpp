/*
 * TODO: remove and replace this file header comment
 * You will edit and turn in this file.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "set.h"
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

/*
 * 本函数判断是否为括号
 */
bool isBracket(char ch) {
    //static const std::set<char>brackets = {'(', ')', '[', ']', '{', '}'};
    static const Set<char>brackets = {'(', ')', '[', ']', '{', '}'};
    return brackets.contains(ch);
}

/*
 * 递归调用，每次判断首字符是否为括号
 * 是则接上结果，之后进入下一轮递归
 * 否则将下一轮结果返回
 */
string operatorsFrom(string str) {
    /* TODO: Implement this function. */
    if(str.empty())
        return "";
    else if(isBracket(str[0]))
        return str[0] + operatorsFrom(str.substr(1,str.length() - 1));
    else
        return operatorsFrom(str.substr(1,str.length() - 1));
}


/*
 * 字符串匹配，迭代查询
 * 只不过这个实现方法并不算高效
 */
bool operatorsAreMatched(string ops) {
    /* TODO: Implement this function. */
    if(ops.empty())
        return true;
    else if(ops.find("()") != std::string::npos) {
        auto idx = ops.find("()");
        return operatorsAreMatched(ops.substr(0,idx) + ops.substr(idx + 2,ops.length() - idx - 2));
    } else if(ops.find("[]") != std::string::npos) {
        auto idx = ops.find("[]");
        return operatorsAreMatched(ops.substr(0,idx) + ops.substr(idx + 2,ops.length() - idx - 2));
    } else if(ops.find("{}") != std::string::npos) {
        auto idx = ops.find("{}");
        return operatorsAreMatched(ops.substr(0,idx) + ops.substr(idx + 2,ops.length() - idx - 2));
    } else {
        return false;
    }
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

STUDENT_TEST("brackets extract tests") {
    EXPECT_EQUAL(operatorsFrom("int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }"),"(){([])(())}");
    EXPECT_EQUAL(operatorsFrom("( ( [ a ] )"),"(([])");
    EXPECT_EQUAL(operatorsFrom("3 ) ("),")(");
    EXPECT_EQUAL(operatorsFrom("{ ( x } y )"),"{(})");
}

STUDENT_TEST("operator balance test") {
    std::string temp = operatorsFrom("operatorsAreMatched(ops.substr(0,idx) + ops.substr(idx + 2,ops.length() - idx - 2));");
    EXPECT(operatorsAreMatched(temp));
}
