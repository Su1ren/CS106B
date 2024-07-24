最后一次作业内容使用了 2022 Winter 的 assignment7，主题为哈希表。在 2020 Summer 的课程录像中，我们学习了拉链法哈希；作业中，我们主要探索的是另外的两种方法：

- 线性探查：发生冲突时，存储位置后移；
- Robin Hood 哈希：在线性探查上做了些许修改，降低其检索开销。

本次作业我们要代码实现以上方法，评估它们的效率，并与拉链法比较。
# Linear Probing
## 线性探查回顾
线性探查法与拉链法的区别在于：

- 在拉链法中，每个 slot 都存放了一个桶，桶可以存放任意数量的元素；而线性探查法中，最多只能存放一个元素。
- 拉链法的 slot 中每个元素都有对应的 hash code；而线性探查法中，元素存放的最终位置不一定与 hash code 对应。
- 拉链法可以直接将元素节点删除；而线性探查法不能删除 slot，只能使用标记的方式从逻辑上删除。
## 实现线性探查
我们的任务就是实现 LinearProbingHashTable 这个类，原型如下：
```cpp
class LinearProbingHashTable {
public:
    LinearProbingHashTable(HashFunction<std::string> hashFn);
    ~LinearProbingHashTable();

    bool contains(const std::string& key) const;

    bool insert(const std::string& key);
    bool remove(const std::string& key);

    bool isEmpty() const;
    int  size() const;

    void printDebugInfo();

private:
    enum class SlotType {
        TOMBSTONE, EMPTY, FILLED
    };

    struct Slot {
        std::string value;
        SlotType type;
    };

    Slot* elems;

    /* The rest is up to you to decide; see below */
};
```
我们要实现的类就类似于 Set<string>，存储不重复的 string。但是也有几点区别：

1. 线性探查类自带哈希函数，实践中哈希表应该自带哈希函数。
2. Set<string> 没有存储上限，而线性探查类存在固定的 slot 数，在类构造时已经确定。因此，不能存储超过 slot 数量的元素。

