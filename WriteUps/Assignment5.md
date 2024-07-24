由于 2022 Summer 的 Assignment5 不开放获取，我只好在往年的网站上寻找资源。发现开放的版本要么文不对题，要么版本对不上。只能用就近的 2022 Winter 了，好在两者都有优先队列和面向对象，用这个替代一下吧。
感觉 Winter 的版本难度和内容量都要大一些。
# PQueue 与 Binary Heap
## 介绍
作业的第一部分是关于优先队列和堆，队列的类定义如下：
```cpp
struct DataPoint {
    std::string name;
    double wieght;
};

class HeapPQueue {
public:
    HeapPQueue();
    ~HeapPQueue();
    
    void enqueue(const DataPoint& data);
    DataPoint dequeue();
    DataPoint peek() const;
    
    bool isEmpty() const;
    int  size() const;
    
    void printDebugInfo();
    
private:
    /* Details coming soon! */
};
```
需要实现以下方法：
`HeaPQueue hpq;`
`hpq.enqueue({"Amy", 103});`
`hpq.enqueue({"Ashley", 101});`
`hpq.enqueue({"Anna", 110});`
`hpq.enqueue({"Luna", 161});`
出队，则结果如下：
`Data data = hpq.dequeue();`
`cout << data.name << endl;`
打印 Ashley。
## 小根堆
我们用来实现优先队列的数据结构是小根堆，有关它的性质和匹配方法在堆排序中已经非常熟悉了。这里归纳为几点：

- 堆的逻辑结构为二叉树；
- 每个节点可以有 0、1、2 个子节点；
- 该树为完全二叉树。

堆中加入元素：

1. 首先放到队尾；
2. 之后逐层调整。

每次堆调整的效率非常高，最多需要 logn 次调整，这是我们已经知道的结论。
堆中弹出元素：

1. 将堆顶，也就是权重最小的根节点出队；
2. 然后将队尾元素填到原先堆顶的位置；
3. 之后逐层调整。

与入堆相同，堆中弹出元素时间复杂度也是 O(logn)。
## 堆的物理结构
我们之前说了堆的逻辑结构是二叉树，但是我们可以用动态数组来作为它的物理结构。
这是因为，若根节点表示为 1，则：

- 给定节点编号 n，则其左右子节点编号为 2n 和 2n + 1；
- 给定节点编号 n，则其父节点编号为 n / 2。

