第二次作业的主要内容是关于一些 ADT，或者说是斯坦福自己的 STL 库的使用。需要用到 Vector、Grid（二维序列）、Set、Map、Stack、Queue，除了 Grid 之外我们在 STL 库中比较熟悉的容器。会解决迷宫、搜索引擎等问题。
# Maze
## 概述
第一部分是我们在数据结构中非常常见的一类问题，即迷宫求解问题。在本课程中是连同 Stack 这种容器和 BFS 一同讲解的，所以作业中也要利用这样的方法。
根据课程提示，我们需要将求解的路径存储到 Stack 中去。res 路径下存在迷宫样例及其解答的 .txt 文件，我们的测试就是如此进行。
## 问题分析
迷宫求解的 BFS 解法是从起点开始呈放射状逐步往外探索，直到达到终点。也就是走一步、走两步、走三步逐次试探，直到第一个到达终点的路径。
这样我们可以将不同长度的路径视为循环的不同轮次，相同长度的各个路径用 Queue 存储，以确保路径延长的连贯性。
如果队首的路径到达了重点，那么可以结束寻找；否则从队首的路径中找到当前位置，继续寻找可以移动一格的位置，并将延长后的路径重新入队。
## 任务一：可行的下一步位置集合
我们将问题分解，首先我们针对迷宫构造和当前路位置，求出所有可以移动位置的集合。
函数原型为：
`Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur)`
既然要返回合法移动，那么我们首先要确定合法移动的条件;

1. 恰好移动一格；
2. 目的点在迷宫内；
3. 目的点不是墙壁。

为了方便四个方向的判断，我们给 GridLocation 这个类重载 `+` 运算符。由于斯坦福库不开源，我们不可能给 GridLocation 这个类添加友元的方法重载，所以只能是以一般函数的形式重载。
```cpp
/*
 * 判断当前位置是否合法，满足在迷宫内，而且可走
 */
bool validLocation(Grid<bool>& maze,GridLocation cur) {
    return (cur.row >= 0 && cur.row < maze.numRows()) && \
           (cur.col >= 0 && cur.col < maze.numCols()) && \
           (maze[cur.row][cur.col]); 
}

/*
 * 为方便四个方向的试探，重载一个 + 运算符
 */
GridLocation operator+(GridLocation l1,GridLocation l2) {
    return {l1.row + l2.row,l1.col + l2.col};
}
```
这样问题就很简单了，只需要在函数体内对四个方向做逐一判断即可。
```cpp
/*
 * 四个方向逐个判断即可
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    const static std::array<GridLocation,4> directions = { // 注意写法，解释见参考文章第二篇
        {
         {0,1},{1,0},{0,-1},{-1,0}
        }
    };
    for(GridLocation d:directions) {
        if(validLocation(maze,cur + d))
            neighbors.add(cur + d);
    }

    return neighbors;
}
```
由于斯坦福库里的 GridLocation 构造不是 constexpr 的，所以不能开启 constexpr 优化，不然还能提高性能。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719763734836-6c2fe55f-b775-4ddd-9a4e-13a1afd4533e.png#averageHue=%23aaa144&clientId=u54a3573d-929e-4&from=paste&height=94&id=u52c28b53&originHeight=141&originWidth=1033&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=20018&status=done&style=none&taskId=u608cc913-f0c8-4fe9-83a6-0f9d114c28b&title=&width=688.6666666666666)
测试通过！

## 任务二：验证路径可行性
下一步是验证路径栈在迷宫中是否能正确走到终点。
函数原型：
`void validatePath(Grid<bool>& maze, Stack<GridLocation> path)`
函数读取一个路径栈，判断根据栈的内容能否逐次还原到起点。需满足以下条件：

1. 自左上角的起点开始，到右下角终点结束；
2. 路径中每一个位置都是上一个位置的合法移动点，这个可以用上一个任务的合法集合判断；
3. 路径不能构成一个环，也就是同一条路径中相同的位置点不能反复出现，可以用一个位置集合判断是否重复。

