/*
 * TODO: remove and replace this file header comment
 * You will edit and turn in this file.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <iostream>    // for cout, endl
#include <climits>
#include "queue.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * 检测队列是否有序，这里是升序
 * 实际上不需要用
 */
bool isSorted(Queue<int> q) {
    int min = INT_MIN;
    while(!q.isEmpty()) {
        if(q.peek() < min)
            return false;
        min = q.dequeue();
    }

    return true;
}

/*
 * 非常熟悉的归并排序基本步骤，但是不能随机存取
 * 所以更像合并链表
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    /* TODO: Implement this function. */
    int min = INT_MIN;
    while(!a.isEmpty() && !b.isEmpty()) {
        if(a.peek() <= b.peek()) {
            if(a.peek() <  min)
                error("first queue is not sorted.");
            min = a.peek();
            result.enqueue(a.dequeue());
        } else {
            if(b.peek() < min)
                error("second queue is not sorted.");
            min = b.peek();
            result.enqueue(b.dequeue());
        }
    }
    while(!a.isEmpty())
        result.enqueue(a.dequeue());
    while(!b.isEmpty())
        result.enqueue(b.dequeue());

    return result;
}

/*
 * This function assumes correct functionality of the previously
 * defined binaryMerge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented –
 * it does not need to modified at all.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}


/*
 * 边界情况：分到一边为空，一边只剩一个，那么直接返回剩下的那个
 * 不需要排序
 * 注意左值引用不能绑右值
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;
    if(all.isEmpty())
        return result;
    if(all.size() == 1)
        return all[0];
    Vector<Queue<int>> left = all.subList(0,all.size() / 2);
    Vector<Queue<int>> right = all.subList(all.size() / 2,all.size() - left.size());

    result = binaryMerge(recMultiMerge(left),recMultiMerge(right));
    return result;
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}

PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}

STUDENT_TEST("test whether sorted") {
    Queue<int> q = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    Queue<int> q2 = {-5, -5, 5, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT(isSorted(q));
    EXPECT(!isSorted(q2));
}

STUDENT_TEST("test whether unsorted queue is detected") {
    Queue<int> q = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    Queue<int> q2 = {-5, -5, 5, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_ERROR(binaryMerge(q,q2));
    EXPECT_ERROR(binaryMerge(q2,q));
    EXPECT_ERROR(binaryMerge(q2,q2));
    EXPECT_NO_ERROR(binaryMerge(q,q));
}

/*
STUDENT_TEST("Time binaryMerge operation") {
    int n = 1000000;
    for(int i = 0;i < 4;++i, n *= 2) {
        Queue<int> a = createSequence(n);
        Queue<int> b = createSequence(n);
        TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
    }
}
*/

STUDENT_TEST("test naiveMerge") {
    Vector<Queue<int>> test = {
        {},
        {},
        {},
        {},
        {},
        };
    Queue<int> expect = {};
    EXPECT_EQUAL(naiveMultiMerge(test),expect);
}

/*
STUDENT_TEST("Time naiveMultiMerge operation, with fixed k and increasing n") {
    int n = 10000;
    int k = n / 100;
    for(int i = 0;i < 5;++i,n *= 2) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), naiveMultiMerge(all));
    }
}


STUDENT_TEST("Time naiveMultiMerge operation, with fixed n and increasing k") {
    int n = 10000;
    int k = n / 100;
    for(int i = 0;i < 5;++i,k *= 2) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), naiveMultiMerge(all));
    }
}
*/

STUDENT_TEST("recMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
        {1, 5, 9, 9, 12},
        {5},
        {},
        {-5, -5},
        {3402}
    };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    expected = recMultiMerge(all);
    EXPECT_EQUAL(recMultiMerge(all), expected);
}


STUDENT_TEST("recMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{},
        {},
        {},
        {},
        {},
        {}
    };
    Queue<int> expected = {};
    expected = recMultiMerge(all);
    EXPECT_EQUAL(recMultiMerge(all), expected);
}

STUDENT_TEST("Time recMultiMerge operation, with fixed k and increasing n") {
    int n = 100000;
    int k = n / 10000;
    for(int i = 0;i < 5;++i,n *= 2) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), recMultiMerge(all));
    }
}


STUDENT_TEST("Time recMultiMerge operation, with fixed n and increasing k") {
    int n = 100000;
    int k = 4;
    for(int i = 0;i < 5;++i,k *= 4) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), recMultiMerge(all));
    }
}
