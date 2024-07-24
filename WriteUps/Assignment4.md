第四次作业的内容同样是递归，但是属于递归回溯，而且用到的方法，适用场景与一般递归有所不同，所以单独开一篇。
# Boggle Score
## 介绍
Boggle 是一种在棋盘上根据给出的字母，按一定顺序遍历组成单词，组成的词越多，得分越高。
![boggle.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720680216955-a4884b55-cb7b-4735-a93f-3ce40b5e89a3.png#averageHue=%23e2e2e2&clientId=u28ff05c5-9bc5-4&from=ui&id=u58e28970&originHeight=226&originWidth=230&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=21421&status=done&style=none&taskId=u86f38a9a-0615-4963-a900-38d8c3f6e9c&title=)
例如以上路径，可以构成单词 peace，于是得分。
每次构词的规则具体如下：

- 单词必须至少为 4 格字母；
- 必须是给定词典中的单词；
- 构成单词的路径必须是在相邻的格子之间，且路径不成环；
- 对于每个格子，它的邻格数量在 3 ~ 8 个之间；
- 每个单词最多计分一次。

构成单词的得分数由词长度决定，4 字母得一分、5 字母得两分、6 字母得三分依次类推。
## 任务：计算最多可得分
作业第一部分的任务就是，通过回溯递归的方法算出棋盘上最多可得分数。函数原型如下：
`int scoreBoard(Grid<char>& board, Lexicon& lex);`
其中参数 lex 是一个词典，属于斯坦福库提供的数据结构，可以用它来快速查询是否存在匹配单词，以及是否存在某前缀。
函数递归地搜索棋盘，凑齐单词获得分数。需要考虑两个问题：

- 回溯：棋盘搜索时递归回溯的理想应用。大致的思想是从 16 格子中每一个开始都搜索一次，递归地探索下一格，并记录路径；
- 标记：为了避免重复访问同一格，我们需要记录每一格是否访问过，这一点和迷宫求解比较类似。
### 积分函数
首先我们从辅助函数开始，传入一个单词，然后计算得分数：
`int points(string str);`
注意这里没有传入词典，所以不需要进行检查是否为有效单词，只需要计分即可。
```cpp
/*
 * 小于四字母的不得分
 */
int points(string str) {
    /* TODO: Implement this function. */
    if(str.length() < 4)
        return 0;
    return str.length() - 3;
}
```
### 试探位置
看起来和迷宫求解很像，但是这个棋盘最多可以有 8 个邻格，所以在试探上要稍微复杂一些。
```cpp
/*
 * 读取棋盘信息，返回试探位置是否合法
 */
bool isInBoard(const Grid<char>& board,GridLocation dst) {
    return board.inBounds(dst);
    // return (dst.row >= 0 && dst.row < board.numRows()) && (dst.col >= 0 && dst.col < board.numCols());
}

/*
 * 重载加法运算符
 */
GridLocation operator+(GridLocation p1, GridLocation p2) {
    return {p1.row + p2.row, p1.col + p2.col};
}


/*
 * 根据当前位置，获取可能的下一格位置数组，不考虑是否已走过
 */
Vector<GridLocation> getNext(const Grid<char>& board, GridLocation start) {
    static const Vector<GridLocation>neighbors = {{-1, -1},{-1, 0},{-1, 1},{0, 1},{1, 1},{1, 0},{1, -1},{0, -1}};
    Vector<GridLocation> destinations;
    for(GridLocation next: neighbors) {
        GridLocation dst = start + next;
        if(isInBoard(board,dst))
            destinations.add(dst);    
    }
    return destinations;
}
```
有关下一格位置的试探，这里我们不考虑是否重复走过，减少参数传递。
将位置判断放到递归函数中。
### 剪枝
我们不需要做穷举搜索。例如词典中没有以 zx 开头的单词，所以当遇到路径构成的词为 zx 时，我们发现词典并不存在这样前缀的单词，那么之后无论怎么搜索都不可能再构成有效词了，可以直接返回 0 分。这样就省去了 zxa、zxakm 之类冗长又无意义的搜索，大大节省时间。
### 避免重复积分
我们获得的总分是从每一格开始递归的得分之和，需要考虑一种情况，也就是词典中重复积分。由于词典没有删除的方法，所以我们可以将每次递归结果设定为一个集合，这样不同轮次的统计结果取并集，最后在 scoreBoard 中取并集，再统一计分。
也即是将统计与积分两个阶段分开，避免重复计算的情况。
### 逐格递归
我们将负责递归的主体部分放到 scoreCell 函数中去，让它一边递归一边统计单词，最后将结果在 scoreBoard 中统一计算得分。代码如下：
```cpp
/*
 * 从单格开始的递归搜索，统计已经积分的单词，避免重复积分
 */
void scoreCell(const Grid<char>& board, GridLocation start, std::string& str, Lexicon& lex, Set<std::string>& words) {
    static Grid<bool> used = {{false, false, false, false},
                              {false, false, false, false},
                              {false, false, false, false},
                              {false, false, false, false}};
    Vector<GridLocation> nextPos = getNext(board,start);
    if(!lex.containsPrefix(str))
        return ;
    if(lex.contains(str))
        words.add(str);
    str.push_back(board[start]);
    used[start] = true;
    for(GridLocation next: nextPos) {
        if(!used[next]) // 未使用过
            scoreCell(board,next,str,lex,words);
    }
    str.pop_back();
    used[start] = false;
}


/*
 * 主体函数，逐格递归，统计得到的单词，最后一起算分。
 */
int scoreBoard(Grid<char>& board, Lexicon& lex) {
    
    Set<std::string> words;

    for(GridLocation pos: board.locations()) {
        std::string letters = "";
        scoreCell(board,pos,letters,lex,words);
    }

    int sum = 0;
    for(std::string word: words) {
        sum += points(word);
    }
    return sum;
}
```
测试结果为：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720689882397-8970af68-1a21-4b8d-ae1e-f81f2038bd6d.png#averageHue=%23374f83&clientId=uaf207f3b-e760-4&from=paste&height=111&id=u0cd83ef9&originHeight=166&originWidth=1016&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=25378&status=done&style=none&taskId=u4339d478-5618-440b-9e5b-1ac08f5c20c&title=&width=677.3333333333334)
出现了分数不匹配！那么应该是有单词遗漏了，我们来排查一下：
```cpp
STUDENT_TEST("check one missing") {
    Set<std::string> inspected = {"ALEF", "ALEFS", "ALEPH", "EFTS", "ELAN", "ELANS", "ETHS", "FANS", "FASH", "FAST", "FASTS", "FETS", "FLAN", "FLANS", "FLASH", "HOTS", "LASH", "LASS", "LAST", "LASTS", "LEFT", "LEFTS", "LEPT", "LETS", "PELF", "PELFS", "PELT", "PETS", "PETTO", "SAFE", "SALE", "SALEP", "SALS", "SALT", "SANS", "SASH", "SHOT", "SHOTS", "SHOTT", "SHOTTS", "SHTETL", "SLASH", "SLEPT", "SNASH", "STELA", "STEP", "STET", "TELA", "TELS", "TETH", "TETHS", "TETS", "TOTS"};
    Set<std::string> all = {"TOTS","HOTS","TELS","TELA","PETTO","PETS","PELF","PELFS","PELT","SHOT","SHOTT","SHOTTS","SHOTS","SHTETL","SNASH","SASH","SAFE","SANS","SALS","SALE","SALEP","SALT","STEP","STELA","STET","SLEPT","SLASH","FETS","FELT","FAST","FASTS","FASH","FANS","FLASH","FLAN","FLANS","ETHS","EFTS","ELAN","ELANS","ALEPH","ALEF","ALEFS","LETS","LEPT","LEFT","LEFTS","LAST","LASTS","LASH","LASS","TETH","TETHS","TETS"};
    EXPECT_EQUAL(all.size(),54);
    std::cout << "The missing one is " << all.difference(inspected) << std::endl;
    // Lexicon lex = sharedLexicon();
    // EXPECT(lex.containsPrefix(""));
}
```
我们可以用斯坦福自己实现的 Boggle 来作为验证，以应得 76 分的棋盘为例，发现集合中差的元素是 FELT，也就是棋盘最右下角的四个元素组成的单词。
这样我大概直到原因了：由于路径是 F-E-L-T，所以到达 T 时，T 还没有被算进来。但是可以发现，在达到 T 之后，没有下一格可去了，所以递归在此返回，没有将 FELT 包含进来。看来是边界条件没有考虑的问题。
因此我给加上一个边界条件，若下一步无处可去，那么原地判断是否为单词，若是则单词集合加进去。
修改后的代码如下：
```cpp
/*
 * 从单格开始的递归搜索，统计已经积分的单词，避免重复积分
 */
void scoreCell(const Grid<char>& board, GridLocation start, std::string& str, Lexicon& lex, Set<std::string>& words) {
    static Grid<bool> used = {{false, false, false, false},
                              {false, false, false, false},
                              {false, false, false, false},
                              {false, false, false, false}};
    Vector<GridLocation> nextPos = getNext(board,start);
    if(!lex.containsPrefix(str))
        return ;
    if(lex.contains(str) && str.length() >= 4)
        words.add(str);

    str.push_back(board[start]);
    used[start] = true;

    bool hasWay = false;
    for(GridLocation next: nextPos) {
        if(!used[next]) {
            scoreCell(board,next,str,lex,words);
            hasWay = true;
        }
    }
    if(!hasWay && lex.contains(str))
        words.add(str);
    str.pop_back();
    used[start] = false;
}
```
再次测试：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720690712369-ea7fd351-fa4d-4cd2-9a48-d14d3750c721.png#averageHue=%23e0dedc&clientId=uaf207f3b-e760-4&from=paste&height=407&id=u4e2d4021&originHeight=611&originWidth=964&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=64677&status=done&style=none&taskId=u4944965c-73ff-4f5d-afc2-43f5f67158a&title=&width=642.6666666666666)
这一次完全通过！
需要说明的是：经过我的检查，空字符也可以作前缀。
# Voting Power
## 集体投票
在一项选举中，如果存在 n 个投票集体，而候选人只有两个，那么投票结果就存在 2n 种可能性。衡量一个区块重要性或投票“权力”的一个指标，是该区块拥有我们所说的“关键投票”的选举结果的数量。
关键票以及摇摆票是改变选举结果的投票。即使一个集体有多个选票，但是对于“关键投票”而言，给定集体的所有投票都指向一个特定的对象。
一个集体拥有关键投票权的结果计数用于计算 Banzhaf power 指数，这是一个衡量给定集体在投票系统中的投票权的指标。
## Banzhaf Power 指数
我们称在选举中以相同方式投票的一个集体为一个联盟。对于给定的投票集体 B，如果它加入某个联盟则该联盟获胜；否则该联盟失败，那么此时 B 的一票就是关键票。也就是说，B 提供了能够影响选举的关键选票。
例如在选举系统下存在三个集团：

| 集团 | 存票 |
| --- | --- |
| 狮子 | 50 |
| 老虎 | 49 |
| 熊 | 1 |

那么在者三个集团之间，就可以结成以下联盟：

- 三个集团的联盟
- 任意两团结盟，剩下一团
- 不结盟

如果选举要求严格多数，那么整个系统就有 100 张票，获胜至少需要 51 票。
以上的组合中，总共可以产生三种联盟获胜的情况：

- 三团联盟；
- 狮子 + 老虎联盟；
- 狮子 + 熊联盟。

而这三种情况下，三个集团的作用如下：

- 狮子在三种结盟情形下都是关键票；
- 老虎在狮虎联盟中持有关键票；
- 熊在狮熊联盟中持有关键票。

一旦关键票离开联盟，那么剩下的集团成员无法获胜。例如狮子若在三团联盟中离开，那么剩下的集团无法获胜，所以狮子是关键票，而老虎和熊不是。
那么我们可以统计，每个集团持有的关键篇数量：

| 集团 | 关键票数 |
| --- | --- |
| 狮子 | 3 |
| 老虎 | 1 |
| 熊 | 1 |

Banzhaf Power 指数可以表示一个集团的投票表决权，计算方式是集团关键票数除以总关键票数。也即是：

| 集团 | Banzhaf Power 指数 |
| --- | --- |
| 狮子 | 60% |
| 老虎 | 20% |
| 熊 | 20% |

尽管熊的票数远少于老虎，但是就关键票数而言，两者持有关键票数相同。相对于老虎而言，狮子的票数只多一张，但是这微弱的差距让它多了两张关键票。
## 任务：计算 Banzhaf 指数
现在我们要实现的就是计算集团的 Banzhaf 指数，原型如下：
`Vector<int> computePowerIndexes(Vector<int>& blocks);`
函数读取各个集团的票数，返回各自的关键票指数。例如输入 {50,49,1}，输出 {60,20,20}。
代码中，只需要计算某个集团的关键票数，然后对每个集团重复即可。具体步骤如下：

- 将目标集团放到一边，然后对剩下的集团递归地求子集。只要结成联盟，不需要存储集团的集合，只需要计票；
- 联盟建立，票数累加之后，就需要考虑目标集团的选票影响了。是否满足目标集团加入则胜利，否则失败。若满足条件则该集团相对于联盟是关键票；
- 对每个集团都进行递归搜索来计数关键票。
### 剪枝
为了缩小搜索规模，我们同样需要剪枝：

- 一旦联盟不再依赖于目标集团的票，搜索就可以停止了。因为再进来集团，目标的票就更无足轻重了。
- 注意减少拷贝的使用，因为求子集需要递归量比较大，耗时可能比较久。
- 不要试图求所有联盟组合情况然后穷举求和，这样对时间和空间开销都非常大。

另外，如果存在票数相同的集团，那么他们所拥有的关键票数一定是相同的，这一点不难理解。
### 代码实现
下面贴出代码：
```cpp
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

```
本问题比较复杂，在代码中给出注释来解释。
测试结果：
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720702401781-63cd6a9f-3832-4083-ab1b-8fa92d7eb504.png#averageHue=%23e3e2e0&clientId=uaf207f3b-e760-4&from=paste&height=289&id=uab3c944e&originHeight=434&originWidth=1035&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=47872&status=done&style=none&taskId=ufce06da7-b38e-419b-83a4-2abaaec104a&title=&width=690)
测试的时间可能比较长，例如 EU 用例，大概花了十几秒才通过。
# 参考文章

- [https://www.acwing.com/blog/content/27637/](https://www.acwing.com/blog/content/27637/)