需要注意一点，由于类带有成员类 Slot，我们在编写其返回 Slot 类成员函数时，返回类型必须要注明类作用域。但是在函数体和参数列表中，并不需要这样做。
## 任务一：构造与析构、哈希表元素数
第一步首先实现 LinearProbingHashTable 类的构造与析构函数，原型：
`LinearProbingHashTable::LinearProbingHashTable(HashFunction<string> hashFn);`
`LinearProbingHashTable::~LinearProbingHashTable();`
`int LinearProbingHashTable::size() const;`
`bool LinearProbingHashTable::isEmpty() const;`
构造函数中，我们需要根据传入的哈希函数创建哈希表，将每个 slot 置为空。我们不能修改 slot 类的构造，但是默认的构造可以自动将 SlotType 置空。另外需要保存初始化使用的哈希函数。
```cpp
LinearProbingHashTable::LinearProbingHashTable(HashFunction<string> hashFn) {
    int size = hashFn.numSlots();
    elems = new Slot[size]();
    slotNum = size;
    this->hashFn =  hashFn;
    //(void) hashFn;
}

LinearProbingHashTable::~LinearProbingHashTable() {
    delete [] elems;
    elems = nullptr;
    elemNum = 0;
    slotNum = 0;
}

int LinearProbingHashTable::size() const {

    return elemNum;
}

bool LinearProbingHashTable::isEmpty() const {

    return elemNum == 0;
}
```
我们给线性探查类加上了记录表内元素数目的私有成员 elemNum，以及记录表容量的成员 slotNum，方便操作。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721729313117-96666384-9406-4de8-9a68-1a2f80249859.png#averageHue=%23faf9f8&clientId=u297ad3e0-38bf-4&from=paste&height=41&id=uf616c9db&originHeight=62&originWidth=727&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=6081&status=done&style=none&taskId=u30348904-00cc-4754-8d61-9cb9723145e&title=&width=484.6666666666667)
初始化和析构函数都是定义良好的，没有出现内存泄漏。
## 任务二：元素插入与查询
之后我们要实现元素的插入和查询，这里先不考虑逻辑删除的情况。当表满的时候，返回 false。函数原型：
`bool LinearProbingHashTable::insert(const string& elem);`
`bool LinearProbingHashTable::contains(const string& elem) const;`
我们给出的实现代码如下：
```cpp
bool LinearProbingHashTable::insert(const string& elem) {
    
    if(slotNum == elemNum || contains(elem))
        return false;
    
    int idx = hashFn(elem);
    while(elems[idx].type != empty)
        idx = (idx + 1 + slotNum) % slotNum;
    
    elems[idx].value = elem;
    elems[idx].type = SlotType::FILLED;
    ++elemNum;
    
    return true;
}

bool LinearProbingHashTable::contains(const string& elem) const {
    int idx = hashFn(elem);
    int count = 0; // 最多查询 slotNum 次，避免无限循环
    while(count <= slotNum && elems[idx].type != SlotType::EMPTY) { // 遇到空位，说明线性探查结束
        if(elems[idx].value == elem)
            return true;
        ++count;
        idx = (idx + 1) % slotNum;
    }
    return false;
}
```
特别需要注意的是，insert 不能插入重复元素；contains 的线性探查存在上限，不能陷入死循环。
由于元素插入中的线性探查属于连续试探，因此在线性探查查询过程中一旦遇到空位，则证明线性探查结束，查询的元素不在表中。
到删除之前的测试都能通过：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721731774450-90614b25-29c0-45b2-9ad4-1cfd4afa0010.png#averageHue=%23faf9f8&clientId=u297ad3e0-38bf-4&from=paste&height=221&id=uf2a55a83&originHeight=331&originWidth=1585&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=53789&status=done&style=none&taskId=u005d0daa-0b10-40f5-b642-84ef27eeda5&title=&width=1056.6666666666667)
注意，对于哈希表的查询存在时间复杂度要求（常数级），即不能是单纯的遍历（ O(n) 级）。
## 任务三：元素删除
逻辑删除应该是最复杂的一部分。将元素从哈希表中删除时，不能直接将位置设为空，在本作业中将其置为 `TOMBSTONE`，标识其已被删除。否则会导致在查询其他元素时，遇到空位提前结束。
因此，考虑元素删除之后，我们的插入和查询函数也需要修改，避免被删除的元素被误认为仍存在于表中。
`bool LinearProbingHashTable::remove(const string& elem);`
我们给出实现：
```cpp
bool LinearProbingHashTable::insert(const string& elem) {

    if(slotNum == elemNum || contains(elem))
        return false;

    int idx = hashFn(elem);
    while(elems[idx].type == SlotType::FILLED)
        idx = (idx + 1) % slotNum;

    elems[idx].value = elem;
    elems[idx].type = SlotType::FILLED;
    ++elemNum;

    return true;
}

bool LinearProbingHashTable::contains(const string& elem) const {
    int idx = hashFn(elem);
    int count = 0; // 最多查询 slotNum 次，避免无限循环
    while(count <= slotNum && elems[idx % slotNum].type != SlotType::EMPTY) { // 遇到空位，说明线性探查结束
        if(elems[idx].value == elem && elems[idx].type != SlotType::TOMBSTONE)
            return true;
        ++count;
        idx = (idx + 1) % slotNum;
    }
    return false;
}

bool LinearProbingHashTable::remove(const string& elem) {
    if(isEmpty() || !contains(elem))
        return false;
    
    int idx = hashFn(elem);
    int count = 0; // 避免死循环
    while(count <= slotNum && elems[idx % slotNum].type != SlotType::EMPTY) {
        if(elems[idx].value == elem && elems[idx].type != SlotType::TOMBSTONE) {
            elems[idx].type = SlotType::TOMBSTONE;
            --elemNum;
            return true;
        }
        ++count;
        idx = (idx + 1) % slotNum;
    }
    return false;
}
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721734521350-fd5212c0-7091-4909-a65f-b634a6d6d589.png#averageHue=%23faf9f8&clientId=u297ad3e0-38bf-4&from=paste&height=424&id=u7e3b83f8&originHeight=636&originWidth=1700&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=109659&status=done&style=none&taskId=ucc50178e-f62c-4ab0-850b-c002ee058b7&title=&width=1133.3333333333333)
所有测试结果都通过了。
# Robin Hood
## 介绍
RobinHood 哈希是一种平方探测的变种，相比而言降低了插入、删除、查询的开销。其最大的特点就是引入了一种新概念：DIB（Distance to Initial Bucket），当前元素所在位置与哈希函数计算的位置的距离。要点可以概括为：

- RobinHood 哈希表中每个元素都需要记录其最终位置与哈希表计算位置的距离。在线性探查中，我们则是单循环式地遍历查询。
- RobinHood 哈希表的插入元素时，若被插入的元素与其初始位置的距离比当前遍历的元素更远，那么我们就会将当前位置的元素拿出来，将被插入的元素放在这个位置，然后让这个被取出的元素重复被插入的元素，向后查询。
- RobinHood 不需要逻辑删除。当删除元素时，从被删除元素的位置向后查找，直到直到空位置，或者遇到一个 slot 内 DIB(P) = 0 的位置为止（不包含右边界），最后将这一区间内的元素集体向左移动一个位置。这样我们不仅删除了元素，而且将这一区间内的元素 DIB 都减少了 1。
- RobinHood 哈希表的元素查询可以提前终止。在查询过程中，如果当前查询的元素的 DIB 已经超过了我们查询过的路程距离，那么这个元素一定不在表中。

举一例说明插入过程：
![robinhood.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721753195546-c9df681b-560f-4c36-8483-72ff180a3c2d.png#averageHue=%23f1f0f0&clientId=u9cc0b3d8-4439-4&from=ui&id=u3d28c25b&originHeight=379&originWidth=541&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=36007&status=done&style=none&taskId=u47929dbf-d6d6-4c2c-9999-f2643d444bd&title=)
首先我们将 abcdf 先后插入到哈希表中，第二行图片的右下角的数字表示 DIB 值。
插入 x 时，由哈希函数计算位置为 1，但是位置 1 已有元素 b，其 DIB = 1 > 0。因此我们继续向后试探，遇到 c，DIB = 2 > 1，继续向后。遇到 d，其 DIB = 0 < 2，于是 d 被取出来，x 放到 d 的位置。
之后执行 d 的插入，向后0试探 f，其 DIB = 1 == 1，于是继续向后，插入到表最后的位置。
## 实现 RobinHood
接下来我们的任务是实现 RobinHood 哈希表，给出类原型：
```cpp
class RobinHoodHashTable {
public:
    RobinHoodHashTable(HashFunction<std::string> hashFn);
    ~RobinHoodHashTable();

