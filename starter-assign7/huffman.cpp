#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "testing/SimpleTest.h"
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
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

/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if(treeShape.isEmpty())
        return nullptr;
    EncodingTreeNode* root = new EncodingTreeNode(nullptr, nullptr);
    Bit bit = treeShape.dequeue();
    if(bit == 0) // leaf node
        root->ch = treeLeaves.dequeue();
    else {
        root->zero = unflattenTree(treeShape, treeLeaves);
        root->one = unflattenTree(treeShape, treeLeaves);
    }

    return root;
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
string decompress(EncodedData& data) {
    EncodingTreeNode* root = unflattenTree(data.treeShape, data.treeLeaves);
    string re = decodeText(root, data.messageBits);
    deallocateTree(root);

    return re;
}


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


/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    Map<char,int> stat = charFreqStat(text);

    return buildHelper(stat);
}


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


/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Queue<Bit> re;
    Map<char,string> encodeTable;
    getEncodingTable(tree, encodeTable, "");
    encodeHelper(encodeTable, re, text);
    return re;
}

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


/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    getShape(tree, treeShape);
    getCharSet(tree, treeLeaves);
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
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

/* * * * * * Testing Helper Functions Below This Point * * * * * */

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

void deallocateTree(EncodingTreeNode* t) {
    if(t) {
        deallocateTree(t->zero);
        deallocateTree(t->one);
        delete t;
        t = nullptr;
    }
}

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

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman"
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