你也许发现了，我们的函数原型中对于空间占用较大的路径栈使用了值传递，为什么呢？
原因是我们在判断路径合法时，由于栈不支持随机存取，我们只能依次出栈来判断，不能是常引用传递。如果是非常引用传递，那么必然会修改原路径栈，这也是我们不希望的。所以我们不得不选择值传递。
```cpp
/* 首先判断栈顶是否为终点，之后逐步判断位置的移动是否合法。
 * 注意起点的判断要放到循环开头。
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};
    GridLocation mazeEntry = {0, 0};
    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }

    GridLocation last = path.pop(); // 上一步所在位置
    Set<GridLocation> visited {last}; // 路径已经到达过的点

    while(!path.isEmpty()) {
        if(path.size() == 1 && path.peek() != mazeEntry)
            error("Path does not begin at maze entry");

        GridLocation temp = path.peek(); // 当前位置
        Set<GridLocation> valid = generateValidMoves(maze,last); // 得到上一步可能的位置
        if(valid.contains(temp) && !visited.contains(temp)) { // 若为合法的上一步位置
            visited.add(temp); // 添加到访问记录
            path.pop(); // 出栈
            last = temp; // 更新上一步位置
        } else if(!valid.contains(temp)){
            error("Unable to walk this way");
        } else if(visited.contains(temp)) {
            error("The path forms a loop, which is prohibited");
        } else{
            error("Something got wrong...");
        }
    }
}
```
测试通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719827262349-4825e134-93ab-4695-a49e-7910eff24127.png#averageHue=%23bfe6a8&clientId=ubd761326-5cd4-4&from=paste&height=183&id=ufd5b2a18&originHeight=274&originWidth=1022&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=43012&status=done&style=none&taskId=u0df3702f-0d74-4082-94fe-7047d4cb83b&title=&width=681.3333333333334)
## 任务三：迷宫求解
解决了上述两个分问题，我们最后来完整解决迷宫问题。
`Stack<GridLocation> solveMaze(Grid<bool>& maze)`
在 BFS 算法下，解决迷宫问题实际上就是一种最短路径问题，每一步的权值都是 1。与一般最短路径问题相比，迷宫问题有几点注意：

- 迷宫至少为两行两列，而且起点和重点都没有墙壁；
- 迷宫必须有解；
- 生成的路径不能成环，即走过的位置不能在路径栈中重复；
- solveMaze 中不需要调用 validatePath，但是可以在测试中验证路径合法性；
- 可以用绘图动画的方法显示程序运行的过程，辅助调试。

可以分为以下几步：

1. 建立填装路径栈的空队；
2. 将所有长度为 1 的路径栈入队，也就是只包含入口的路径栈；
3. 若队列不为空：
将队首路径栈出队；
	①若栈顶元素为终点，那么可以返回该路径栈。
	②否则判断当前路径的位置的可行下一步，对于每一个可行的下一步，做当前路径栈的拷贝，将路径延长并入队。