    bool contains(const std::string& key) const;

    bool insert(const std::string& key);
    bool remove(const std::string& key);

    bool isEmpty() const;
    int  size() const;

    void printDebugInfo();

private:
    static const int EMPTY_SLOT = /* something */;

    struct Slot {
        std::string value;
        int distance;
    };

    Slot* elems;

    /* The rest is up to you to decide; see below */
};
```
如上所示，与线性探查相同，我们需要实现构造析构等一系列函数，而且这个哈希表不会扩容。
需要注意的是，相比线性探查，我们不再需要 SlotType 这个枚举类，所有的 Slot 只有两种可能：要么为空、要么存有元素及其 DIB。对于空的 slot，我们用一个整型常量 EMPTY_SLOT 来表示其 DIB。
## 任务一：构造析构、哈希表元素数
与线性探查相同，我们先实现前四个函数。
```cpp
RobinHoodHashTable::RobinHoodHashTable(HashFunction<string> hashFn) {
    int size = hashFn.numSlots();
    this->hashFn = hashFn;
    elems = new Slot[size]();
    slotNum = size;
    for(int i = 0;i < size;++i) {
        elems[i].distance = EMPTY_SLOT;
    }
}

RobinHoodHashTable::~RobinHoodHashTable() {
    delete [] elems;
    elems = nullptr;
}

int RobinHoodHashTable::size() const {

    return elemNum;
}

bool RobinHoodHashTable::isEmpty() const {

    return elemNum == 0;
}

