#include "HeapPQueue.h"
#include <algorithm>
using namespace std;

/*
 * 构造函数
 * 将动态分配空间设为初始大小
 * 之后将指针指向动态分配的 DataPoint 数组
 */
HeapPQueue::HeapPQueue() : logicalSize(0) {
    elems = new DataPoint[INITIAL_SIZE];
    allocatedSize = INITIAL_SIZE;
}

/*
 * 析构函数
 * 将动态分配的空间释放掉
 * 将逻辑大小和分配空间都重置为 0
 * 将指针置空
 */
HeapPQueue::~HeapPQueue() {
    logicalSize = 0;
    allocatedSize = 0;
    delete [] elems;
    elems = nullptr;
}


/*
 * 返回队列中 DataPoint 数量
 */
int HeapPQueue::size() const {

    return logicalSize;
}

/*
 * 返回实际 DataPoint 数量是否为 0
 */
bool HeapPQueue::isEmpty() const {

    return logicalSize == 0;
}


/*
 * 入队函数
 * 若空间不足，则重新分配空间
 * 否则入队，然后调整堆
 */
void HeapPQueue::enqueue(const DataPoint& data) {
    if(logicalSize == allocatedSize - 1)
        resize();
        //error("Array is full now");

    elems[++logicalSize] = data;
    adjustHeapUp();
}

/*
 * 读取堆顶
 * 如果堆为空，则报错
 */
DataPoint HeapPQueue::peek() const {
    if(logicalSize == 0)
        error("The Array is empty now");
    return elems[1];
}


/*
 * 出队
 * 返回堆顶元素，若堆为空则报错
 * 弹出堆顶之后向下调整
 */
DataPoint HeapPQueue::dequeue() {
    if(logicalSize == 0)
        error("The Array is empty now");

    DataPoint re = elems[1];
    adjustHeapDown();

    return re;
}

/* This function is purely for you to use during testing. You can have it do whatever
 * you'd like, including nothing. We won't call this function during grading, so feel
 * free to fill it with whatever is most useful to you!
 *
 * If you choose not to use this function, don't delete it from this .cpp
 * file. Instead, just leave the body of the function empty.
 *
 * TODO: Delete this comment and replace it with one describing what this function
 * actually does.
 */
void HeapPQueue::printDebugInfo() {
    /* TODO: Delete this comment and (optionally) put debugging code here. */
}


/*
 * 堆上调整函数
 * 迭代地与父节点判断，若值小于父节点则交换值
 * 也可以用递归来解决。
 */
void HeapPQueue::adjustHeapUp() {
    for(int i = logicalSize;i > 1;i /= 2) {
        if(elems[i].weight < elems[i / 2].weight)
            std::swap(elems[i],elems[i / 2]);
    }
}


/*
 * 重新分配大小
 * 当入队后超出数组大小时，重新分配数组
 */
void HeapPQueue::resize() {
    allocatedSize *= 2;
    DataPoint* newElems = new DataPoint[allocatedSize]; // 重新分配大小为之前两倍

    for(int i = 1;i <= logicalSize;++i) { // 数据拷贝 注意索引
        newElems[i] = elems[i];
    }

    delete [] elems; // 释放原来的空间
    elems = newElems; // 指向新数组
    newElems = nullptr; // 指针置空，不再管理
}


/*
 * 堆下调整函数
 * 首先将堆顶元素和队尾交换
 * 之后从堆顶开始迭代与子节点比较，并和更小的子节点交换
 * 注意最后一个节点的父节点，不一定有右子
 */
void HeapPQueue::adjustHeapDown() {
    std::swap(elems[1],elems[logicalSize--]);
    for(int root = 1;root <= logicalSize / 2;) { // logicalSize / 2：最后一个节点的父节点
        int left = root * 2;
        int right = root * 2 + 1;
        int lower = (right <= logicalSize && elems[left].weight > elems[right].weight) ? right : left;
        if(elems[lower].weight > elems[root].weight)
            break; // 若某一层不需要调整，那么调整结束
        std::swap(elems[root],elems[lower]); // 否则需要调整
        root = lower; // 下一次调整的起始位置是被置换的子节点位置
    }
}