这里还有一个问题，如果不能将路径栈之前走过的位置点剔除掉，那么消耗的时间会大幅增加甚至陷入死循环。但是给每一个栈单独开一个特殊对应的位置点集合又太多了，而且无法一一对应。所以我们单开一个函数，利用路径栈的拷贝求出路径点的集合并返回。
```cpp
/*
 * 由于栈不可随机访问，在试探时给每个路径栈附带一个路径点集合不切实际，而且无法对应。
 * 因此我们根据栈做拷贝，另开一个函数返回路径点集合，用时间换空间。
 */
Set<GridLocation> visitedLocations(Stack<GridLocation> path) {
    Set<GridLocation> visited;
    while(!path.isEmpty()) {
        GridLocation temp = path.pop();
        visited.add(temp);
    }
    return visited;
}

/* 
 * 将前几部整合到一个函数中，绘图来辅助示意。
 * 返回最先到达的路径栈，也就是最短路径。
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;
    Queue<Stack<GridLocation>> bfs{{{0, 0}}}; // 初始状态
    GridLocation mazeExit = {maze.numRows() - 1, maze.numCols() - 1}; // 终点

    bool hasSolution = false;
    while(!bfs.isEmpty()) {
        path = bfs.dequeue();
        MazeGraphics::highlightPath(path,"green",10);
        if(!path.isEmpty() && path.peek() == mazeExit) {
            // path = front;
            hasSolution = true;
            break;
        }

        Set<GridLocation> nextLocations = generateValidMoves(maze, path.peek());
        Set<GridLocation> visited = visitedLocations(path);
        for(GridLocation next: nextLocations) {
            if(!visited.contains(next)) {
                Stack<GridLocation> extended = path;
                extended.push(next);
                bfs.enqueue(extended);
            }
        }
    }
    if(!hasSolution)
        error("The maze has no solution, please check it again");

    return path;
}
```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719835701066-dea6f42c-a6ca-48b6-8551-a3bc83a4ee59.png#averageHue=%23d7d5d3&clientId=ubd761326-5cd4-4&from=paste&height=451&id=u74f08a39&originHeight=677&originWidth=1364&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=127421&status=done&style=none&taskId=u0ebe530d-4929-4b85-badc-46fd416ba95&title=&width=909.3333333333334)
成功了！我们完成了代码实现迷宫求解。
最后我们利用 res 路径下的其他文件，将我们的求解结果和 soln 文件给出的答案比对，是否满足最短路径条件。
```cpp
STUDENT_TEST("solveMaze on file 17×37, and check if the result matches") {
    Grid<bool> maze;
    readMazeFile("res/17x37.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);
    Stack<GridLocation> fromFile;

    readSolutionFile("res/17x37.soln", fromFile);
    EXPECT_NO_ERROR(validatePath(maze,soln));
    EXPECT_EQUAL(fromFile,soln);
}

STUDENT_TEST("solveMaze on file 19×35, and check if the result matches") {
    Grid<bool> maze;
    readMazeFile("res/19x35.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);
    Stack<GridLocation> fromFile;

    readSolutionFile("res/19x35.soln", fromFile);
    EXPECT_NO_ERROR(validatePath(maze,soln));
    EXPECT_EQUAL(fromFile,soln);
}

STUDENT_TEST("solveMaze on file 25×33, and check if the result matches") {
    Grid<bool> maze;
    readMazeFile("res/25x33.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);
    Stack<GridLocation> fromFile;

    readSolutionFile("res/25x33.soln", fromFile);
    EXPECT_NO_ERROR(validatePath(maze,soln));
    EXPECT_EQUAL(fromFile,soln);
}

STUDENT_TEST("solveMaze on file 33×41, and check if the result is valid") {
    Grid<bool> maze;
    readMazeFile("res/33x41.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}
```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719838273800-79c876bc-eb6a-40c5-a9a6-c11824c49623.png#averageHue=%23d1d0cf&clientId=ubd761326-5cd4-4&from=paste&height=619&id=uf5826288&originHeight=929&originWidth=1031&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=68774&status=done&style=none&taskId=ub86ed5fb-61fb-41b3-b492-cf69d391bc5&title=&width=687.3333333333334)
结果显示我们的做法完全正确。
# Search Engine
## 概述
搜索引擎是信息时代我们每天都会用的东西，每个网页都有一个 URL，也就是统一资源定位符，来唯一标识网页和主体信息。
本作业中我们首先要了解网页数据的构成，使用恰当的数据结构，学习编写读取主体信息的程序。之后编程来实现单一查询网页信息检索，最后实现网页信息的多查询匹配检索。
## 理解网页数据
本作业中，我们通过课程网站数据库存储的网页来获取主体文本信息。数据库文件格式如下：

- 数据库中的每一行按以下结构成对分组：
	第一行为网页的 URL；
	第二行为网页的主体信息，不带任何换行符。
- 前两行构成了第一组，第三第四行为第二组，以此类推。