```
注意初始化时，需要将每个位置的 DIB 置为空，也就是 EMPTY_SLOT。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721805127603-fce7831f-12e8-4093-9427-1ec02ceae05d.png#averageHue=%23fbfaf9&clientId=u46e14bec-0273-4&from=paste&height=45&id=uab4b8bba&originHeight=67&originWidth=701&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=6068&status=done&style=none&taskId=u4b040560-f1f9-493a-80d1-0cc08f41f8a&title=&width=467.3333333333333)
初始化通过。
## 任务二：元素插入与查询
之后是元素的插入与查询。
`bool RobinHoodHashTable::contains(const string& elem) const;`
`bool RobinHoodHashTable::insert(const string& elem);`
给出实现如下：
```cpp
bool RobinHoodHashTable::insert(const string& elem) {
    if(elemNum == slotNum || contains(elem))
        return false;
    int home = hashFn(elem);
    Slot target = {elem, 0}; // 初始映射位置
    while(elems[home % slotNum].distance != EMPTY_SLOT) { // 不是空位，否则可以直接插入
        if(elems[home % slotNum].distance < target.distance) { // 元素插进去，交换出来继续向后查询
            std::swap(elems[home % slotNum], target); // 不要在这里增加元素个数
        }
        home = (home + 1) % slotNum;
        target.distance = (target.distance + 1) % slotNum;
    } // 循环结束，还需要最后一次交换
    std::swap(elems[home % slotNum], target);
    ++elemNum; // 最后再增加
    return true;
}

bool RobinHoodHashTable::contains(const string& elem) const {
    int home = hashFn(elem);
    int count = 0;
    while(elems[home % slotNum].distance != EMPTY_SLOT && count <= elems[home % slotNum].distance) { // 额外的条件可以提前结束
        if(elems[home % slotNum].value == elem)
            return true;
        home = (home + 1) % slotNum;
        ++count; // 注意不要取余，防止死循环
    }

    return false;
}
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721807449316-7b3dca74-55d7-48ea-a66f-79f5c92e984e.png#averageHue=%23faf9f8&clientId=u46e14bec-0273-4&from=paste&height=283&id=u3140d104&originHeight=424&originWidth=1595&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=68530&status=done&style=none&taskId=u0ead06c6-bf22-4853-a356-a4c7f94de2c&title=&width=1063.3333333333333)
一直到删除元素之前，都可以通过。
## 任务三：元素删除
最后是元素删除，函数原型：
`bool RobinHoodHashTable::remove(const string& elem);`
这一部分我们会重复利用到查询的代码，但是为了降低代码重复率，我们添加一个查询元素在表中下标的函数。若元素不在表中，则返回 EMPTY_SLOT。同时修改 contains 方法，检查下标的查询结果是否为空位。
给出实现：
```cpp
bool RobinHoodHashTable::contains(const string& elem) const {
    if(isEmpty() || getElemIdx(elem) == EMPTY_SLOT)
        return false;

    return true;
}

int RobinHoodHashTable::getElemIdx(const string &elem) const {
    int home = hashFn(elem);
    int count = 0;
    while(elems[home % slotNum].distance != EMPTY_SLOT && count <= elems[home % slotNum].distance) {
        if(elems[home % slotNum].value == elem)
            return home % slotNum;
        home = (home + 1) % slotNum;
        ++count;
    }

    return EMPTY_SLOT;
}


bool RobinHoodHashTable::remove(const string& elem) {
    if(isEmpty() || !contains(elem))
        return false;

    int realPos = getElemIdx(elem);
    int startPos = realPos;
    elems[realPos].distance = EMPTY_SLOT;
    while(elems[(startPos + 1) % slotNum].distance > 0) { // 不是空位，DIB != 0
        std::swap(elems[(startPos + 1) % slotNum],elems[startPos]);
        --elems[startPos].distance;
        startPos = (startPos + 1) % slotNum;
    }
    --elemNum;
    return true;
}
```
测试如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721809556110-f9a345e3-965e-4ed4-9198-51fa29116f25.png#averageHue=%23faf9f8&clientId=u46e14bec-0273-4&from=paste&height=444&id=u2d5a44f0&originHeight=666&originWidth=1599&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=111385&status=done&style=none&taskId=ude5c9669-d22a-4f06-b5c2-b4ab591e11e&title=&width=1066)
全部测试都能通过了。
# 性能分析
拉链法实现的哈希表已经包含在 Demo 目录下了，现在我们要拿来与线性探查法和开放定址法比较，测试它们的性能。
## 测试方法
我们将以如下方式测试哈希表的性能：

