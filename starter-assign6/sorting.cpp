#include "listnode.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/*
 * 链表分割
 * 遍历将原始链表拆分为三条
 * 我们将原表以传入 re，第一个节点值就是主元
 */
void partition(ListNode* list, ListNode*& lessFront, ListNode*& equalFront, ListNode*& greaterFront) {
    if(list == nullptr)
        return ;

    int pivot = list->data;
    ListNode* p = list->next; // 快指针

    equalFront = list;
    ListNode* equal = equalFront;
    equal->next = nullptr;

    ListNode* less = lessFront;
    ListNode* greater = greaterFront;

    while(p) {
        if(p->data < pivot) {
            //q->next = p->next;
            if(lessFront == nullptr) {
                lessFront = p;
                less = lessFront;
            }
            else {
                less->next = p;
                less = p;
                //less->next = nullptr;
            }
        } else if(p->data > pivot) {
            //q->next = p->next; // 摘掉
            if(greaterFront == nullptr) {
                greaterFront = p;
                greater = greaterFront;
            }
            else {
                greater->next = p;
                greater = p;
                //greater->next = nullptr;
            }
        } else {
            equal->next = p;
            equal = p;
            //equal->next = nullptr;
        }

        p = p->next; // 更新快指针
    }
    if(less)
        less->next = nullptr;
    if(greater)
        greater->next = nullptr;
    if(equal)
        equal->next = nullptr;

}


/*
 * 合并两个链表
 * 返回结果的头结点
 */
ListNode* mergeLinklist(ListNode* lhs, ListNode* rhs) {
    if(lhs == nullptr)
        return rhs;

    ListNode* tie = lhs;
    while(tie->next) {
        tie = tie->next;
    }
    tie->next = rhs;
    rhs = nullptr;

    return lhs;
}


/*
 * 链表合并
 * 将多个链表和合并到同一个链表中返回
 * 返回节点为最终链表的第一个节点
 */
ListNode* concatenate(ListNode* left, ListNode* pivot, ListNode* right) {
    ListNode* temp = mergeLinklist(pivot,right);
    ListNode* re =  mergeLinklist(left,temp);

    pivot = nullptr;
    right = nullptr;
    return re;
}


/*
 * 链表快排
 * 包含两部分分割与合并
 * 递归调用自身来排序
 */
void quickSort(ListNode*& front) {
    if(front && front->next) {
        ListNode* pivot = nullptr;
        ListNode* left = nullptr;
        ListNode* right = nullptr;
        partition(front,left,pivot,right);
        quickSort(left);
//        quickSort(pivot);
        quickSort(right);
        front = concatenate(left,pivot,right);
    }
}


/* * * * * * Test Code Below This Point * * * * * */

/* TODO: Write your own student tests below to test your sorting helper
 * functions and overall sorting algorithm correctness. We have provided
 * the skeleton of some tests below to help you get started with this
 * process.
 */

/*
 * We have provided this utility function as a convenience. This
 * function traverses the linked list represented by the provided
 * front pointer and prints out the data stored in the list along
 * the way in a convenient format. You may find it to be helpful to
 * use this function as a debugging aid.
 */
void printList(ListNode* front) {
   cout << "{";
   for (ListNode *cur = front; cur != nullptr; cur = cur->next) {
       cout << cur->data;
       if (cur->next != nullptr){
           cout << ", ";
       }
   }
   cout << "}" << endl;
}

/*
 * This utility function deallocates the memory for all the nodes in a
 * given linked list. It can be used to recycle the memory allocated
 * during a test case and avoid memory leaks.
 */
void deallocateList(ListNode* front) {
    while(front) {
        ListNode* s = front->next;
        delete front;
        front = s;
    }
}

/*
 * This utility function is given a vector of values and constructs
 * an equivalent linked list containing the same values in the same order.
 * It can be used to construct a linked list to use as an input to
 * a function being tested.
 */
ListNode* createList(Vector<int> values){
    ListNode* front = nullptr;
    ListNode* rear = front;
    for(int i = 0;i < values.size();++i) {
        ListNode* s = new ListNode;
        s->data = values[i];
        s->next = nullptr;
        if(front == nullptr) {
            front = s;
            rear = front;
        } else {
            rear->next = s;
            rear = s;
        }
    }
    return front;
}


/*
 * 链表长度
 * 返回链表长度
 */
int getLen(ListNode* front) {
    int len = 0;
    while(front) {
        front = front->next;
        ++len;
    }
    return len;
}