数据库文件样例在 res 路径下的 tiny.txt 和 website.txt。
## 使用反向索引
为了提高搜索效率，我们需要考虑使用何种数据结构来构建和存储数据。搜索引擎通常使用一种巧妙的排列方式，称为反向索引。在介绍反向索引之前，我们先来看看常见的索引方式——正向索引。
我们以查阅词典为例，正向索引将是从一个位置（页码）到该页面上出现的所有单词的映射。
而反向索引，顾名思义就是与根据页码翻书的过程相反。例如，如果你在 CS106B 教材的索引中查找关键词“互联网”，它列出了两个页码，18 和 821。“互联网”一词出现在第 18 页，再次出现在第 821页。因此，反向索引是从单词到位置的映射。
要构建反向索引，通常从正向索引形式的数据开始，然后处理转换为反向索引的过程。反向索引会需要一些时间，不过一旦完成，反向索引就能够实现快速查找查询匹配项。

## 任务一：字符串处理
首先我们同样从小问题着手，写一个字符串处理的函数。函数原型如下：
`string cleanToken(string token)`
这个函数读取一串在主体文本中出现的字符串，经过处理后返回最简洁的版本。在这个函数中我们需要剔除所有标点符号前后缀，并将字母全部转为小写。另外函数还会检查 token 是否仅包含非字母的字符，否则返回空串表示 token 被丢弃。
准确地说就是：

- 剔除字符串中前导和后缀标点，例如 <<section>> 中的大于号和小于号。但是 section's 中的引号不能去掉。可以用 ispunct 判断是否应该为标点。
- 确认 token 是否包含至少一个字母，可以用 isalpha 来判断字母。若一个字母都不存在，那么 token 应当被丢弃不进入索引之列，函数返回空字符串。
- 将 token 转为小写。

