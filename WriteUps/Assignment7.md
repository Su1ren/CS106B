本章作业内容为二叉树部分的综合，是一个哈夫曼编码文件的实现。首先我们复习哈夫曼编码以及哈夫曼树、哈夫曼树序列化的内容，方便后续调试。
# WarmUp
## 介绍
第一部分是复习哈夫曼编码的实现，分为加解码、树序列化、生成最佳哈夫曼树几个环节，最后有关工具函数的测试。
## 任务一：加解码
首先是根据位序列和加密树来对密码解密。在加密树中，每个叶子节点代表一个字符，从根节点到叶节点的路径表示了对应的加密后的密文。例如：
```
        *
       / \
      *   O
     / \
    N   *  
       / \
      M   S
```
将左枝标记为 0，右枝标记为 1。则字符 S 可以加密为位序列： 011。
函数原型为：
`string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits);`
`Queue<Bit> encodeText(EncodingTreeNode* tree, string text);`
我们需考虑如何遍历的问题，由于内部节点不表示字符，所以我们可以用是否遇到叶节点作为译码判据。
给出如下实现，但是目前树还没建起来，因此测试无法通过。
```cpp
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    EncodingTreeNode* root = tree;
    std::string re;
    while(!messageBits.isEmpty()) {
        Bit bit = messageBits.dequeue();
        if(bit == 0)
            tree = tree->zero;
        else
            tree = tree->one;
        if(tree->isLeaf()) {
            re.push_back(tree->ch);
            tree = root;
        }
    }
    return re;
}
```
```cpp
/*
 * 递归遍历编码树
 * 得到叶结点的位序列
 */
void getEncodingTable(EncodingTreeNode* root, Map<char, string>& table, string path) {
    if(root && root->isLeaf())
        table[root->ch] = path;
    else {
        getEncodingTable(root->zero, table, path + '0');
        getEncodingTable(root->one, table, path + '1');
    }   
}

/*
 * 编码函数
 * 根据传入的映射表、明文和队列译码为位序列
 */
void encodeHelper(const Map<char,string>& table, Queue<Bit>& re, const string& text) {
    for(char ch: text) {
        string temp = table[ch];
        for(int i = 0;i < temp.size();++i)
            re.enqueue(temp[i] - '0');
    }
}


Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Queue<Bit> re;
    Map<char,string> encodeTable;
    getEncodingTable(tree, encodeTable, "");
    encodeHelper(encodeTable, re, text);
    return re;
}
```
## 任务二：序列化与反序列化
在哈夫曼编码中，被压缩的数据中必须包含编码树的信息，否则无法解码。
那么问题来了，如何将编码树写入到压缩数据中呢？我们总不能直接将树的形状描绘到文件中去吧，那样做的成本和难度也太高了，而且也不方便解码操作。
所以我们需要一种方法，将编码树的信息解析同被它加密的数据一样，编码为位序列。这样根据压缩后的位序列，我们可以在编码后方便地还原编码树，再实现解码。以上的过程我们称为编码树的序列化以及反序列化。
其中最简单的一种方法是，将树根据树形和字符集合分别序列化。方案如下：

1. 树形序列化中，若根节点位叶节点，则表示为 0；否则表示为 1。之后再对左右子树做序列化。遍历顺序为先序遍历。
2. 字符集合序列化中，我们通过中序遍历，将得到的编码字符组成序列。