/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Add your own custom tests here! */

/*
STUDENT_TEST("initially enqueue report error when overgrow") {
    HeapPQueue pq;


    DataPoint one   = { "First",  1 };
    DataPoint two   = { "Second", 2 };
    DataPoint three = { "Third",  3 };
    DataPoint four  = { "Four",   4 };
    DataPoint five  = { "Five",   5 };
    DataPoint six   = { "Six",    6 };

    pq.enqueue(one);
    pq.enqueue(two);
    pq.enqueue(three);
    pq.enqueue(four);
    pq.enqueue(five);

    auto* oldArray = pq.elems;
    EXPECT_ERROR(pq.enqueue(six));
}
*/











/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Newly-created heap is empty.") {
    HeapPQueue pq;

    EXPECT(pq.isEmpty());
    EXPECT(pq.size() == 0);
}

PROVIDED_TEST("Newly-created heap allocates an array of the right size.") {
    HeapPQueue pq;

    EXPECT_NOT_EQUAL(pq.elems, nullptr);
    EXPECT_EQUAL(pq.logicalSize, 0);
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Can enqueue a single element.") {
    HeapPQueue pq;
    pq.enqueue({ "First", 137.0 });

    /* Check other fields for validity. */
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.logicalSize, 1);
    EXPECT(!pq.isEmpty());

    /* Slot zero in the priority queue is not used, so we won't check
     * its value here.
     *
     * Slot one in the priority queue should contain the newly-added
     * element.
     */
    EXPECT_EQUAL(pq.elems[1], { "First", 137.0 });

    /* There is still space left, so no array growth
     * should have been necessary.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues two elements in sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one = { "First",  1.6108 };
    DataPoint two = { "Second", 2.7182 };

    pq.enqueue(one);

    /* Make sure it's there. */
    EXPECT_EQUAL(pq.elems[1], one);

    pq.enqueue(two);

    /* We should have
     *
     * [ unused ] [ one ] [ two ]
     *
     * because one.weight < two.weight.
     */
    EXPECT_EQUAL(pq.size(), 2);
    EXPECT_EQUAL(pq.elems[1], one);
    EXPECT_EQUAL(pq.elems[2], two);

    /* There is still space left, so no array growth
     * should have been necessary.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues two elements in reverse sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one = { "First",  3.1415 };
    DataPoint two = { "Second", 2.7182 };

    pq.enqueue(one);

    /* Make sure it's there. */
    EXPECT_EQUAL(pq.elems[1], one);

    pq.enqueue(two);

    /* We should have
     *
     * [ unused ] [ two ] [ one ]
     *
     * because one.weight > two.weight.
     */
    EXPECT_EQUAL(pq.size(), 2);
    EXPECT_EQUAL(pq.elems[1], two);
    EXPECT_EQUAL(pq.elems[2], one);

    /* There is still space left, so no array growth
     * should have been necessary.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues three elements in sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one   = { "First",  1.6108 };
    DataPoint two   = { "Second", 2.7182 };
    DataPoint three = { "Third",  3.1415 };

    pq.enqueue(one);
    pq.enqueue(two);
    pq.enqueue(three);

    /* We should have these contents:
     *
     * [ unused ] [ one ] [ two ] [ three ]
     */
    EXPECT_EQUAL(pq.size(), 3);
    EXPECT_EQUAL(pq.elems[1], one);
    EXPECT_EQUAL(pq.elems[2], two);
    EXPECT_EQUAL(pq.elems[3], three);

    /* There is still space left, so no array growth
     * should have been necessary.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues three elements in reverse-sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one   = { "First",  161 };
    DataPoint two   = { "Second", 106 };
    DataPoint three = { "Third",  103 };

    pq.enqueue(one);
    pq.enqueue(two);   // Bubbles up
    pq.enqueue(three); // Bubbles up

    /* We should have these contents:
     *
     * [ unused ] [ three ] [ one ] [ two ]
     *
     * The reason is that inserting two should swap with
     * one, and inserting three should swap with two.
     */
    EXPECT_EQUAL(pq.size(), 3);
    EXPECT_EQUAL(pq.elems[1], three);
    EXPECT_EQUAL(pq.elems[2], one);
    EXPECT_EQUAL(pq.elems[3], two);

    /* There is still space left, so no array growth
     * should have been necessary.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues five elements in sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one   = { "First",  1 };
    DataPoint two   = { "Second", 2 };
    DataPoint three = { "Third",  3 };
    DataPoint four  = { "Four",   4 };
    DataPoint five  = { "Five",   5 };

    pq.enqueue(one);
    pq.enqueue(two);
    pq.enqueue(three);
    pq.enqueue(four);
    pq.enqueue(five);

    /* We should have these contents:
     *
     * [ unused ] [ one ] [ two ] [ three ] [ four ] [ five ]
     *
     * No insertion should trigger a bubble up.
     */
    EXPECT_EQUAL(pq.size(), 5);
    EXPECT_EQUAL(pq.elems[1], one);
    EXPECT_EQUAL(pq.elems[2], two);
    EXPECT_EQUAL(pq.elems[3], three);
    EXPECT_EQUAL(pq.elems[4], four);
    EXPECT_EQUAL(pq.elems[5], five);

    /* We are exactly at capacity. No growth should have
     * occurred here.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueues five elements in reverse-sorted order.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one   = { "First",  10 };
    DataPoint two   = { "Second", 9 };
    DataPoint three = { "Third",  8 };
    DataPoint four  = { "Four",   7 };
    DataPoint five  = { "Five",   6 };

    pq.enqueue(one);
    pq.enqueue(two);
    pq.enqueue(three);
    pq.enqueue(four);
    pq.enqueue(five);

    /* We should have these contents:
     *
     * [ unused ] [ five ] [ four ] [ two ] [ one ] [ three ]
     *
     * To see why this is, get out a pencil and paper and trace
     * through the exact series of insertions performed.
     */
    EXPECT_EQUAL(pq.size(), 5);
    EXPECT_EQUAL(pq.elems[1], five);
    EXPECT_EQUAL(pq.elems[2], four);
    EXPECT_EQUAL(pq.elems[3], two);
    EXPECT_EQUAL(pq.elems[4], one);
    EXPECT_EQUAL(pq.elems[5], three);

    /* We are exactly at capacity. No growth should have
     * occurred here.
     */
    EXPECT_EQUAL(pq.allocatedSize, HeapPQueue::INITIAL_SIZE);
}

