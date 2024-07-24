// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include <numeric>
#include "voting.h"
#include "testing/SimpleTest.h"
using namespace std;


/*
 * 求所有集团的关键票数
 */
Vector<int> getCriticalVotes(const Vector<int>& blocks, int index, int border, Vector<bool>& status, int total) {
    if(index == blocks.size()) { // 到达终点
        Vector<int> res(index, 0);
        if(total >= border) { // 总分过半，拿到优势
            int n = blocks.size();
            for(int i = 0;i < n;++i) {
                if(status[i] && (total < border + blocks[i])) // 考虑进来之后，去掉会导致不过半
                    res[i] += 1; // 就算关键票
            }
        }
        return res;
    }

    Vector<int> exclude = getCriticalVotes(blocks, index + 1, border, status, total); // 不包含 index，进入下一轮递归，得到结果
    status[index] = true; // 将 index 集团考虑进来
    Vector<int> include = getCriticalVotes(blocks, index + 1, border, status, total + blocks[index]); // 包含 index，进入下一轮递归
    status[index] = false; // 回溯，仍然不考虑
    for(int i = 0;i < blocks.size();++i)
        exclude[i] += include[i]; // 考虑 + 不考虑求和
    return exclude;
}


/*
 * 将关键票数变为指数
 */
void convertIndex(Vector<int>& votes) {
    int sum = std::reduce(votes.begin(),votes.end());
    for(int i = 0;i < votes.size();++i) {
        votes[i] = (votes[i] * 100) / sum;
    }
}

/*
 * 计算所有票数的一半，取上界
 * 只要累计达到这个值就必然获胜，后续不需要再加集团
 */
Vector<int> computePowerIndexes(Vector<int>& blocks)
{
    Vector<int> result;
    int sum = std::reduce(blocks.begin(),blocks.end()) / 2 + 1; // 达到一半（取上界）

    Vector<bool> countered(blocks.size(),false);
    result = getCriticalVotes(blocks, 0, sum, countered, 0);

    convertIndex(result);

    return result;
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test power index, blocks 50-49-1") {
    Vector<int> blocks = {50, 49, 1};
    Vector<int> expected = {60, 20, 20};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks Hempshead 1-1-3-7-9-9") {
    Vector<int> blocks = {1, 1, 3, 7, 9, 9};
    Vector<int> expected = {0, 0, 0, 33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-NY 55-38-39") {
    Vector<int> blocks = {55, 38, 29};
    Vector<int> expected = {33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-GA 55-38-16") {
    Vector<int> blocks = {55, 38, 16};
    Vector<int> expected = {100, 0, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Time power index operation") {
    Vector<int> blocks;
    for (int i = 0; i < 15; i++) {
        blocks.add(randomInteger(1, 10));
    }
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}