/*
 * This utility function compares a linked list and a vector for
 * equivalence and returns true if both contain the same values in the
 * same order. It can be used to confirm a linked list's contents match
 * the expected.
 */
bool areEquivalent(ListNode* front, Vector<int> v) {
    if(getLen(front) != v.size())
        return false;

    for(int i = 0;i < v.size();++i) {
        if(v[i] != front->data)
            return false;
        front = front->next;
    }

    return true;
}


void modify(ListNode* front) {
    if(front)
        front->data = 0;
}

/*
STUDENT_TEST("Test createList") {
    Vector<int> test = {5, 6, 7, 8, 9};
    ListNode* list = createList(test);
    printList(list);
    if(areEquivalent(list,test))
        std::cout << "The list is equivalent to vector\n";
    modify(list);
    printList(list);
    deallocateList(list);
}
*/

STUDENT_TEST("Test linklist partition") {
    ListNode* list = createList({5,3,9,7,2,8,1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}


STUDENT_TEST("Test empty linklist partition") {
    ListNode* list = createList({});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp),getLen(list));

    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test single element list") {
    ListNode* list = createList({1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);

    EXPECT_EQUAL(getLen(temp),getLen(list));

    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test reverse sorted element list") {
    ListNode* list = createList({6, 5, 4, 3, 2, 1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp), 6);

    deallocateList(temp);
    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test some dupplicate elements list") {
    ListNode* list = createList({1,1,2,6,6,5,9,7,3});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp), 9);

    deallocateList(temp);

    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test all duplicate elements list") {
    ListNode* list = createList({1,1,1,1,1,1,1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp), 7);

    deallocateList(temp);
    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test positive and negative element list") {
    ListNode* list = createList({1, -2, 3, -4, 5, -6});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp), 6);
    deallocateList(temp);

    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test long list") {
    Vector<int> test;
    for(int i = 0;i < 100;++i) {
        test.add(randomInteger(-100,100));
    }
    ListNode* list = createList(test);
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    EXPECT_EQUAL(getLen(temp), 100);

    deallocateList(temp);

    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}

STUDENT_TEST("Test concatenate") {
    Vector<int> test;
    for(int i = 0;i < 100;++i) {
        test.add(randomInteger(-100,100));
    }
    ListNode* list = createList(test);
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    printList(list);
    ListNode* pivot = nullptr;
    partition(list,left,pivot,right);

    left = concatenate(left,list,right);
    printList(left);
    EXPECT_EQUAL(getLen(left), 100);

    deallocateList(left);
}


STUDENT_TEST("Whole test"){
    /* Initialize vector to sequence of values */
    Vector<int> test;
    for(int i = 0;i < 100;++i) {
        test.add(randomInteger(-100,100));
    }
    /* Create equivalent list to use as test input */
    ListNode* list = createList(test);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    test.sort();

    /* Confirm sorted list matches sorted vector */
    printList(list);
    EXPECT(areEquivalent(list, test));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}


STUDENT_TEST("Time degraded linked list quicksort vs vector quicksort") {
    int startSize = 1000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = i;
            list = new ListNode(v[i], list);
        }

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to compare relative speed. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}

STUDENT_TEST("This is a skeleton to demonstrate an end-to-end student test."){
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 3, 4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));

    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

PROVIDED_TEST("This tests some of the testing helper functions you are recommended to implement."){
    /* Creates a small test list containing the values 1->2->3. */
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);

    /* Test equality comparison function. */
    EXPECT(areEquivalent(testList, {1, 2, 3}));
    EXPECT(!areEquivalent(testList, {1, 2}));
    EXPECT(!areEquivalent(testList, {1, 2, 3, 4}));
    EXPECT(!areEquivalent(testList, {1, 3, 2}));

    /* Test create list function. */
    ListNode* studentList = createList({1, 2, 3, 4});
    printList(studentList);
    ListNode* cur = studentList;
    for (int i = 1; i <= 4; i++){
        EXPECT(cur != nullptr);
        EXPECT_EQUAL(cur->data, i);
        cur = cur->next;
    }

    /* Test deallocate list functions. There should be no memory leaks from this test. */
    deallocateList(studentList);
    deallocateList(testList);
}

/*
 * The provided test case demonstrates a comparsion between the same
 * algorithm (quicksort) operating on a linked list versus a vector.
 */
PROVIDED_TEST("Time linked list quicksort vs vector quicksort") {
    int startSize = 50000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to compare relative speed. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}