PROVIDED_TEST("Enqueue grows the internal array.") {
    HeapPQueue pq;

    /* Add these items in this order. */
    DataPoint one   = { "First",  1 };
    DataPoint two   = { "Second", 2 };
    DataPoint three = { "Third",  3 };
    DataPoint four  = { "Four",   4 };
    DataPoint five  = { "Five",   5 };
    DataPoint six   = { "Six",    6 };

    pq.enqueue(one);
    pq.enqueue(two);
    pq.enqueue(three);
    pq.enqueue(four);
    pq.enqueue(five);

    /* This next one triggers the grow step. */
    auto* oldArray = pq.elems;
    pq.enqueue(six);

    /* The array should have been replaced with a new one. */
    EXPECT_NOT_EQUAL(pq.elems, oldArray);
    EXPECT_EQUAL(pq.allocatedSize, 2 * HeapPQueue::INITIAL_SIZE);

    /* We should have these contents:
     *
     * [ unused ] [ one ] [ two ] [ three ] [ four ] [ five ] [ six ]
     *
     * No insertion should trigger a bubble up.
     */
    EXPECT_EQUAL(pq.elems[1], one);
    EXPECT_EQUAL(pq.elems[2], two);
    EXPECT_EQUAL(pq.elems[3], three);
    EXPECT_EQUAL(pq.elems[4], four);
    EXPECT_EQUAL(pq.elems[5], five);
    EXPECT_EQUAL(pq.elems[6], six);
    EXPECT_EQUAL(pq.size(), 6);
}

PROVIDED_TEST("Enqueue repeatedly grows the internal array.") {
    HeapPQueue pq;

    /* Insert lots of values in sorted order. No bubble-up should
     * ever occur in this process, but the internal array will
     * need to grow many times.
     */
    for (int i = 1; i < 100; i++) {
        pq.enqueue({ to_string(i), double(i) });
        EXPECT_EQUAL(pq.size(), i);

        /* Confirm the array contents exactly match what they
         * should be - namely, the elements in sorted order.
         */
        for (int j = 1; j <= i; j++) {
            EXPECT_EQUAL(pq.elems[j], { to_string(j), double(j) });
        }
    }
}