给出我的解法：
```cpp
/*
 * 本函数用来清除所有前后缀标点
 */
void trimPunct(string& token) {
    int start = 0;
    int end = token.length() - 1;
    while(start < token.length() && std::ispunct(token[start])) {
        ++start;
    }
    while(end >= start && std::ispunct(token[end])) {
        --end;
    }
    token = token.substr(start,end - start + 1);
}

/*
 * 判断是否至少包含一个字母，若不包含则将 token 置为空串
 */
void isValidToken(string& token) {

    for(char ch: token) {
        if(std::isalpha(ch)) {
            return ;
        }
    }
    token.clear();
}

/*
 * 依次调用前两个函数，之后原地转为小写即可
 */ 
string cleanToken(string token) {
    
    trimPunct(token);
    isValidToken(token);
    toLowerCaseInPlace(token);
    return token;
}
```
能够通过测试。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719915679251-871b678a-b375-4b5d-9879-548c3edb5dee.png#averageHue=%23e2e0de&clientId=ucfc12f34-f5bc-4&from=paste&height=143&id=ubc99e60f&originHeight=215&originWidth=990&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=24637&status=done&style=none&taskId=u1b78110a-45a0-4b04-902f-6b32646ec90&title=&width=660)
## 任务二：收集 Token
第二个辅助函数我们将从主体文本中统计 token，形成一个集合。
函数原型：
`Set<string> gatherTokens(string bodytext)`
函数形参是从网页上读取的主体信息字符串，函数要将这个字符串整理成不同 token 的集合并返回。
首先我们要将 bodytext 进行 token 化，也就是将字符串拆分为多个以空格为分隔符的单词。字符串的分割在 C++ 标准库中没有对应函数，但是斯坦福提供了 stringSplit 来专门完成这个任务。
之后我们调用上一个任务写好的 cleanToken 对每个 token 进行修整，之后将得到的 token 存入集合。
```cpp
/*
 * 首先将 bodytext 进行 token 化
 * 也就是将字符串拆分为多个以空格为分隔符的单词。
 * 之后我们调用上一个任务写好的 cleanToken 对每个 token 进行修整
 * 将得到的 token 存入集合返回。
 */
Set<string> gatherTokens(string bodytext)
{
    Set<string> tokens;
    Vector<string> words = stringSplit(bodytext," ");
    for(const std::string& str : words) {
        tokens.add(cleanToken(str));
    }
    tokens.remove("");
    return tokens;
}
```
要注意的是，我们经过 cleanToken 处理可能得到空字符串。虽然空串可以被集合统计进去，但不是有效的 token，所以需要在统计完后手动删除。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719938665909-f2c7b406-b11b-4d95-a634-8c9f55a42c2b.png#averageHue=%23a9a043&clientId=u22ce6433-d9af-4&from=paste&height=95&id=u8270e9b1&originHeight=142&originWidth=1022&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=17403&status=done&style=none&taskId=ue906f9ba-33c3-4106-b841-fb53e04a167&title=&width=681.3333333333334)
测试通过！
## 任务三：建立反向索引
我们需要一个函数来读取数据库文件里的内容，将其转换为反向索引的形式。函数原型如下：
`int buildIndex(string dbfile, Map<string, Set<string>>& index)`
解释一下：我们读取数据库文件名，以及 token 到包含 token 的网站主体的 URL 集合的反向索引。函数返回值就是数据库文件中匹配到的页面数。
首先是文件读取，我们可以参考 Maze 中的迷宫文件读取函数，将文件内容按行返回到 Vector 中。
对于每个页面，使用上一个任务解决的 gatherToken 来提取 tokens 集合。对于页面中的每个 token，可以通过将匹配项添加到反向索引中的条目，来记录与页面URL的匹配。
所以 index 映射表存储的就是 token 和包含 token 的网页 URL 集合的映射关系。
```cpp
/*
 * 读取数据库文件中所有行，先不做区分
 */
void readDataBase(const std::string& filename,Vector<string>& lines) {
    ifstream in;
    if(!openFile(in,filename)) {
        error("Cannot open file named: " + filename);
    }
    readEntireFile(in,lines);
}

/*
 * 本函数使用一个循环每次读取 url 和 token 里的一个 string
 * 提取其中的 token，根据 token 给对应 url 集合添加数据，由集合自动去重。
 */
void addEntries(const Vector<string>& url,const Vector<string>& token,Map<string,Set<string>>& index) {
    for(int i = 0;i < token.size();++i) {
        Set<string> temp = gatherTokens(token[i]);
        for(const std::string& tokens: temp) {
            index[tokens].add(url[i]);
        }
    }
}

/*
 * 先读入所有行，之后按奇偶分为 URL 行和 Token 行方便分开处理
 * 之后给 index 中添加记录
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    Vector<string> allLines;
    readDataBase(dbfile,allLines);
    Vector<string> urlLines,tokenLines;

    for(int i = 0;i < allLines.size();++i) {
        if(i % 2)
            tokenLines.add(allLines[i]);
        else
            urlLines.add(allLines[i]);
    }

    addEntries(urlLines,tokenLines,index);

    return urlLines.size();
}
```
测试通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719938759121-da2ae9a1-4201-47fa-beae-bbf5b1330606.png#averageHue=%23aaa044&clientId=u22ce6433-d9af-4&from=paste&height=32&id=u785e0099&originHeight=48&originWidth=909&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=6037&status=done&style=none&taskId=u9a98c3a8-5854-48c7-bddd-fb61c84960d&title=&width=606)
给出调试过程，可以发现我们确实建立了正确的反向索引。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719938709758-2a6f7358-600a-40c5-ad24-c2093a56d662.png#averageHue=%23faf9f7&clientId=u22ce6433-d9af-4&from=paste&height=261&id=u939f7b4b&originHeight=392&originWidth=1671&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=80598&status=done&style=none&taskId=u09b286be-3fe7-4871-903b-21886f7c2fd&title=&width=1114)
这一环节是本作业的核心，也是比较难想的一步，特别是在逐行分割的情况下如何将 URL 和 token 的关系重新链接起来，需要仔细考虑。
## 任务四：匹配查询
建立起了反向索引，下一步我们就要根据查询关键字来进行反向索引了。函数原型：
`Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)`
query 可以是单个查询词，也可以是复合的一串查询。分别表示单个单词；或者多个连续词的序列，其中每个词都可以用 `+` 或 `-` 来修饰。同样可以用 stringSplit 将 query 拆分为多个查询词。
匹配规则如下：