例如以下编码树和序列化结果：
```
     *                          *                          *
   /   \                      /   \                       / \
  E     *                    A     *                     *   O
       / \                        /  \                  / \
      W   K                      *    N                N   * 
                                / \                       / \
                               D   B                     M   S
 
  10100                       1011000										1101000
  EWK                         ADBN											NMSO
```
给出函数原型：
`EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves);`
`void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves);`
我们先写序列化函数，需要两个工具函数：先序遍历确定树形；中序遍历确定字符集。
```cpp
/*
 * 递归版先序遍历序列化树形
 * 迭代版需要使用栈，比较麻烦
 */
void getShape(EncodingTreeNode* root, Queue<Bit>& treeShape) {
    if(!root)
        return ;
    if(root->isLeaf())
        treeShape.enqueue(0);
    else
        treeShape.enqueue(1);
    getShape(root->zero, treeShape);
    getShape(root->one, treeShape);
}

/*
 * 递归版中序遍历序列化字符集
 * 迭代版同样要使用栈
 */
void getCharSet(EncodingTreeNode* root, Queue<char>& charSet) {
    if(!root)
        return ;
    getCharSet(root->zero, charSet);
    if(root->isLeaf())
        charSet.enqueue(root->ch);
    getCharSet(root->one, charSet);
}

void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    getShape(tree, treeShape);
    getCharSet(tree, treeLeaves);
}
```
至于反序列化函数，我们需要递归实现，其实也就是变种的根据先序中序遍历结果还原二叉树。无论是先中后序遍历，到达叶结点的顺序是一样的。
```cpp
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if(treeShape.isEmpty())
        return nullptr;
    EncodingTreeNode* root = new EncodingTreeNode(nullptr, nullptr);
    Bit bit = treeShape.dequeue();
    if(bit == 0) // 到达叶节点，赋值
        root->ch = treeLeaves.dequeue();
    else { // 否则继续构造左右子树
        root->zero = unflattenTree(treeShape, treeLeaves);
        root->one = unflattenTree(treeShape, treeLeaves);
    }

    return root;
}
```
但是测试需要工具函数，我们这里没法测试。
## 任务三：生成最佳哈夫曼树
最后的任务是根据输入构造哈夫曼树，使得其压缩效率最佳。
在构造哈夫曼树时，可能会遇到相同权重的情况，因此可能会产生许多等价的镜像哈夫曼树。但是我们并不做区分。
函数原型：
`EncodingTreeNode* buildHuffmanTree(string text);`
首先，我们需要统计字符频率，来确定权重。如果出现的字符少于两种则报错。
然后根据这些频率信息建立叶节点，存入到优先队列中。取出权重最低两种建立中间节点，权重为两字之和，再压入优先队列。直到最后只剩下一个根节点。
```cpp
/*
 * 字符频率统计函数
 * 返回映射表，若映射表少于两项则直接报错
 */
Map<char,int> charFreqStat(const string& text) {
    Map<char,int> stat;
    for(char ch: text) {
        ++stat[ch];
    }
    if(stat.size() < 2)
        error("Text too short to construct encoding.");
    return stat;
}

/*
 * 哈夫曼树建立函数
 * 根据传入的字符频率，首先建立叶节点，赋予权重为频率和。
 * 再将其压入优先队列
 * 提供的树节点不带权重
 * 但是斯坦福库的优先队列可以自定义权重
 */
EncodingTreeNode* buildHelper(const Map<char,int>& stat) {
    PriorityQueue<EncodingTreeNode*> weights;
    for(char ch: stat) {
        EncodingTreeNode* leaf = new EncodingTreeNode(ch);
        weights.enqueue(leaf, stat[ch]);
    }
    while(weights.size() > 1) {
        double leftWeight = weights.peekPriority();
        EncodingTreeNode* left = weights.dequeue();
        double rightWeight = weights.peekPriority();
        EncodingTreeNode* right = weights.dequeue();
        EncodingTreeNode* internal = new EncodingTreeNode(left, right);
        weights.enqueue(internal, leftWeight + rightWeight);
    }
    return weights.dequeue();
}


EncodingTreeNode* buildHuffmanTree(string text) {
    Map<char,int> stat = charFreqStat(text);
    
    return buildHelper(stat);
}
```
## 工具函数测试
### 建立样树
为了方便测试，我们来试着建立以下样树：
```
               *
             /   \
            T     *
                 / \
                *   E
               / \
              R   S
```
树中有 4 个叶节点，字符集为 {T，R，S，E}，以及三个内部节点。函数原型如下：
`EncodingTreeNode* createExampleTree();`
我们可以通过构造函数来一步建立所有关系。
```cpp
EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode* root = new EncodingTreeNode{
        new EncodingTreeNode{'T'},
        new EncodingTreeNode{
            new EncodingTreeNode{
                new EncodingTreeNode{'R'},
                new EncodingTreeNode{'S'}
            },
            new EncodingTreeNode{'E'},
        },
    };

    return root;
}
```
### 释放节点
我们建立节点分配的空间需要通过 delete 释放，所以我们需要一个释放节点的函数。
`void deallocateTree(EncodingTreeNode* t);`
后序遍历的递归版本是比较简单的，给出实现：
```cpp
void deallocateTree(EncodingTreeNode* t) {
    if(t) {
        deallocateTree(t->zero);
        deallocateTree(t->one);
        delete t;
        t = nullptr;
    }
}
```
这样我们一部分测试就能通过了：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721636885827-8d0ca6c5-72ee-436a-a5eb-c9fa1f8d4a1c.png#averageHue=%2361b5a8&clientId=u8e57e067-6ecc-4&from=paste&height=429&id=ued66fb52&originHeight=644&originWidth=1038&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=73504&status=done&style=none&taskId=u283d8ccb-20cb-4ac4-8454-35c82b6f95b&title=&width=692)
### 等价编码树
本函数用来判断两个编码树是否等价，判据为：