PROVIDED_TEST("Enqueue can bubble up long chains.") {
    HeapPQueue pq;

    /* First, create this heap:
     *
     *                2
     *             /     \
     *          3           4
     *         /  \       /   \
     *        5    6     7     8
     *       / \  / \   / \   /
     *      9 10 11 12 13 14 15
     *
     * This can be done by inserting all these elements in order
     * into the priority queue.
     */
    for (int i = 2; i <= 15; i++) {
        pq.enqueue({ to_string(i), double(i) });
    }

    /* Confirm the queue is set up correctly. */
    for (int i = 2; i <= 15; i++) {
        EXPECT_EQUAL(pq.elems[i - 1], { to_string(i), double(i) });
    }
    EXPECT_EQUAL(pq.size(), 14);

    /* Now, enqueue 1. This will cause a chain of swaps that
     * ends with the following:
     *
     *                1
     *             /     \
     *          3           2
     *         /  \       /   \
     *        5    6     7     4
     *       / \  / \   / \   / \
     *      9 10 11 12 13 14 15  8
     */
    pq.enqueue({ "1", 1 });
    EXPECT_EQUAL(pq.size(), 15);

    EXPECT_EQUAL(pq.elems[1],   {  "1",  1 });
    EXPECT_EQUAL(pq.elems[2],   {  "3",  3 });
    EXPECT_EQUAL(pq.elems[3],   {  "2",  2 });
    EXPECT_EQUAL(pq.elems[4],   {  "5",  5 });
    EXPECT_EQUAL(pq.elems[5],   {  "6",  6 });
    EXPECT_EQUAL(pq.elems[6],   {  "7",  7 });
    EXPECT_EQUAL(pq.elems[7],   {  "4",  4 });
    EXPECT_EQUAL(pq.elems[8],   {  "9",  9 });
    EXPECT_EQUAL(pq.elems[9],   { "10", 10 });
    EXPECT_EQUAL(pq.elems[10],  { "11", 11 });
    EXPECT_EQUAL(pq.elems[11],  { "12", 12 });
    EXPECT_EQUAL(pq.elems[12],  { "13", 13 });
    EXPECT_EQUAL(pq.elems[13],  { "14", 14 });
    EXPECT_EQUAL(pq.elems[14],  { "15", 15 });
    EXPECT_EQUAL(pq.elems[15],  {  "8",  8 });
}

PROVIDED_TEST("Can enqueue and dequeue a single element.") {
    HeapPQueue pq;

    DataPoint first = { "First", 137 };
    pq.enqueue(first);

    EXPECT_EQUAL(pq.dequeue(), first);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT(pq.isEmpty());
}

PROVIDED_TEST("Reports an error when dequeuing from an empty queue.") {
    HeapPQueue pq;

    EXPECT_ERROR(pq.dequeue());
}

PROVIDED_TEST("Can bubble down to the left.") {
    HeapPQueue pq;

    /* Form this heap:
     *
     *         1
     *        / \
     *       2   3
     *      /
     *     4
     */
    DataPoint v1 = { "First",  1 };
    DataPoint v2 = { "Second", 2 };
    DataPoint v3 = { "Third",  3 };
    DataPoint v4 = { "Fourth", 4 };

    pq.enqueue(v1);
    pq.enqueue(v2);
    pq.enqueue(v3);
    pq.enqueue(v4);

    EXPECT_EQUAL(pq.size(), 4);
    EXPECT_EQUAL(pq.elems[1], v1);
    EXPECT_EQUAL(pq.elems[2], v2);
    EXPECT_EQUAL(pq.elems[3], v3);
    EXPECT_EQUAL(pq.elems[4], v4);

    /* Do a dequeue, which leaves this as the first step:
     *
     *         4
     *        / \
     *       2   3
     *
     * This should then swap the 4 down to the left to give
     *
     *         2
     *        / \
     *       4   3
     */
     EXPECT_EQUAL(pq.dequeue(), v1);
     EXPECT_EQUAL(pq.size(), 3);
     EXPECT_EQUAL(pq.elems[1], v2);
     EXPECT_EQUAL(pq.elems[2], v4);
     EXPECT_EQUAL(pq.elems[3], v3);
}