- 对于单个查询词，直接返回包含该词的所有 URL 集合；
- 查询词序列将被视为复合查询，将多个单查询词结果合成为一个结果；
- 考虑修饰符的情况时，匹配方式存在一些变化：
	①若不存在修饰符，则默认为多个查询词的并集；
	②若修饰符为 `+`，则结果为多个查询结果的交集；
	③若修饰符为 `-`，则结果为查询结果的差集，即不包含修饰符后的查询词
- 对于 query 的查询词，也需要使用 cleanToken 进行处理；
- 查询是忽略大小写的。

示例：

- quokka：匹配所有包含 quokka 的网页 URL；
- simple cheap：表示 simple OR cheap，返回的结果至少需要包含两个词中的一个；
- tasty +healthy：表示 tasty AND healthy，返回的结果必须同时包含两词；
- tasty -mushrooms：表示 tasty WITHOUT mushrooms，返回结果包含 tasty 但不包含 mushrooms；
- tasty -mushrooms simple +cheap：表示 tasty WITHOUT mushrooms OR simple AND cheap，返回 ((("tasty" without "mushrooms") or "simple") and "cheap")。

注意查询没有使用操作符，是简单地从左往右处理。
对于查询词还有以下要求：

- 每个查询语句必须是非空的，而且至少包含一个查询词；
- 若查询词带有修饰符，那么修饰符必须出现在第一个字符，单独的一个修饰符并不是查询词；
- 第一个查询词不带有修饰符；
- 可以认为所有查询词经过 clean 之后不会为空。

综上我们有如下代码：
```cpp
/*
 * 本函数对提取得到的单个查询词做处理
 * 注意不能服用 cleanToken，因为会将修饰符一并去掉
 * 若含有修饰符，则对后续的子串 cleanToken
 * 否则直接 cleanToken
 */
void trimQuery(string& query) {
    if(query[0] == '+' || query[0] == '-') {
        std::string temp = cleanToken(query.substr(1,query.length() - 1));
        query.replace(query.begin() + 1,query.end(),temp);
    } else {
        query = cleanToken(query);
    }
}


/*
 * 本函数执行实际的查询词匹配操作，包括修饰符的处理
 */
void termMatch(const Map<string, Set<string>>& index,const Vector<string>& terms,Set<string>& result) {
    for(const std::string& term: terms) {
        if(term[0] != '+' && term[0] != '-') {
            result.unionWith(index.get(term));
        } else if(term[0] == '+') {
            result.intersect(index.get(term.substr(1,term.length() - 1)));
        } else {
            result.difference(index.get(term.substr(1,term.length() - 1)));
        }
    }
}

/*
 * 实现匹配查询函数，需要自己重新写 trim 函数，然后逐个 term 匹配
 */ 
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Set<string> result;

    Vector<string> terms = stringSplit(query," ");
    for_each(terms.begin(),terms.end(),trimQuery);

    termMatch(index,terms,result);

    return result;
}
```
需要注意的是，经过我的测试，gatherTokens 中，std::ispunct 会将查询语句中的修饰符也给删掉，所以我们不能复用。这一点教案中并没有指出。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1719996866741-dfd14a2b-a36a-4d35-b28d-598ee24f0b4f.png#averageHue=%23f7f4f3&clientId=u9e0ff967-d9d2-4&from=paste&height=127&id=u9bf2ded8&originHeight=191&originWidth=312&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=7098&status=done&style=none&taskId=u37b6aeef-acc3-4a90-bc39-d074a5737c7&title=&width=208)
经过调试我们可以看到，程序运行恰好和我们的预想和手算结果相符。
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720001369768-9152ea5a-cf6a-47ff-b074-0adefc88bce8.png#averageHue=%23f9f6f5&clientId=u9e0ff967-d9d2-4&from=paste&height=388&id=u1def9261&originHeight=582&originWidth=559&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=37467&status=done&style=none&taskId=u88e9b63a-46ed-4673-8658-ad18eb2de70&title=&width=372.6666666666667)
完整通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720001433658-44f3dbb1-df3d-44d3-9815-1e03760e8713.png#averageHue=%23dfddda&clientId=u9e0ff967-d9d2-4&from=paste&height=487&id=u1f0c5403&originHeight=731&originWidth=1030&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=84107&status=done&style=none&taskId=u0746dc78-1693-4e7c-8cba-769616c462e&title=&width=686.6666666666666)
## 任务五：整合
到这里具体的实现已经结束了，我们最后需要将前几步整合到一个函数中去，实现用户查询输入的读取。
函数原型：
`void searchEngine(string dbfile)`
本函数需要的输入仅为一个数据库文件名，工作形式类似于控制台：