- 树形相同；
- 叶节点中对应字符相同·

在测试中需要包含以下几种情况：

- 只有根节点的树和空树返回 false；
- 建立第二个节点，如果和上一条的单节点字符相同则返回 true；
- 调用 createExampleTree 确定和单个节点树判断返回 false；
- 再次调用 create，两次返回的结果树应该相同；
- 比较样树和它的子树，应该返回 false。

给出以下实现及测试代码：
```cpp
bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    if(a == nullptr && b == nullptr)
        return true;
    if((a == nullptr && b != nullptr) || (a != nullptr && b == nullptr))
        return false;
    if(a->isLeaf() || b->isLeaf()) // 遇到一方叶节点
        return a->isLeaf() && b->isLeaf() && (a->ch == b->ch);

    return areEqual(a->zero, b->zero) && areEqual(a->one, b->one);
}

/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Write your own student tests. */

STUDENT_TEST("test one node with empty tree") {
    EncodingTreeNode* root = new EncodingTreeNode('R');
    EXPECT(!areEqual(root, nullptr));

    deallocateTree(root);
}

STUDENT_TEST("test two nodes") {
    EncodingTreeNode* root1 = new EncodingTreeNode('R');
    EncodingTreeNode* root2 = new EncodingTreeNode('S');
    EncodingTreeNode* root3 = new EncodingTreeNode('R');
    EXPECT(!areEqual(root1, root2));
    EXPECT(areEqual(root1, root3));

    deallocateTree(root1);
    deallocateTree(root2);
    deallocateTree(root3);
}

STUDENT_TEST("test one node with example tree") {
    EncodingTreeNode* root1 = new EncodingTreeNode('R');
    EncodingTreeNode* root2 = createExampleTree();
    EXPECT(!areEqual(root1, root2));

    deallocateTree(root1);
    deallocateTree(root2);
}

STUDENT_TEST("test two example trees") {
    EncodingTreeNode* root1 = createExampleTree();
    EncodingTreeNode* root2 = createExampleTree();
    EXPECT(areEqual(root1, root2));

    deallocateTree(root1);
    deallocateTree(root2);
}

STUDENT_TEST("test tree with its subtrees") {
    EncodingTreeNode* root = createExampleTree();
    EXPECT(!areEqual(root, root->zero));
    EXPECT(!areEqual(root, root->one));

    deallocateTree(root);
}
```
测试结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721641323088-158b55e0-e654-4af8-92e3-08c6ee0a4c36.png#averageHue=%23c0ebae&clientId=u8e57e067-6ecc-4&from=paste&height=594&id=u25b96115&originHeight=891&originWidth=1020&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=91493&status=done&style=none&taskId=uf540531b-c6e0-4a57-974a-4bed8f9ec80&title=&width=680)
可以看出我们提供的测试都能通过。
# Huffman Coding
## 介绍
在完成了上一部分的工作之后，我们终于可以开始进行端到端的数据压缩和解压缩了。这一部分我们的任务就是完成完整的 compress 和 decompress。
首先我们对文件目录有一个了解：