PROVIDED_TEST("Can bubble down to the right.") {
    HeapPQueue pq;

    /* Form this heap:
     *
     *         1
     *        / \
     *       3   2
     *      /
     *     4
     */
    DataPoint v1 = { "First",  1 };
    DataPoint v2 = { "Second", 2 };
    DataPoint v3 = { "Third",  3 };
    DataPoint v4 = { "Fourth", 4 };

    pq.enqueue(v1);
    pq.enqueue(v3);
    pq.enqueue(v2);
    pq.enqueue(v4);

    EXPECT_EQUAL(pq.size(), 4);
    EXPECT_EQUAL(pq.elems[1], v1);
    EXPECT_EQUAL(pq.elems[2], v3);
    EXPECT_EQUAL(pq.elems[3], v2);
    EXPECT_EQUAL(pq.elems[4], v4);

    /* Do a dequeue, which leaves this as the first step:
     *
     *         4
     *        / \
     *       3   2
     *
     * This should then swap the 4 down to the right to give
     *
     *         2
     *        / \
     *       3   4
     */
     EXPECT_EQUAL(pq.dequeue(), v1);
     EXPECT_EQUAL(pq.size(), 3);
     EXPECT_EQUAL(pq.elems[1], v2);
     EXPECT_EQUAL(pq.elems[2], v3);
     EXPECT_EQUAL(pq.elems[3], v4);
}

PROVIDED_TEST("Can bubble down two layers.") {
    HeapPQueue pq;

    /* Form this heap:
     *
     *           1
     *        /     \
     *       2       3
     *      / \     /
     *     4   5   6
     */
    DataPoint v1 = { "First",  1 };
    DataPoint v2 = { "Second", 2 };
    DataPoint v3 = { "Third",  3 };
    DataPoint v4 = { "Fourth", 4 };
    DataPoint v5 = { "Fifth",  5 };
    DataPoint v6 = { "Sixth",  6 };

    pq.enqueue(v1);
    pq.enqueue(v2);
    pq.enqueue(v3);
    pq.enqueue(v4);
    pq.enqueue(v5);
    pq.enqueue(v6);

    EXPECT_EQUAL(pq.size(), 6);
    EXPECT_EQUAL(pq.elems[1], v1);
    EXPECT_EQUAL(pq.elems[2], v2);
    EXPECT_EQUAL(pq.elems[3], v3);
    EXPECT_EQUAL(pq.elems[4], v4);
    EXPECT_EQUAL(pq.elems[5], v5);
    EXPECT_EQUAL(pq.elems[6], v6);

    /* Do a dequeue, which leaves this as the first step:
     *
     *           6
     *        /     \
     *       2       3
     *      / \
     *     4   5
     *
     * This should then swap the 6 down to the left to give
     *
     *           2
     *        /     \
     *       6       3
     *      / \
     *     4   5
     *
     * This should swap the 6 down to the left again to give
     *
     *           2
     *        /     \
     *       4       3
     *      / \
     *     6   5
     */
     EXPECT_EQUAL(pq.dequeue(), v1);
     EXPECT_EQUAL(pq.size(), 5);
     EXPECT_EQUAL(pq.elems[1], v2);
     EXPECT_EQUAL(pq.elems[2], v4);
     EXPECT_EQUAL(pq.elems[3], v3);
     EXPECT_EQUAL(pq.elems[4], v6);
     EXPECT_EQUAL(pq.elems[5], v5);
}

