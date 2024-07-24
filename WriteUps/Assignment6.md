第六部分主要内容是链表，属于老生常谈的话题了。包含两部分：逃出迷宫和排序链表。
# Labyrinth Escape
## 介绍
现在你被困在迷宫中，唯一的逃出方法是释放魔法咒语。为此你需要探索迷宫，找到三个魔法物品：

- Spell Book：魔法书，记载了咒语
- Potion：魔法药水，含有让咒语生效的成分
- Wand：法杖：让你集中注意力

只要凑齐三件物品，就可以离开迷宫。
与一般的迷宫不同，本章迷宫是一个链表迷宫，由多个相互连接的小隔间组成。其结构如下：
```cpp
struct MazeCell {
    std::string contents;   // empty string if this cell contains no item

    MazeCell* north;
    MazeCell* south;
    MazeCell* east;
    MazeCell* west;
};
```
如上所示，不同隔间之间通过指针相互串联织成迷宫。如以下示例：
![labyrinth.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720940314523-d03eb750-eacd-4484-b1c4-2b9ae4d29855.png#averageHue=%23edecec&clientId=u9d4594cf-e133-4&from=ui&id=uaf746672&originHeight=291&originWidth=288&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=79235&status=done&style=none&taskId=u0b2664f3-7b61-44f6-804d-f74a23bee34&title=)
每个隔间都可能存在物品，三种魔法物品名为 Spellbook、Potion 和 Wand。若隔间无物，则 content 字段为空串。
笑脸即为初始位置，你需要在迷宫中探索，可能的路径如下：

- **ESNWWNNEWSSESWWN**
- **SWWNSEENWNNEWSSEES**

可以验证，以上路径都可以收集到 3 件物品。
## 任务一：判断路径
第一个任务是判断给定起始点和路径、检查能否收集到全部物品。原型如下：
`bool isPathToFreedom(MazeCell* start, string moves, Set<string> needs);`
函数读取一个指针、路径字符串和当前需要的物品集合。若路径走完后仍未能收集全物品，那么返回 false。
注意对路径合法性的判断，不能走到迷宫之外。
```cpp
bool isPathToFreedom(MazeCell* start, string moves, Set<string> needs) {
    if(needs.contains(start->contents))
        needs.remove(start->contents);
    for(char step: moves) {
        switch(step) {
        case 'E':
            start = start->east; break;
        case 'W':
            start = start->west; break;
        case 'S':
            start = start->south; break;
        case 'N':
            start = start->north; break;
        default:
            error("Invalid direction");
        }
        if(start == nullptr)
            return false;

        needs.remove(start->contents);
        if(needs.isEmpty())
            return true;
    }

    return needs.isEmpty();
}
```
测试通过。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720942662485-ee7abe7a-3357-4df2-a55e-3097fd20845f.png#averageHue=%23dcdbd6&clientId=u9d4594cf-e133-4&from=paste&height=168&id=u70029ab0&originHeight=252&originWidth=764&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=27900&status=done&style=none&taskId=u88dea3c1-e2bb-44d3-abc2-ee694bd30c2&title=&width=509.3333333333333)
## 任务二：探索自定义迷宫
第二步，我们需要给出自己的 ID，然后程序会自动生成一个迷宫，之后 ID 就不要更改了。
在给出的测试案例中下断点调试，找到合法的路径。可以边调试边画图，最后根据路径画出路线，使用任务一的函数判断自己的路径是否可以离开迷宫。
由于这个迷宫不是完美迷宫，可能存在环路或多条路径，而且隔间之间可能存在单向路。
这里我的迷宫探索轨迹如下所示：
![](https://cdn.nlark.com/yuque/0/2024/jpeg/43291115/1720944671765-c3860c8b-2432-46d1-abca-1b04d4bcc275.jpeg)
路径为 SSEESNNESSNNWWNE。
经检查：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720944991669-20543582-ddda-4ea3-bad1-ec2813ec4e5a.png#averageHue=%23dcdad6&clientId=u9d4594cf-e133-4&from=paste&height=201&id=uea8e9ef0&originHeight=302&originWidth=754&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=33023&status=done&style=none&taskId=ub4524982-d51c-476e-b458-924f51dc0af&title=&width=502.6666666666667)
确实是有效路径。

# Sorting LinkList
## 介绍
这一部分作业是编写一个自动排序的链表，与以往不同的是，我们需要写更多工具函数，例如：

- 打印链表
- 释放链表内存
- 创建链表
- 判断链表和 Vector 的值是否一一对应

至于链表排序，使用的算法是快速排序。
需要注意，根据课程录像来看，实验中的链表是不带头结点的。
## 任务一：工具函数
### 链表打印
这一部分 cpp 文件已经给出了代码，我们就沿用这个。
```cpp
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
```
### 链表建立
该函数读取一个 Vector，按照其中的值和顺序建立链表，保证顺序一致。那么我们只能使用尾插法建立链表。
```cpp
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

STUDENT_TEST("Test createList") {
    Vector<int> test = {5,6,7,8,9};
    ListNode* list = createList(test);
    printList(list);
}
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720949187839-c89a3a34-3a73-4559-9f3c-1371b0abd67e.png#averageHue=%23fbfbfa&clientId=u9d4594cf-e133-4&from=paste&height=63&id=u77013f22&originHeight=94&originWidth=1305&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=7594&status=done&style=none&taskId=u91694132-0fda-4f2f-a64e-c75c61059d6&title=&width=870)
证明我们的链表建立是正确的。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720949625723-de9a48d6-a5dc-4e74-a0ab-a96070f924e9.png#averageHue=%23e5deca&clientId=u9d4594cf-e133-4&from=paste&height=104&id=u7720714c&originHeight=156&originWidth=760&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=15558&status=done&style=none&taskId=u1061c583-4532-4355-86e6-45d1a8b6b9c&title=&width=506.6666666666667)
由于没有写销毁函数，所以测试警告了内存泄露。
### 链表销毁
链表销毁函数读取一个节点，然后释放其中申请的所有内存。
原型如下：
`void deallocateList(ListNode* front);`
给出实现：
```cpp
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
```
还是同样的数据，测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720949951691-8e560990-e0a6-472c-a274-071ec220f90e.png#averageHue=%23e9e8e6&clientId=u9d4594cf-e133-4&from=paste&height=75&id=u00e7f9ee&originHeight=113&originWidth=764&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=10492&status=done&style=none&taskId=ud6f47cb5-1b91-4cf2-9be6-cea882aff4c&title=&width=509.3333333333333)
可以发现，测试通过了，没有段错误，没有内存泄露。
### 链表检查
最后一个函数是检查链表和 Vector 中的数据是否一一对应。
原型：
`bool areEquivalent(ListNode* front, Vector<int> v);`
为了方便检查，我们再写一个获取链表长度的函数，这样若两输入长度不同则直接返回 false。
整体代码如下：
```cpp
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


STUDENT_TEST("Test createList") {
    Vector<int> test = {5,6,7,8,9};
    ListNode* list = createList(test);
    printList(list);
    if(areEquivalent(list,test))
        std::cout << "The list is equivalent to vector\n";
    deallocateList(list);
}
```
测试得到：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720951453302-dfc9f544-cf57-47dd-91b6-8254985d924d.png#averageHue=%23f4f3f2&clientId=u9d4594cf-e133-4&from=paste&height=347&id=u300e78d4&originHeight=520&originWidth=1323&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=57444&status=done&style=none&taskId=ue21bddba-ec28-4e19-be39-5aba772efee&title=&width=882)
工具函数的测试通过。

## 任务二：快速排序
任务一中，我们完成了关于链表的一系列工具函数。接下来需要借助这些工具来实现基于链表的排序，使用的排序算法为快速排序。原型如下：
`void quickSort(ListNode*& front);`
有关快速排序的原理我们已经非常熟悉了，这里主要讲讲在链表上如何操作。
我们知道快排是基于分治法的思想。在数组快排中，我们不断将其划分为左右两个子数组，分别排序之后再合并。但是链表不支持随机存取，所以需要自己写链表的拆分和合并函数，命名为 partition 和 concatenate，并各自测试。
测试用例大概需要：

- 空链表
- 单一元素的链表
- 已有序或是恰好逆序的链表
- 含有重复元素或是全部为同一元素的链表
- 含有正负值的链表
- 随机结构的链表
- 长链表
### 链表分割
partition 中，我们将链表的首元素置为主元，然后将原来的链表划分为三条：严格小于主元的元素、等于主元的元素以及严格大于主元的元素。现有的节点会被分配到子链表中，不要额外分配空间或者将节点全部拷贝。节点放入子表顺序无关。
另外的问题在于，C++ 只能返回一个值，但是我们的链表划分为 3 条。所以我们可能需要传入引用来修改。与值传递指针不同，引用传入指针既可以修改指针指向的对象，也可以修改指针，让原指针指向不同的对象。
给出以下代码：
```cpp
/*
 * 链表分割
 * 遍历将原始链表拆分为三条
 * 我们将原表以传入 re，第一个节点值就是主元
 */
void partition(ListNode* list, ListNode*& lessFront, ListNode*& equalFront, ListNode*& greaterFront) {
    if(list == nullptr)
        return ;

    int pivot = list->data;
    ListNode* p = list->next; 

    equalFront = list;
    ListNode* equal = equalFront;
    equal->next = nullptr;

    ListNode* less = lessFront;
    ListNode* greater = greaterFront;

    while(p) {
        if(p->data < pivot) {
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

        p = p->next; 
    }
    if(less)
        less->next = nullptr;
    if(greater)
        greater->next = nullptr;
    if(equal)
        equal->next = nullptr;

}
```
函数传入四个指针，除了原来的链表外，左中右分别为小于、等于和大于主元节点的链表。
测试用例如下：
```cpp
STUDENT_TEST("Test linklist partition") {
    ListNode* list = createList({5,3,9,7,2,8,1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

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
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test single element list") {
    ListNode* list = createList({1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test reverse sorted element list") {
    ListNode* list = createList({6, 5, 4, 3, 2, 1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}


STUDENT_TEST("Test some dupplicate elements list") {
    ListNode* list = createList({1,1,2,6,6,5,9,7,3});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}


STUDENT_TEST("Test all duplicate elements list") {
    ListNode* list = createList({1,1,1,1,1,1,1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}


STUDENT_TEST("Test positive and negative element list") {
    ListNode* list = createList({1, -2, 3, -4, 5, -6});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}


STUDENT_TEST("Test long list") {
    Vector<int> test;
    for(int i = 0;i < 100;++i) {
        test.add(randomInteger(-100,100));
    }
    ListNode* list = createList(test);
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);
    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720961586049-5a879131-1dfd-440c-842e-54a4057e66ed.png#averageHue=%23e0dfdc&clientId=u9d4594cf-e133-4&from=paste&height=306&id=u5f1faf6f&originHeight=459&originWidth=1025&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=46806&status=done&style=none&taskId=u50780c4e-ebe9-43f1-a3a1-8386edca829&title=&width=683.3333333333334)
测试全部通过，看来我们的分割没有问题。
### 链表合并
合并函数用来将三个子链表合并为一个链表，注意考虑空链表的情况。
由于 quickSort 需要递归调用自己来实现排序，所以 partition 和 concatenate 必须是迭代式的。
最后是几点要求：

- 只能改变链表的连接方式来排序，不能只是单纯的交换值，也不能分配或者释放空间。但是可以使用辅助指针。
- 不要使用标准库的排序函数和 STL 库的工具。
- 不要借用辅助数据结构例如 array、Vector、Map 等。

由于直接将三条合并，需要考虑的条件比较多，于是我们决定写一个两两合并函数，简化流程。
给出我的实现：
```cpp
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

```
测试用例如下：
```cpp
STUDENT_TEST("Test empty linklist partition") {
    ListNode* list = createList({});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);

    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test single element list") {
    ListNode* list = createList({1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);

    deallocateList(left);
    deallocateList(list);
    deallocateList(right);
}

STUDENT_TEST("Test reverse sorted element list") {
    ListNode* list = createList({6, 5, 4, 3, 2, 1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);

    deallocateList(temp);
    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test some dupplicate elements list") {
    ListNode* list = createList({1,1,2,6,6,5,9,7,3});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    deallocateList(temp);

    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test all duplicate elements list") {
    ListNode* list = createList({1,1,1,1,1,1,1});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
    deallocateList(temp);
    // deallocateList(left);
    // deallocateList(list);
    // deallocateList(right);
}


STUDENT_TEST("Test positive and negative element list") {
    ListNode* list = createList({1, -2, 3, -4, 5, -6});
    ListNode* left = nullptr;
    ListNode* right = nullptr;
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
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
    partition(left,list,right);

    printList(left);
    printList(list);
    printList(right);

    ListNode* temp = concatenate(left, list, right);
    printList(temp);
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
    partition(left,list,right);

    left = concatenate(left,list,right);
    printList(left);

    deallocateList(left);
}
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720965747925-e0676e23-c4b1-4b00-be1b-32067e2d0dbe.png#averageHue=%23f4f3f2&clientId=u9d4594cf-e133-4&from=paste&height=613&id=ubfe1be16&originHeight=919&originWidth=1989&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=138227&status=done&style=none&taskId=uee756802-d54d-41b6-b4d9-fa1f093839b&title=&width=1326)
可以看出链接没有问题。
但是我要说这个函数写得不太安全，最后将链表合并后有多余的指针，但是指针没有被置空的话并不安全。
### 快速排序
最后一步，需要考虑的边界情况是：当链表为空，或者只有一个元素时，直接返回即可。
否则在内部先划分左中右表，分别对左右表递归划分，最后通过 concatenate 来链接。
```cpp
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

```
给出测试代码：
```cpp
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
```
最后测试：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720968841897-79b20a31-bbd5-4a1d-b22f-a905838b1e3d.png#averageHue=%23aaa046&clientId=u9d4594cf-e133-4&from=paste&height=621&id=u4886716b&originHeight=932&originWidth=1038&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=108264&status=done&style=none&taskId=u7e38989f-21e6-42ea-a056-30d10fa88f4&title=&width=692)
排序结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720969410683-40da3f0a-5d3e-42ea-82de-dde2e40b9d97.png#averageHue=%23f8f7f5&clientId=u9d4594cf-e133-4&from=paste&height=117&id=u201c6092&originHeight=175&originWidth=1332&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=25704&status=done&style=none&taskId=u6a10e270-37f6-4a41-8619-46328491c89&title=&width=888)
测试通过。
值得一提的是，参考[2]中使用的方法和我大致相同。其中划分函数写得更为简洁，使用的是头插法，不需要考虑更多条件，可以参考一下。
### 复杂度分析
我们观察以上的测试结果图，可以发现，链表上的快速排序比 Vector 的排序方法要快很多——尽管 Vector 的排序方法是基于高效的 std::sort。
但是当原始序列有序或逆序时，快速排序的效率就会急剧恶化，退化为冒泡排序。同时由于递归的存在，占用的空间复杂度为 O(n)。在我们将问题规模设为 50000 时，直接发生了爆栈。
```cpp
STUDENT_TEST("Time degraded linked list quicksort vs vector quicksort") {
    int startSize = 50000;

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
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720969931362-8b6a4b84-d102-4b4a-83bb-c00df58fb313.png#averageHue=%23e8e7e6&clientId=u9d4594cf-e133-4&from=paste&height=497&id=u4a6270bd&originHeight=746&originWidth=1044&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=72780&status=done&style=none&taskId=u5bbb221b-3f28-4eed-9bfd-96fd8891092&title=&width=696)
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720969938376-8f3684bd-8562-4795-99de-e258a5c417ea.png#averageHue=%23fffff9&clientId=u9d4594cf-e133-4&from=paste&height=26&id=ua69d0b2d&originHeight=39&originWidth=789&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=5944&status=done&style=none&taskId=ud98768a7-fd34-4759-ad97-82b1a53dced&title=&width=526)
于是回调参数，将 startSize 设为 1000，这时可以正常排序，但是在性能上已经远远落后于 Vector 的 sort 了。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720970175062-02ffb4cd-98bc-4564-9d0d-3b60d6814c0f.png#averageHue=%23bfe7a9&clientId=u9d4594cf-e133-4&from=paste&height=128&id=u609b7860&originHeight=192&originWidth=987&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=41513&status=done&style=none&taskId=u22bf77c3-bf08-4321-8507-7a7755a09c9&title=&width=658)
# 参考文章

- [https://juejin.cn/post/7029650181830230052](https://juejin.cn/post/7029650181830230052)
- [https://github.com/before-Sunrise/cs106B-2020summer/blob/master/assignment/assign5/assign5-starter/src/sorting.cpp](https://github.com/before-Sunrise/cs106B-2020summer/blob/master/assignment/assign5/assign5-starter/src/sorting.cpp)
- [https://www.cnblogs.com/choumeng/p/6410247.html](https://www.cnblogs.com/choumeng/p/6410247.html)