- treenode.h 定义了 EncodingTreeNode。
- bits.h 定义了位：Bit 的数据结构，可以用整型数 0 和 1 做转型。同时也定义 EncodedData，来保存哈夫曼编码后的文件。
- bits.cpp 中我们提供了向流中读写位的方法，用到了位运算。
- main.cpp 包含了交互界面和端到端的文件压缩和解压缩接口。
- huffman.cpp 是具体实现文件压缩和解压缩的文件。
## 任务：压缩和解压缩
最后的一项任务是将之前的工作串起来，完成文件的压缩和解压缩。函数原型如下：
`string decompress(EncodedData& data);`
`EncodedData compress(string messageText);`
给出我们的实现，注意哈夫曼树在建立用完后要及时释放空间。
```cpp
string decompress(EncodedData& data) {
    EncodingTreeNode* root = unflattenTree(data.treeShape, data.treeLeaves);
    string re = decodeText(root, data.messageBits);
    deallocateTree(root);

    return re;
}

EncodedData compress(string messageText) {
    /* TODO: Implement this function. */
    Queue<Bit> treeShape;
    Queue<char> charSet;
    EncodingTreeNode* root = buildHuffmanTree(messageText);
    getShape(root, treeShape);
    getCharSet(root, charSet);
    Queue<Bit> encoded = encodeText(root, messageText);

    deallocateTree(root);
    return {treeShape, charSet, encoded};
}
```
测试如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721643215666-8c5888b1-43b8-4fb7-bb24-1d9ca39f8b9a.png#averageHue=%23e0dfdc&clientId=u8e57e067-6ecc-4&from=paste&height=503&id=uc030af33&originHeight=754&originWidth=986&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=74044&status=done&style=none&taskId=u9ae14be1-aff8-485d-addf-90f24dd7551&title=&width=657.3333333333334)
测试全部通过！
## 文件加解码
现在我们实现了压缩和解压缩，可以来解密 res 路径下的文件了。
在 res 路径下，存在 mystery.jpg.huf 文件，是被哈夫曼编码过的文件。我们将其解压缩，就可以得到原来的图片。
最后得到图片名为 unhuf.mystery.jpg：
![unhuf.mystery.jpg](https://cdn.nlark.com/yuque/0/2024/jpeg/43291115/1721643709606-2b7d480a-1c09-4580-a07e-8af80ce932f0.jpeg#averageHue=%23907c6c&clientId=u8e57e067-6ecc-4&from=ui&id=u8f703d48&originHeight=419&originWidth=368&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=151656&status=done&style=none&taskId=ua22d9164-ffc0-42e0-b120-6e8c7252a33&title=)
最后来测试加密功能和解密功能是否互逆，对 excellent.wav 文件做编码然后再解码。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721643994845-9e2e6ae3-0182-48b8-9f66-3ea368422c0e.png#averageHue=%23fbfaf9&clientId=u8e57e067-6ecc-4&from=paste&height=450&id=uc55ceab3&originHeight=675&originWidth=802&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=45955&status=done&style=none&taskId=ufd7fb19a-7444-4855-993f-a3eb11a8752&title=&width=534.6666666666666)
可以看出，经过一次哈夫曼编码译码之后，文件完整性仍然未被破坏，播放仍然可以听到一声 "excellent"。
另外值得一提的是，对文本文件 dream.txt 的编码，可以将其由 9 KB 压缩到 5 KB。对于图片 jpg 文件而言，压缩性能就没有那么好了。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1721644224568-d12e4787-679f-4736-a2dc-ca0827a60728.png#averageHue=%23fbfaf8&clientId=u8e57e067-6ecc-4&from=paste&height=385&id=uf256bb43&originHeight=577&originWidth=751&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=41845&status=done&style=none&taskId=u79fa3876-65fe-4b97-b531-9cae39d6e0e&title=&width=500.6666666666667)