- 将 res 目录下的 EnglishWords.txt 中的所有单词插入到一个空表中，每个单词都插入两次：第一次测试成功插入的速率；第二次则是测试当元素已存在时，插入失败的速率。
- 插入完毕后，查询每个单词，测量查询成功时每次查询的平均用时。
- 查询每个单词的大写版本，由于表中的单词都是小写的，所以可以测量查询失败的平均用时。
- 尝试删除每个单词的大写版本，用来测量删除失败的平均用时。
- 尝试删除每个单词的小写版本，用来测量删除成功的平均用时。

我们将在不同的负载情况下测试性能，记录用时，可能花 5 - 10 分钟。
测试结果中，纵轴表示负载因子，横轴表示哈希表的类型，作对比处理。
## 测试结果分析
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721811246394-e752947e-7615-4a0a-a9dc-7d1674100b75.png#averageHue=%23f3f2ab&clientId=u46e14bec-0273-4&from=paste&height=871&id=ucc8427a9&originHeight=1307&originWidth=1038&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=307177&status=done&style=none&taskId=u0f910c73-b001-4bac-bcbf-cc63368be03&title=&width=692)
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721811270915-585d81d7-8cab-4427-9e10-920165807aa1.png#averageHue=%23f4f2b4&clientId=u46e14bec-0273-4&from=paste&height=670&id=u7584a076&originHeight=1005&originWidth=1032&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=234199&status=done&style=none&taskId=u90b0dd91-4059-434d-a9c9-126cc6d308a&title=&width=688)
可以看出，在不同负载因子下，三种哈希表的性能如下表所示：

| 操作 | 拉链法 | 线性探测 | RobinHood |
| --- | --- | --- | --- |
| 查询成功 | 不同负载下稳定低 | 随负载增长，逐渐超过拉链 | 随负载增长，耗时超过线性探测 |
| 查询失败 | 不同负载下稳定低 | 随负载增长耗时剧增，超过拉链 | 随负载增长，耗时超过拉链，但远低于线性探测 |
| 插入成功 | 不同负载下稳定低 | 随负载增长，高负载时超过拉链，低于 RobinHood | 随负载增长，耗时始终高于线性探测 |
| 插入失败 | 不同负载下稳定低 | 随负载增长，逐渐超过拉链 | 随负载增长，耗时始终低于拉链 |
| 删除成功 | 不同负载下耗时波动，整体水平较低 | 随负载增长，高负载时超过拉链 | 随负载增长，耗时始终高于线性探测 |
| 删除失败 | 不同负载下耗时波动，整体水平较低 | 随负载增长，高负载时剧增。 | 不同负载下耗时波动，且始终低于拉链 |


## 实践拓展
在以上的哈希表实现之外，还有一些其他的实践方案：

- 我们实现的线性探测和 RobinHood 并不存在哈希表扩容的情况，但是一般而言哈希表会随着总负载的增大而性能下降。因此在建表时，通常会选择一定的负载因子作为阈值，当检测到总负载达到阈值时，则另立新表。
- 逻辑删除存在一个很大的弊端，就是在多次删除之后，表中会存在大量的逻辑删除标记，使得哈希表的查询效率大大降低。部分哈希表会跟踪被删除元素的位置，达到一定阈值时则重建新表，来清理这些逻辑删除标记。
- 除线性探测外，还有其他的冲突处理方式例如平方探测、双散列等。Cuckoo 使用两个单独的哈希函数，并将每个元素放入两个表中的一个，确保元素位于第一个哈希函数给出的第一个表中，或者位于第二个哈希函数给定的第二表中。Hopscotch 类似于线性探测，但可确保元素永远不会离其原始位置“太远”。FKS 类似于拉链法，但使用两层哈希方案来确保每个元素最多可以在两次探测中找到。
# 参考文章

- [https://code.mforever78.com/security/2016/09/24/robin-hood-hashmap/](https://code.mforever78.com/security/2016/09/24/robin-hood-hashmap/)