PROVIDED_TEST("Can bubble down when only one child exists.") {
    HeapPQueue pq;

    /* Form this heap:
     *
     *         1
     *        / \
     *       2   3
     *      / \
     *     4   5
     */
    DataPoint v1 = { "First",  1 };
    DataPoint v2 = { "Second", 2 };
    DataPoint v3 = { "Third",  3 };
    DataPoint v4 = { "Fourth", 4 };
    DataPoint v5 = { "Fifth",  5 };

    pq.enqueue(v1);
    pq.enqueue(v2);
    pq.enqueue(v3);
    pq.enqueue(v4);
    pq.enqueue(v5);

    EXPECT_EQUAL(pq.size(), 5);
    EXPECT_EQUAL(pq.elems[1], v1);
    EXPECT_EQUAL(pq.elems[2], v2);
    EXPECT_EQUAL(pq.elems[3], v3);
    EXPECT_EQUAL(pq.elems[4], v4);
    EXPECT_EQUAL(pq.elems[5], v5);

    /* Do a dequeue, which leaves this as the first step:
     *
     *         5
     *        / \
     *       2   3
     *      /
     *     4
     *
     * This should then swap the 5 down to the left to give
     *
     *         2
     *        / \
     *       5   3
     *      /
     *     4
     *
     * This should swap the 5 down to the left again to give
     *
     *         2
     *        / \
     *       4   3
     *      /
     *     5
     */
     EXPECT_EQUAL(pq.dequeue(), v1);
     EXPECT_EQUAL(pq.size(), 4);
     EXPECT_EQUAL(pq.elems[1], v2);
     EXPECT_EQUAL(pq.elems[2], v4);
     EXPECT_EQUAL(pq.elems[3], v3);
     EXPECT_EQUAL(pq.elems[4], v5);
}

PROVIDED_TEST("Enqueue / dequeue single element") {
    HeapPQueue pq;
    DataPoint point = { "enqueue me!", 4 };
    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);

    pq.enqueue(point);
    EXPECT_EQUAL(pq.size(), 1);
    EXPECT_EQUAL(pq.isEmpty(), false);

    EXPECT_EQUAL(pq.dequeue(), point);
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Dequeue / peek on empty heap throws error") {
    HeapPQueue pq;

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

