第三次作业的主题内容是递归。2022 Summer 将回溯也包含了进来，但是为了条理清晰我采取了 2022 Spring 的做法，将回溯放到 Assignment4 文档中去。至于作业的内容还是 Summer 的版本。
# Balanced Operators
## 问题引入
根据 C++ 的语法，存在三种括号运算符，即圆方花括号。凡是良构的程序，括号都必须是严格匹配的，否则编译器会直接报错。作业第一部分就是使用递归进行括号匹配检查。
例如：
`int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }`
经过筛选需要匹配的括号语句就是：
`(){([])(())}`
## 匹配规则
匹配失败的情况大致分为三种：

1. 右括号缺失，未完全闭合，例如 ( ( [ a ] )
2. 左右括号顺序颠倒，例如 3 ) (
3. 不同括号嵌合结构错误，例如 { ( x } )
## 任务一：提取括号
首先我们要将语句中的括号全部提取出来，以下是函数原型：
`string operatorsFrom(string ops);`
不同于以往的迭代式提取字符，我们使用递归式处理字符串，步骤如下：

1. 处理第一个字符，判断是否为括号；
2. 递归调用处理剩下的字符串并保存到结果中；
3. 拼接多级返回结果到最终字符串。

得到以下实现代码：
```cpp
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
```
测试通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720542288891-648a3b0c-d056-4ff2-a01c-f261c09f36e7.png#averageHue=%23f5f4f0&clientId=ub77e3cdc-bec3-4&from=paste&height=200&id=u49c4bba0&originHeight=300&originWidth=1778&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=62259&status=done&style=none&taskId=u2e906920-7d9b-45f7-b407-06b648474c8&title=&width=1185.3333333333333)
## 任务二：匹配括号
将字符串的所有括号提取出来后，剩下的就是匹配了，同样是递归式匹配。
函数原型：
`bool operatorsAreMatched(string ops);`
给出一点提示，可以按照这样的匹配规则：
在以下条件下，字符串括号是匹配的：

- 字符串为空；
- 字符串中存在 `()`、`[]`、`{}` 的字串，而且将他们去掉并不影响剩下字符串的匹配。

例如，`[(){}]`是匹配的，我们在检测时的调用顺序如下：

1. operatorAreMatched("[(){}]") -> 存在 `()`，将其去除，检查剩下的字符串是否匹配；
2. operatorAreMatched("[{}]") -> 存在 `{}`，将其去除，检查剩下的字符串是否匹配；
3. operatorAreMatched("[]") -> 存在 `[]`，将其去除，检查剩下的字符串是否匹配；
4. operatorAreMatched("") -> 空字符串，必然匹配，直接返回 true。

最后可以得到以下代码：
```cpp
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
```
测试通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720591750799-6ad0b691-6067-43d6-8adb-f5a59616d584.png#averageHue=%23f5f4f1&clientId=u8b926f7b-bf31-4&from=paste&height=480&id=u77b90e57&originHeight=720&originWidth=1965&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=144602&status=done&style=none&taskId=u6e49f6fb-35fe-4e01-ba5a-2976bc2331f&title=&width=1310)
# Sierpinski
## 介绍
递归可以用来做分形，其中最知名的的是 sierpinski 三角。其原理是通过自相似的图案生成的图，如下递归：

- 零阶三角是整个大三角；
- n 阶三角是由 3 个 n - 1 阶边长为其一半的三角组成的。

如图所示：
![sierpinski.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720592161386-0c8a567a-dd5a-4114-9077-7595c33c5921.png#averageHue=%23c5c5c5&clientId=u8b926f7b-bf31-4&from=ui&id=u111b358d&originHeight=234&originWidth=1330&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=155162&status=done&style=none&taskId=ubb398870-3f83-4260-9712-4803e87a765&title=)
更高阶的三角中，看起来我们好像即画了黑三角也有白三角，但是我们只需要画黑三角，白色只是空位而已。

## 任务：画三角
这一部分的作业内容比较短，而且给出了将三角涂黑的函数，不需要自己实现。
`void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three);`
那么剩下的任务就是画出三角形，原型如下：
`int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order);`
函数读取三角形的三个顶点以及阶数，返回已经画出的黑三角数目。尽管用一个简单的数学公式计算绘制的三角数，我们需要用递归来验证。
```cpp
/*
 * 特殊重载，这里将加号运算符重载为取中点
 */
GPoint operator+(const GPoint& p1,const GPoint& p2) {
    return {(p1.x + p2.x) / 2, (p1.y + p2.y) / 2};
}

/*
 * 所有的高阶三角，最后都会变成低阶三角、零阶三角
 * 在零阶三角时画出图案
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    /* TODO: Implement this function. */
    if(order == 0) {
        fillBlackTriangle(window,one,two,three);
        return 1;
    } else {
        int part1 = drawSierpinskiTriangle(window, one, one + two, one + three, order - 1);
        int part2 = drawSierpinskiTriangle(window, one + two, two, two + three, order - 1);
        int part3 = drawSierpinskiTriangle(window, one + three, two + three, three, order - 1);
        return part1 + part2 + part3;
    }
}

```
最终结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720596237669-90bd6b8b-2c94-40a4-a8cb-e6dbda2da03a.png#averageHue=%23e7e7e7&clientId=u8b926f7b-bf31-4&from=paste&height=582&id=u17b5954a&originHeight=873&originWidth=990&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=118971&status=done&style=none&taskId=uab039aaa-d964-4375-bfbc-630aea8a5a0&title=&width=660)
可以得到正确结果，总三角个数也是正确的。
# Merge
## 二路归并
在计算机科学领域，二路归并是非常常见的操作：将两个有序序列合并成一个更大的有序序列。归并也是许多排序任务的主体步骤。
## 任务一：实现迭代版归并
第一个任务是实现迭代版的二路归并，函数原型：
`Queue<int> binaryMerge(Queue<int> a, Queue<int> b);`
本任务有一些要求：

- 队列元素排升序，合并后的结果也排升序；
- 形参为值传递，可以随便修改，没有最终状态要求；
- 两队的长度不一定相等，且可能为空；
- 合并没有去重要求；
- 将任务分解：选择需要处理的下一个元素，之后处理，降低代码重复率；

不过二路归并其实在学习归并排序时已经非常熟了，实现比较简单：
```cpp
/*
 * 非常熟悉的归并排序基本步骤，但是不能随机存取
 * 所以更像合并链表
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    /* TODO: Implement this function. */
    while(!a.isEmpty() && !b.isEmpty()) {
        if(a.peek() <= b.peek()) {
            result.enqueue(a.dequeue());
        } else {
            result.enqueue(b.dequeue());
        }
    }
    while(!a.isEmpty())
        result.enqueue(a.dequeue());
    while(!b.isEmpty())
        result.enqueue(b.dequeue());

    return result;
}

```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720598703088-7f3e4ac3-271a-48d5-ab79-86b51bd77f89.png#averageHue=%23cecdc9&clientId=u8b926f7b-bf31-4&from=paste&height=36&id=u303b971d&originHeight=54&originWidth=749&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=5454&status=done&style=none&taskId=ue4dcb1de-c27e-43d5-8955-475f55bcd90&title=&width=499.3333333333333)
测试通过。
## 任务二：强制有序检测
上一个任务我们实现的二路归并要求输入必须有序，但是一般而言，输入不会自己检查，需要我们强制检查有序性。所以第二个任务就是给 binaryMerge 加上有序性检测。
给出两种方法：

1. 直接的方法是写一个检测是否有序的函数，然后分别对两个输入序列测试；
2. 另一种方法比较麻烦，是边归并边检查，不需要辅助函数。

这里我们两种都写，但是 binaryMerge 中方法使用第二种。
```cpp
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

```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720601022175-25ca3193-5199-4557-bca8-c60faff2dd49.png#averageHue=%23f4f2f0&clientId=u8b926f7b-bf31-4&from=paste&height=335&id=ue845d193&originHeight=502&originWidth=1976&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=109944&status=done&style=none&taskId=uef5d9913-1e9a-41a7-94ae-af579dcec11&title=&width=1317.3333333333333)
测试是通过的。
## 任务三：时间复杂度分析
通过 TIME_OPERATION，我们可以测量函数的执行用时。例如以下代码：
```cpp
int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
```
我们将程序规模定为两队列的大小之和。
通过数据结构课的学习，我们可以知道，执行一次二路归并的复杂度为 O(min(a,b))，现在两队大小控制一致，那么就是线性复杂度。现在我们每次将队列的大小翻一番，测试是否服从这个规律。
我们设定初始大小为 1000000。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720601849124-808311c2-87e2-4f8e-8ed7-275fdf10eec1.png#averageHue=%23bfe6a8&clientId=u8b926f7b-bf31-4&from=paste&height=172&id=u6cc8a986&originHeight=258&originWidth=1010&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=38748&status=done&style=none&taskId=uab598d83-ebc3-48e4-b3c5-6549d685513&title=&width=673.3333333333334)
可以看出，用时上和队列大小和基本成正比，可以认为是线性复杂度。
## 任务四：简单多路归并
二路归并接受两个有序序列，相应地，多路归并就是多个序列合并成一个有序序列。
给出一个简单的多路归并函数，不断遍历输入队列，反复调用二路归并来逐对做归并。
```cpp
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
```
经过测试，对一些边界情况的处理有效：
```cpp
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
```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720603450551-b59da53d-56e4-4b00-b357-1cfeef53a5ad.png#averageHue=%23a69c44&clientId=u8b926f7b-bf31-4&from=paste&height=31&id=ubc83611c&originHeight=46&originWidth=541&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=3894&status=done&style=none&taskId=u3b805c59-d081-4e3b-8b5f-6acb19bd25f&title=&width=360.6666666666667)
我们知道多路归并的参数除了队列总元素数目 n，还有归并路数 k，现在测试简单多路归并的时间复杂度。按以下思路：

1. 固定 n，测试 5 组不同路数的 k；
2. 固定 k，测试 5 组不同大小的 n。

注意一点，为了尽量不让一路出现空队，要求 k <= n。
我使用的测试代码如下：
```cpp
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

```
测试结果如下：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720604216188-f1ec38aa-b8de-42d3-b74c-fc335654f350.png#averageHue=%23bfe7a8&clientId=u8b926f7b-bf31-4&from=paste&height=187&id=ua3f9188b&originHeight=280&originWidth=1025&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=60124&status=done&style=none&taskId=uab80398b-0c0a-4aec-b985-1c19de2c3ae&title=&width=683.3333333333334)
似乎和 n 以及 k 都成正比。也就是 T = m * n，也满足 T = p * k。那么我们可以猜想时间复杂度是 O(n * k)。

## 任务五：分治多路归并
### 效率问题
以上的简单多路归并中，由于每次归并的队列越来越大，效率并不高。
现在让我们参考归并排序中的归并算法，重新考虑多路归并算法，也就是分治法下的多路归并。
函数原型：
`Queue<int> recMultiMerge(Vector<Queue<int>>& all);`
### 分治步骤
分治多路归并使用了递归，按如下步骤：

1. 将 k 路序列分为两半，左半有 k / 2 个序列（取整），剩下的归入右半，可通过 Vector 的 subList 处理；
2. 递归调用 recMultiMerge 处理左半边序列来生成有序序列，之后对右半边做同样处理；
3. 之后使用二路归并将左右半边得到的有序序列合并，成一个最后有序序列返回。

我们不去深究递归调用中有序序列是怎么产生的，只需要看下图：
![merge.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720606601715-b190d5a7-1b0b-4684-98cf-22ee691b558c.png#averageHue=%23f9f9f9&clientId=u8b926f7b-bf31-4&from=ui&id=u04b7ea7b&originHeight=459&originWidth=288&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=36315&status=done&style=none&taskId=udcfe1a96-52d2-468c-892b-842b8867271&title=)
上图中，最初一共有 7 路需要归并。第一次划分成左 3 右 4 序列，经过若干次递归调用，得到了两个有序的序列，最后调用一次二路归并即可完成。
现在我们给出代码：
```cpp
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
```
测试都能通过。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720617765252-8f860bbf-aed1-4802-8c3f-4d7f19baaefb.png#averageHue=%23bfe6a7&clientId=u2b72934b-df2e-4&from=paste&height=428&id=uef2f961a&originHeight=642&originWidth=1036&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=108751&status=done&style=none&taskId=u97a24417-1b7c-4018-9f20-49895a19507&title=&width=690.6666666666666)
### 复杂度分析
分治版多路归并，每次将整个归并范围划分为两份，直到一边为空。划分完后各自两两归并，每次二路归并时间复杂度为 O(n)，总共划分 logk 次，所以总时间复杂度为 O(n * logk) 级。
换个角度想，如果我们将每一路的元素都只留下一个，那么这个多路归并就成了实际上的归并排序。此时 k = n，时间复杂度就是 O(n * logn)。
给出测试代码：
```cpp
// 注意 k 始终不大于 n
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
    int k = n / 10000;
    for(int i = 0;i < 5;++i,k *= 4) {
        Queue<int> input = createSequence(n);
        Vector<Queue<int>> all(k);
        distribute(input, all);
        TIME_OPERATION(input.size(), recMultiMerge(all));
    }
}
```
得到结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720618893766-fdce2bb5-6ced-4e09-a823-e965bafbe3a4.png#averageHue=%23bfe7a8&clientId=u2b72934b-df2e-4&from=paste&height=181&id=u01c5173e&originHeight=272&originWidth=1017&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=59266&status=done&style=none&taskId=u8d262014-7e4c-455b-85b1-274fec08471&title=&width=678)
可以看出，确实与 n 成正比，但是与 k 的关系不太明显，对数关系确实比较难观察。修改一下：
```cpp
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

```
再次测试：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720619323819-d07c7b58-4fce-4a4c-afdc-1750b2aea1fa.png#averageHue=%23bfe7a8&clientId=u2b72934b-df2e-4&from=paste&height=93&id=u13c86799&originHeight=140&originWidth=997&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=29607&status=done&style=none&taskId=u999c668a-2b32-4ba4-a553-8b8a0ba2bd9&title=&width=664.6666666666666)
这次就是比较清楚了，大概是 2、4、6、8、10 的比例关系，确实与 xk 成正比。