- 首先根据数据库文件建立反向索引；
- 打印处理的页面数，和找到的查询词匹配数到控制台；
- 进入循环，提示用户输入查询语句；
- 对于每次输入，匹配页面并将 URL 打印到控制台；
- 若用户输入空串，则视为查询结束，退出程序。

综上我们将代码整合起来得到：
```cpp
/*
 * 本函数执行搜索引擎的数据读取和索引建立步骤
 */
void preProcess(const std::string& dbfile,Map<string,Set<string>>& index) {
    int nPages = buildIndex(dbfile,index);
    std::cout << "Stand by while building index...\n";
    std::cout << "Indexed " << nPages << " pages containing " << index.size() << " unique terms\n";
    std::cout << std::endl;
}

/*
 * 本函数实现控制台交互
 */
void userInteraction(Map<string,Set<string>>& index) {
    while(1) {
        std::string input = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if(input.empty()) {
            // std::cout << "All done!" << std::endl; 这一步并不需要，控制台能自动打印
            return ;
        }
        
        Set<string> urls = findQueryMatches(index,input);
        
        if(!urls.isEmpty()) {
            std::cout << "Found " << urls.size() << " matching pages\n";
            std::cout << urls << std::endl;
        } else {
            std::cout << "No page has been found, please check your input" << std::endl;
        }
    }
}

/*
 * 具体功能已经被其他函数全部接管，实际上是一个初始化索引，执行函数调用的外壳
 */ 
void searchEngine(string dbfile)
{
    Map<string,Set<string>> index;
    preProcess(dbfile,index);

    userInteraction(index);
}
```
测试结果通过！
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1720004164460-9fe593e7-e527-4e4d-8b54-8609a8b59ff3.png#averageHue=%23faf9f8&clientId=u9e0ff967-d9d2-4&from=paste&height=717&id=u60166fe1&originHeight=1076&originWidth=1301&originalType=binary&ratio=1.5&rotation=0&showTitle=false&size=124167&status=done&style=none&taskId=u383abb1d-be35-43cb-8a2e-b3180013a42&title=&width=867.3333333333334)
值得一提的是，QT 的控制台还支持使用上下方向键来切换指令，可以说相当好用了。
# 参考文章

- [https://www.learncpp.com/cpp-tutorial/overloading-operators-using-normal-functions/](https://www.learncpp.com/cpp-tutorial/overloading-operators-using-normal-functions/)
- [https://www.learncpp.com/cpp-tutorial/stdarray-of-class-types-and-brace-elision/](https://www.learncpp.com/cpp-tutorial/stdarray-of-class-types-and-brace-elision/)
- [https://elloop.github.io/c++/2015-12-22/learning-using-stl-26-std-for-each](https://elloop.github.io/c++/2015-12-22/learning-using-stl-26-std-for-each)