PROVIDED_TEST("Enqueue elements in sorted order.") {
    HeapPQueue pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "elem" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), double(i) });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), double(i) });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue many elements in sorted order.") {
    HeapPQueue pq;
    for (int i = 0; i < 10000; i++) {
        pq.enqueue({ "elem" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 10000);
    for (int i = 0; i < 10000; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), double(i) });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), double(i) });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue elements in reverse-sorted order.") {
    HeapPQueue pq;
    for (int i = 10; i >= 0; i--) {
        pq.enqueue({ "elem" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 11);
    for (int i = 0; i <= 10; i++) {
        EXPECT_EQUAL(pq.peek(), { "elem" + to_string(i), double(i) });
        EXPECT_EQUAL(pq.dequeue(), { "elem" + to_string(i), double(i) });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Enqueue many elements in reverse-sorted order.") {
    HeapPQueue pq;
    for (int i = 10000; i >= 0; i--) {
        pq.enqueue({ "elem" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 10001);
    for (int i = 0; i <= 10000; i++) {
        auto removed = pq.dequeue();
        DataPoint expected = {
            "elem" + to_string(i), double(i)
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert ascending and descending sequences.") {
    HeapPQueue pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + to_string(i), 2 * double(i) });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), 2 * double(i) + 1 });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 40; i++) {
        auto removed = pq.dequeue();
        EXPECT_EQUAL(removed.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert large ascending and descending sequences.") {
    HeapPQueue pq;
    for (int i = 0; i < 20000; i++) {
        pq.enqueue({ "a" + to_string(i), 2 * double(i) });
    }
    for (int i = 19999; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), 2 * double(i) + 1 });
    }

    EXPECT_EQUAL(pq.size(), 40000);
    for (int i = 0; i < 40000; i++) {
        auto removed = pq.dequeue();
        EXPECT_EQUAL(removed.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert random permutation.") {
    Vector<DataPoint> sequence = {
        { "A", 0 },
        { "D", 3 },
        { "F", 5 },
        { "G", 6 },
        { "C", 2 },
        { "H", 7 },
        { "I", 8 },
        { "B", 1 },
        { "E", 4 },
        { "J", 9 },
    };

    HeapPQueue pq;
    for (DataPoint elem: sequence) {
        pq.enqueue(elem);
    }

    EXPECT_EQUAL(pq.size(), sequence.size());

    for (int i = 0; i < 10; i++) {
        auto removed = pq.dequeue();
        DataPoint expected = {
            string(1, 'A' + i), double(i)
        };
        EXPECT_EQUAL(removed, expected);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert duplicate elements.") {
    HeapPQueue pq;
    for (int i = 0; i < 20; i++) {
        pq.enqueue({ "a" + to_string(i), double(i) });
    }
    for (int i = 19; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 40);
    for (int i = 0; i < 20; i++) {
        auto one = pq.dequeue();
        auto two = pq.dequeue();

        EXPECT_EQUAL(one.weight, i);
        EXPECT_EQUAL(two.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Insert many duplicate elements.") {
    HeapPQueue pq;
    for (int i = 0; i < 20000; i++) {
        pq.enqueue({ "a" + to_string(i), double(i) });
    }
    for (int i = 19999; i >= 0; i--) {
        pq.enqueue({ "b" + to_string(i), double(i) });
    }

    EXPECT_EQUAL(pq.size(), 40000);
    for (int i = 0; i < 20000; i++) {
        auto one = pq.dequeue();
        auto two = pq.dequeue();

        EXPECT_EQUAL(one.weight, i);
        EXPECT_EQUAL(two.weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);
}

PROVIDED_TEST("Handles data points with empty string name.") {
    HeapPQueue pq;
    for (int i = 0; i < 10; i++) {
        pq.enqueue({ "" , double(i) });
    }
    EXPECT_EQUAL(pq.size(), 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(pq.dequeue(), { "", double(i) });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT(pq.isEmpty());
}

PROVIDED_TEST("Handles many data points with empty string name.") {
    HeapPQueue pq;
    for (int i = 0; i < 10000; i++) {
        pq.enqueue({ "" , double(i) });
    }
    EXPECT_EQUAL(pq.size(), 10000);
    for (int i = 0; i < 10000; i++) {
        EXPECT_EQUAL(pq.dequeue(), { "", double(i) });
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT(pq.isEmpty());
}

PROVIDED_TEST("Handles data points with negative weights.") {
    HeapPQueue pq;
    for (int i = -10; i < 10; i++) {
        pq.enqueue({ "" , double(i) });
    }
    EXPECT_EQUAL(pq.size(), 20);
    for (int i = -10; i < 10; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
}

PROVIDED_TEST("Handles many data points with negative weights.") {
    HeapPQueue pq;
    for (int i = -10000; i < 10000; i++) {
        pq.enqueue({ "" , double(i) });
    }
    EXPECT_EQUAL(pq.size(), 20000);
    for (int i = -10000; i < 10000; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
}

PROVIDED_TEST("Interleave enqueues and dequeues.") {
    HeapPQueue pq;
    int n = 100;
    for (int i = n / 2; i < n; i++) {
        pq.enqueue({"", double(i)});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = n / 2; i < n; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);

    for (int i = 0; i < n / 2; i++) {
        pq.enqueue({"", double(i)});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = 0; i < n / 2; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("Interleave many enqueues and dequeues.") {
    HeapPQueue pq;
    int n = 10000;
    for (int i = n / 2; i < n; i++) {
        pq.enqueue({"", double(i)});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = n / 2; i < n; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);

    for (int i = 0; i < n / 2; i++) {
        pq.enqueue({"", double(i)});
    }
    EXPECT_EQUAL(pq.size(), n / 2);
    for (int i = 0; i < n / 2; i++) {
        EXPECT_EQUAL(pq.dequeue().weight, i);
    }
    EXPECT_EQUAL(pq.size(), 0);
}

PROVIDED_TEST("Stress test: cycle 250,000 elems (should take at most a few seconds)") {
    HeapPQueue pq;
    int n = 250000;
    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomReal(0, 100000) });
    }
    EXPECT_EQUAL(pq.size(), n);

    for (int i = 0; i < n; i++) {
        pq.dequeue();
    }
    EXPECT_EQUAL(pq.size(), 0);
    EXPECT_EQUAL(pq.isEmpty(), true);

    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomReal(0, 100000) });
    }
    EXPECT_EQUAL(pq.size(), n);
}