这样我们可以通过一个节点的编号快速定位其相关节点，因此用数组作为物理结构是可行的。例如下图的卫星名称以及升空时间：
![satellite_array_heap_2.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720774035542-a3895d30-6c69-4cf1-b03c-06a6bb73f796.png#averageHue=%23e9e8e7&clientId=u15327415-650c-4&from=ui&id=u04b11171&originHeight=288&originWidth=554&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=76490&status=done&style=none&taskId=u80bac199-bd41-4c2b-ba61-2c7ff08882f&title=)
这样就直观多了。如果这时有新元素入堆，则会首先到数组末尾，编号为 8。
![satellite_array_enqueue_1.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720774523496-e3e1f7ac-f134-4cf7-badc-389d36afa8e4.png#averageHue=%23ecebea&clientId=u15327415-650c-4&from=ui&id=u35f501a6&originHeight=360&originWidth=684&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=79857&status=done&style=none&taskId=ua35d06bd-fb95-45e8-b88a-dabcbd55da6&title=)
之后和它的父节点 4 比较，发现权值小于父节点，按照规定和父节点交换。
![satellite_array_enqueue_2.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720774577177-44eed2c9-1bd8-482a-abaf-73eeaa8f0afc.png#averageHue=%23ebebea&clientId=u15327415-650c-4&from=ui&id=uc47906d1&originHeight=360&originWidth=688&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=82766&status=done&style=none&taskId=u9433b09a-b01b-4507-a9f0-ae14481e82a&title=)
仅有一级交换还不够，可以发现新元素仍然比其父节点值更小，所以需要现在的父节点 2 交换。
![satellite_array_enqueue_3.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720774642429-cf066824-fc06-4bf1-8c28-c9585c106862.png#averageHue=%23ecebea&clientId=u15327415-650c-4&from=ui&id=ue0ad5580&originHeight=360&originWidth=688&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=82909&status=done&style=none&taskId=uc03321cf-cbe5-43bb-a373-5c188dbaf35&title=)
交换过后，新元素和堆顶比较，发现大于堆顶，也就不需要再交换了。
总结以上步骤，我们可以发现逐层的比较是有很强的规律性的，可以利用递归来解决，当然迭代也可以。
需要注意的是，方便起见，我们用来存储 DataPoint 的动态数组是从 1 开始计数的。而 0 位置并没有使用。
## 堆的实现
### 任务一：类定义
现在我们来着手实现这个堆。丰富其定义如下：
```cpp
class HeapPQueue {
public:
    HeapPQueue();
    ~HeapPQueue();

    void enqueue(const DataPoint& data);
    DataPoint dequeue();
    DataPoint peek() const;

    bool isEmpty() const;
    int  size() const;

    void printDebugInfo();

private:
    DataPoint* elems = nullptr;
    int logicalSize = 0;
    int allocatedSize = 0;

    static const int INITIAL_SIZE = 6;
};
```
私有变量的定义：

- elems：指向动态数组的指针，默认为 nullptr。
- logicalSize：目前优先队列中的元素数目；
- allocatedSize：数组已动态分配的大小。
- INITIAL_SIZE：数组的初始大小。

同样将任务插接，分成几步实现。
### 任务二：构造析构、判断大小
首先，我们要实现动态数组的构造函数和析构函数，并判断是否为空，判断队列中元素数。函数原型如下：
```cpp
    /*
     * Creates a new, empty priority queue.
     */
    HeapPQueue();

    /*
     * Cleans up all memory allocated by this priorty queue. Remember, you're responsible
     * for managing your own memory!
     */
    ~HeapPQueue();

    /*
     * Returns whether the priority queue is empty.
     *
     * This operation must run in time O(1).
     *
     * @return Whether the priority queue is empty.
     */
    bool isEmpty() const;

    /*
     * Returns the number of data points in this priority queue.
     *
     * This operation must run in time O(1).
     *
     * @return The number of elements in the priority queue.
     */
    int  size() const;
```
贴出我的实现代码：
```cpp
/*
 * 构造函数
 * 将动态分配空间设为初始大小
 * 之后将指针指向动态分配的 DataPoint 数组
 */
HeapPQueue::HeapPQueue() :allocatedSize(INITIAL_SIZE), logicalSize(0) {
    elems = new DataPoint[INITIAL_SIZE];
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
```
测试通过。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720776933901-fc64ae36-e777-4fbd-8db5-bc1bcbb189b0.png#averageHue=%23faf8f7&clientId=u15327415-650c-4&from=paste&height=47&id=u6f337c42&originHeight=70&originWidth=1070&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=9709&status=done&style=none&taskId=uee6fa3a1-a696-4b2e-a60f-36304050339&title=&width=713.3333333333334)
### 任务三：入队
入队的逻辑也可以分成两部分，堆调整和内存管理。

1. 首先需要注意，入队可能导致数组空间不足，需要检测。
2. 若空间足够，则继续实现堆调整逻辑。
3. 之后就可以去掉空间不足的检测，改写为空间继续分配。

注意数组 0 位置不使用。
给出的我的实现代码：
```cpp
/*
 * 入队函数
 * 若空间不足，则抛出错误
 * 否则入队，然后调整堆
 */
void HeapPQueue::enqueue(const DataPoint& data) {
    if(logicalSize == allocatedSize - 1)
        error("Array is full now");
    elems[++logicalSize] = data;
    adjustHeap();
}

/*
 * 堆调整函数
 * 迭代地与父节点判断，若值小于父节点则交换值
 * 也可以用递归来解决。
 */
void HeapPQueue::adjustHeap() {
    for(int i = logicalSize;i > 1;i /= 2) {
        if(elems[i].weight < elems[i / 2].weight)
            std::swap(elems[i],elems[i / 2]);
    }
}
```
首先会在超出容量时报错：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720780702872-4364c280-6fa2-43c3-bf37-f590e4dcef49.png#averageHue=%23f9f6f4&clientId=u15327415-650c-4&from=paste&height=211&id=u0b80de1b&originHeight=316&originWidth=1085&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=43668&status=done&style=none&taskId=u2c88f305-1e1f-41ce-b437-4a88b3e6284&title=&width=723.3333333333334)
之后我们添加上扩充容量的函数，综合如下：
```cpp
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
    adjustHeap();
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
```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720782536927-7ce39aff-d084-4a2f-920a-22f8665bb113.png#averageHue=%23f8f5f4&clientId=u15327415-650c-4&from=paste&height=237&id=u5220933c&originHeight=356&originWidth=1098&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=51545&status=done&style=none&taskId=ud7269904-6d80-44b9-82cb-eeacd99f0c4&title=&width=732)
可以看出，实现没有问题。
### 任务四：查看队首、出队
之后查询队首和出队函数，查询队首比较简单，若队列为空则报错。
而出队操作需要返回堆顶元素，若堆为空同样报错。所以我们还需要一个向下调整的函数，和向上调整的过程刚好相反。但是由于一个节点可能存在两个子节点，所以需要考虑一些边界情况。
```cpp
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
```
最后一个节点的父节点为 logicalSize / 2，它一定有左子但不一定有右子。所以需要一个条件来做越界访问的处理。
而且这个迭代也存在剪枝：当一层不需要调整，那么下面几层都不需要了。这样可以直接返回，若不满足则需要将元素交换。为了处理连锁反应，下一轮调整中需要将被置换的节点视为父节点。
最后测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720802124808-1d501c34-9306-4514-891a-7bba16b04606.png#averageHue=%23f9f8f6&clientId=u08796079-43c2-4&from=paste&height=649&id=ub0605c43&originHeight=974&originWidth=1255&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=146495&status=done&style=none&taskId=ubd998fdc-7bfc-490e-bbc1-2f69614257b&title=&width=836.6666666666666)
测试全部通过！
### 复杂度检查
通过时间测试，发现入队和出队用时为：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720802456809-27c2b87a-d4b1-454e-8eb9-0a59cb7aa121.png#averageHue=%23fbf9f9&clientId=u08796079-43c2-4&from=paste&height=719&id=u96e25153&originHeight=1078&originWidth=1488&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=89692&status=done&style=none&taskId=u1bcc248a-02dc-4269-9791-d8f5a9432c5&title=&width=992)
我们知道入堆和出堆的时间复杂度都是 O(nlogn)，而 logn 非常小，所以可以认为是近似的线性复杂度，图表也反映了这一点。
# Apportionment
## 介绍
美国每十年进行一次人口普查，统计五大地区的人口数量。然后这些数字将被用来确定众议院中每个州将获得多少席位。这个从州人口到席位数量的过程被称为分摊（Apportionment）。
如何公平分配席位是一个棘手的问题，有一种名为亨廷顿-希尔的算法可以解决。
## Huntington-Hill 算法
该算法的工作原理如下：首先，给每一个州都分配一个席位；之后将所有州放到一个小根堆中，其权重为该州的人口除以 ![](https://cdn.nlark.com/yuque/__latex/ad9f8ac78aa46f4d4950680c3a3e9915.svg#card=math&code=%5Csqrt%7B2%7D&id=cTolU)。最后重复以下过程给每个州安排席位：

1. 将最高权重的州从堆中移除；
2. 给该州分配一个席位；
3. 将该州的权重重新设为 ![](https://cdn.nlark.com/yuque/__latex/99eae24004be5bd0e429b4fdeaa8d546.svg#card=math&code=%5Cfrac%7BP%7D%7B%5Csqrt%7BS%2A%28S%2B1%29%7D%7D&id=DoyOH)，P 为该州人口，S 为该州已分配的席位数量，包含第二步分配的那一个。

举一例说明，现在有 5 个州，他们的人口数据如下：

- Affairs：7984
- Being：14938
- Denial：13509
- Matter：14617
- Mind：25419

假设待分配的席位有 14 个。首先给每个州都分配一个席位，然后将人口除以 ![](https://cdn.nlark.com/yuque/__latex/ad9f8ac78aa46f4d4950680c3a3e9915.svg#card=math&code=%5Csqrt%7B2%7D&id=OCJ6t)，放入到大根堆中。处理结果如下：

- Affairs：5645.54
- Being：10562.76
- Denial：9552.31
- Matter：10355.95
- Mind：17973.95

现在将该州弹出，分配一个席位。于是 Mind 获得第二个席位，权重变成 ![](https://cdn.nlark.com/yuque/__latex/2ccf33557790b37977465504c9e82a54.svg#card=math&code=%5Cfrac%7B25419%7D%7B%5Csqrt%7B2%2A%282%2B1%29%7D%7D%5Capprox&id=rWTUY)10377.26。之后 Mind 被重新入堆。

- Affairs：5645.54
- Being：10562.76
- Denial：9552.31
- Matter：10355.95
- Mind：10377.26

此时最大权重是 Being，于是弹出堆并获得第二个席位，计算之后重新入堆。重复以上操作，直到最后所有席位都被分配完。
接下来我们就开始着手实现相关算法。
## 任务：实现分摊
首先第一个任务是实现分摊，函数原型如下：
`Map<string, int> apportion(const Map<string, int>& populations, int numSeats);`
函数读取一个州人口数据映射表，席位数，返回最终的席位分配表。
作业的要求是，不能修改之前关于 HeaPQueue 类的实现。虽然我们之前实现的是小根堆，但是必须用它来找权重的最大州，这是最大的难点。
你也许会想，既然是小根堆，那么最大值肯定出现在最后一层吧，那可不一定。如果你翻回到第一部分关于堆的介绍，就会发现最大值也可能不在最后一层，例如下图：
![](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720774642429-cf066824-fc06-4bf1-8c28-c9585c106862.png?x-oss-process=image%2Fformat%2Cwebp#averageHue=%23ecebea&from=url&id=gPRrN&originHeight=360&originWidth=688&originalType=binary&ratio=1.5&rotation=0&showTitle=false&status=done&style=none&title=)
最大值为 2005，出现在倒数第二层。由于队列的动态数组是私有成员，所以不能通过遍历来求值。但是堆逻辑结构是二叉树，也许可以通过类似递归二叉树来求最大值。
但是即使是递归，也需要获得根节点。而我们无法获取 elems 的引用，也就没办法递归了。
我们不妨转换思路，既然在小根堆找最大值很难，我们就让最大值变成最小值，取它权重的相反数不就行了。
也就是：
```cpp
/*
 * 预处理函数
 * 初始化分配表，给每个州预先分配一个席位
 */
void preProcess(const Map<std::string,int>& populations, Map<std::string,int>& re) {
    for(const std::string& state: populations) {
        re[state] = 1;
    }
}



/*
 * 建堆，筛选，直到分配完毕
 */
void apportionHelper(const Map<std::string,int>& populations, int numSeats, Map<std::string,int>& re) {
    HeapPQueue hpq;
    for(const std::string& state: populations) {
        double weight = populations[state]  / sqrt(2);
        hpq.enqueue({state, -weight});
    }

    while(numSeats) {
        DataPoint top = hpq.dequeue();
        ++re[top.name];
        --numSeats;

        top.weight = -(populations[top.name] / sqrt(re[top.name] * (re[top.name] + 1)));
        hpq.enqueue(top);
    }
}


/*
 * 分摊函数
 * 若席位数量少于州数量，则抛出错误
 */
Map<string, int> apportion(const Map<string, int>& populations, int numSeats) {
    if(populations.size() > numSeats)
        error("Not sufficient seats offered");
    Map<std::string,int> re;
    preProcess(populations,re);
    numSeats -= re.size();

    apportionHelper(populations, numSeats,re);

    return re;
}
```
最后测试通过。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720860434582-eed7c53e-b7a3-4415-b3a5-b698a768fb0b.png#averageHue=%23fbfaf9&clientId=u6bc64e3e-15fe-4&from=paste&height=130&id=ud69e5ee2&originHeight=195&originWidth=1466&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=27375&status=done&style=none&taskId=u9ca1e022-7d35-492a-a45e-0f674ec717e&title=&width=977.3333333333334)
这里我不得不说，我最初的计算权重方法是先对 P 取平方，然后除以分母，最后对结果开平方。但是与测试误差较大：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720860736819-f6ece6c6-8a71-4ef0-9e91-3bff05526c13.png#averageHue=%23f8f3f2&clientId=u6bc64e3e-15fe-4&from=paste&height=197&id=u9d10ae05&originHeight=295&originWidth=1478&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=41704&status=done&style=none&taskId=u6a706f6b-f3c0-42fc-95d1-73b12e5a29a&title=&width=985.3333333333334)
从结果可以看出，误差确实比较大。非常奇怪的一点。
最后有一个流言测试，说若 New York 州多 89 人，或者 Minnesota 州少 26 人，则前者多一席位，后者少一席位。已经给出了调整后的数据集，现在来验证流言是否为真。
```cpp
STUDENT_TEST("Test MNDown and NYUp") {
    Map<std::string,int> dataMN = populationsFrom("res/apportionment/2020-MNDown26.csv");
    Map<std::string,int> dataNY = populationsFrom("res/apportionment/2020-NYUp89.csv");
    auto ap1 = apportion(dataMN, 435);
    auto ap2 = apportion(dataNY, 435);
    std::cout << ap1 << std::endl;
    std::cout << ap2 << std::endl;
}
```
最后得到以下结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720862252224-c7baf6ff-2af6-41d1-8390-e7691dc245d2.png#averageHue=%23ebe8e6&clientId=u6bc64e3e-15fe-4&from=paste&height=283&id=ufafe19d5&originHeight=425&originWidth=1992&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=105236&status=done&style=none&taskId=ud484b9cc-c693-4df9-8979-e1240db0bd7&title=&width=1328)
可以发现，并不是传言那么回事。
