作业内容主要为算法，分为两个问题：

1. 完全数
2. Soundex 查找

用到的主要是基础算法、数论方面的内容。
# PerfectNum
一个非常经典的问题，求一定范围内的所有完全数。也就是所有除自己以外的因数之和等于自身的数。
## 穷举法
首先我们可以想到穷举法：
```cpp
long divisorSum(long n) {
    long sum{ 0 };
    for (int i{ 1 }; i < n; ++i)
        if (n % i == 0)
            sum += i;
    return sum;
}

bool isPerfect(long n) {
    return n != 0 && divisorSum(n) == n;
}

void findPerfects(long n) {
    for (int i{ 1 }; i < n; ++i) {
        if (isPerfect(i)) {
            std::cout << "Find a perfect Num: " << i << '\n';
        }
        if (i % 10000 == 0)
            std::cout << "." << std::flush;
    }
    std::cout << "Finish brute search to " << n << '\n';
}
```
在我电脑上测试，平均用时为 1.21s 左右。不难看出，这种穷举的时间复杂度为 O(n2)。
`Find a perfect Num: 6`
`Find a perfect Num: 28`
`Find a perfect Num: 496`
`Find a perfect Num: 8128`
`...Finish brute search to 40000`
`Brute search costs: 1.267s`
## 循环次数优化
上述的穷举时间复杂度达到了平方级，但是实际上我们不需要做这么多次循环。
因为对于任意自然数 n，若存在因数 a，那么必存在对应因数 b = n / a，且两者的最小值不大于![](https://cdn.nlark.com/yuque/__latex/dc6cd61c51eb8a988fa147213b0b3fc0.svg#card=math&code=%5Csqrt%7Bn%7D&id=p4TYT)。
另外需要注意的是完全平方数的情况，此时 a == b，可能存在边界情况。
所以我们可以将上述代码优化成：
```cpp
long smarterSum(long n) {
    long total = 1;
    for(int i = 2;i <= sqrt(n);++i) { // 从 2 开始，忽略 1 的情况
        if(n % i == 0) { // 同时加上 i 和 n / i
            total += i;
            total += n / i;
        }
        if(n / i == i && n % i == 0) // 出现完全平方数，减去一份
            total -= i;
    }

    return total;
}

bool isPerfectSmarter(long n) {
    return n != 1 && smarterSum(n) == n;
}

void findPerfectsSmarter(long stop) {
    for(long i = 1;i < stop;++i) {
        if(isPerfectSmarter(i))
            std::cout << "Find a perfect num: " << i << '\n';
        if(i % 10000 == 0)
            std::cout << "." << std::flush;
    }
    std::cout << "Finish smart search to " << stop << '\n';
}
```
这样再去运行，消耗时间直接降低到了 0.179s，在 ![](https://cdn.nlark.com/yuque/__latex/e4b34182d718b292bda80af7594e1c3d.svg#card=math&code=n%20%2A%20%5Csqrt%7Bn%7D&id=Uoh6Z) 复杂度情况下远远低于未优化的情况！
我们将内层循环的次数降低到了![](https://cdn.nlark.com/yuque/__latex/dc6cd61c51eb8a988fa147213b0b3fc0.svg#card=math&code=%5Csqrt%7Bn%7D&id=NXIUR)级，但我们的思路基本上仍然是一个数接一个地搜索，所以从第三个完全数 496 到下一个 8128，需要花费的相对时间仍然没变。有没有更高的办法呢？这需要用到一些数论上的知识 ，我们首先介绍梅森素数。 
## 梅森素数与欧几里得定理
首先给出定义，梅森素数是形为 2k - 1 （k 为整数）的素数，例如 31 = 25 - 1，就是一个梅森素数。
梅森素数的增长非常快，第 51 个梅森素数有将近 2500 万位。
在公元前四百多年，欧几里得发现了完全数和梅森素数之间的奇妙关系：
若 2k - 1 是素数，那么 (2k - 1) * 2k - 1 必然是完全数，例如 31 * 24 = 496 恰好是完全数。
这里给一个简单证明：
设 p = 2k - 1 是素数，n = (2k - 1) * 2k - 1 。要证明 n 是完全数，我们要证明其因数和 ![](https://cdn.nlark.com/yuque/__latex/a70f4eb38dd49b34bac3d1a6eef03b98.svg#card=math&code=%5Csigma%28n%29%20%3D%202n&id=zJyPq)。也就是一个自然数的所有因数（包括自身）之和为两倍自身值。计算公式需要用到素因数分解，如下图所示：
![79f0f736afc37931cbb881f5edc4b74542a9111d.webp](https://cdn.nlark.com/yuque/0/2024/webp/43291115/1718450447448-6d40bcd3-ac85-4ac8-adf9-2aa988a58d04.webp#averageHue=%23e8e8e8&clientId=u2fdac75e-bc71-4&from=ui&id=u781bae37&originHeight=410&originWidth=731&originalType=binary&ratio=1&rotation=0&showTitle=false&size=60988&status=done&style=none&taskId=u99fe051f-6aea-436b-9e86-245fce2933c&title=)
又因为 ![](https://cdn.nlark.com/yuque/__latex/ab4fb58389502a9d39af671d1da6fa4b.svg#card=math&code=%5Csigma%28p%29%20%3D%20p%20%2B%201%20%3D%202%20%5E%20k&id=LFjDq)，因数和函数是积性函数，所以可以得到：
![](https://cdn.nlark.com/yuque/__latex/83e518cc6c8763fbca5df2374eaf8e18.svg#card=math&code=%5Csigma%282%20%5E%20%7Bk%20-%201%7D%29%20%3D%201%20%2A%202%20%5E%201%20%2A%202%20%5E%202%20%2A%20...%20%2A%202%20%5E%20%7Bk%20-%201%7D%20%3D%20%5Cfrac%7B2%20%5E%20%7Bk%20-%201%20%2B%201%7D%20-%201%7D%7B2%20-%201%7D%20%3D%202%20%5E%20k%20-%201&id=L25v2)
![](https://cdn.nlark.com/yuque/__latex/93900521e03ac9eb56396cda2b07f104.svg#card=math&code=%5Csigma%28n%29%20%3D%20%5Csigma%282%20%5E%20%7Bk%20-%201%7D%29%20%2A%20%5Csigma%28p%29%20%3D%20%282%20%5E%20k%20-%201%29%20%2A%202%20%5E%20k%20%20%3D%202n&id=wTdMV) 
Q.E.D.
介绍了这么多数论知识，要如何应用到优化中去呢？
## 欧几里得优化
穷举法中我们从 1 到 n 逐个检查它们是否是完全数，但是现在有了以上的定理，我们可以直接根据梅森素数去找对应的完全数。步骤如下：

1. 从 k = 1 开始；
2. 计算 m = 2k - 1；
3. 判断 m 是否是素数，可以用 smarterSum 是否为 1 来判断；
4. 若 m 为素数，那么上述积就是完全数；
5. 自增 k，继续循环。 

这样我们可以用 findNthPerfectEuclid(n) 来寻找第 n 个完全数，比逐个判断的效率高得多！
```cpp
long findNthPerfectEuclid(long n) {
    int num {0};
    for(int k = 2;num != n;++k) { // 循环选择从 2 开始，避免 1 的判断
        long long m = std::pow(2,k) - 1;
        if(smarterSum(m) == 1) { // m is prime
            std::cout << "Find the " << ++num << " th perfect num: " << m * std::pow(2,k - 1) <<'\n';
            if(num == n)
                return m * std::pow(2,k - 1);
        }
    }
    return 0;
}

```
同样我们来找前四个完全数
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1718452210516-3360c9e9-dc1a-4917-a50a-24c0b05ca12f.png#averageHue=%23f8f6f4&clientId=u2fdac75e-bc71-4&from=paste&height=141&id=u1fdfcdda&originHeight=141&originWidth=693&originalType=binary&ratio=1&rotation=0&showTitle=false&size=15037&status=done&style=none&taskId=u7b032570-681d-441b-ad4c-f03be6ba302&title=&width=693)
甚至连计时器都无法检测到了，选择的 k 越大，这种方法的优势也越大！
# Soundex Search
## 概述
Soundex 查找是一种对姓名的发音进行编码与查询的算法，输入英文名得到发音的加密。
我们需要通过输入的姓名来对数据库中相同编码的姓名做匹配。
最后分析算法的功效和局限。
## 为什么需要 Soundex 查找
由于英语的发音和拼写发方面缺乏一致性，匹配姓氏可能会遇到麻烦。例如许多常见形式有各种拼写方式，而且由于输入数据有误、拼写文化差异以及音译错误等原因不断变化。
也正因如此，字符串的精确匹配算法在姓氏匹配上应用效果不佳。相比之下，Soundex 按照语音结构对名字进行分组，以实现按发音匹配，而不是按字面的字符匹配，有更高的效率。
## Soundex 编码方案
Soundex 编码结果为 4 字符长的字符串，以姓氏的头字母大写打头，后面跟 3 个数字。数字的编码算法如下：

1. 将姓氏中的非字母的字符全部去掉；
2. 按以下映射关系将字母转换为数字：
3. 将连续的相同数组去重，只保留一份；
4. 将结果的头数字替换为头字母大写；
5. 去掉结果中的所有 0；
6. 若结果长度大于 4 字符，则进行截断；若不满 4 字符，在末尾补 0。

映射表如下：

| Digit | represents the letters |
| --- | --- |
| 0 | A E I O U H W Y |
| 1 | B F P V |
| 2 | C G J K Q S X Z |
| 3 | D T |
| 4 | L |
| 5 | M N |
| 6 | R |

例如姓氏 “Angelou”，编码过程为：
Angelou -> 0520400 -> 052040 -> A52040 -> A524
### 将问题分解
我们一步一步来，将一个大的任务逐一分解。比如第一步，先将所有姓氏字符串全部转为数字。有一个初步方案：
```cpp
string lettersOnly(string s) {
    string result = charToString(s[0]);
    for (int i = 1; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}
```
但是问题也很明显，这种算法对于字符串全部都是非字母的，以及空字符串没有任何检查能力。
在传入空字符串时，得到的返回字符串是一个 "\x00"，而不是 ""。这说明我们的算法存在问题，需要再修改一下。
需要注意的是，CS106B 课程使用的模板库并不是 C++ 标准模板库。例如本作业中使用的 charToString 并不是标准库内的函数，而是斯坦福自主开发的功能函数，存放在 strlib.h 中，可以将单个字符转为字符串 std::string。CS106B 使用的动态数组、集合、映射表等容器都是自主开发的，命名上与 STL 的区别在于首字母大写，即 Vector、Set、Map 等。
最后我们修改成：
```cpp
string lettersOnly(string s) {
    string result{};
    for (int i = 0; i < s.length(); ++i) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}
```
不得不说，这种用值传递的方式我总想改一下，奈何头文件非要这么定义。
这下断言都能通过了。
### 字符映射
接下来我们进行后续步骤，首先是建立一个映射表，由于只在编码函数中使用，所以可以将其设为局部静态。由于不分大小写，所以映射表统一为大写。
注意在进行映射之前首先要对是否为空做检测，并且保存大写的首字母。
```cpp
/*
 * 本函数作为实际的映射执行函数
 */
void mapping(std::string& s) {
    static Map<char,int> encode = {
        {'A',0},
        {'B',1},
        {'C',2},
        {'D',3},
        {'E',0},
        {'F',1},
        {'G',2},
        {'H',0},
        {'I',0},
        {'J',2},
        {'K',2},
        {'L',4},
        {'M',5},
        {'N',5},
        {'O',0},
        {'P',1},
        {'Q',2},
        {'R',6},
        {'S',2},
        {'T',3},
        {'U',0},
        {'V',1},
        {'W',0},
        {'X',2},
        {'Y',0},
        {'Z',2},
        };
    for(int i = 0;i < s.size();++i) {
        s[i] = encode.get(s[i]) + '0';
    }
}

string soundex(string s) {
    if(s.empty())
        return "";

    toUpperCase(s);
    auto initial = s[0]; // Save the uppercase initial letter

    mapping(s);

    return s;
}
```
### 连续数字去重
完成映射之后，下一步对连续出现的数字去重，只保留一份。
用最简单的一次遍历。
```cpp
/*
 * 本函数用来对连续的数字做去重，然后将首个数字替换为大写字母。
 */
void coalesceAndReplace(std::string& s,char initial) {
    std::string re{};
    re.push_back(*s.begin());
    for(int i = 0;i < s.length();++i) {
        if(s[i] != s[i + 1] && s[i + 1] != '\0')
            re.push_back(s[i + 1]);
    }
    *re.begin() = initial;
    s = re;
}

string soundex(string s) {

    s = lettersOnly(s); // 去掉所有非字母字符
    if(s.empty()) // 若结果为空串，那么直接返回空串，不影响后续进行。
        return "";

    toUpperCaseInPlace(s);
    char initial = s[0]; // 保存大写首字母

    mapping(s);
    coalesceAndReplace(s,initial);

    return s;
}
```
### 去零、填充或截断
这里我们用 STL 中的算法来高效拷贝。
```cpp
/*
 * 本函数用来去除 s 中所有的 0，
 * 并且通过填充或者截断的方式将长度固定到 4 字符。
 */
void delZeroAndStapleLength(std::string& s) {
    std::string re;
    std::copy_if(s.begin(),s.end(),std::back_inserter(re),[](char x) {
        return x != '0';
    });

    if(re.length() > 4) { // 截断
        re.resize(4);
        s = re;
        return ;
    }
    if(re.length() < 4) { // 填充
        int padding = 4 - re.length();
        std::string temp(padding,'0');
        re.append(temp);
        s = re;
        return ;
    }
    s = re;
}

string soundex(string s) {

    s = lettersOnly(s); // 去掉所有非字母字符
    if(s.empty()) // 若结果为空串，那么直接返回空串，不影响后续进行。
        return "";

    toUpperCaseInPlace(s);
    char initial = s[0]; // 保存大写首字母

    mapping(s); // 映射替换
    coalesceAndReplace(s,initial); // 去重，替换首字母

    delZeroAndStapleLength(s); // 去 0，固定长度
    return s;
}
```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1718532485842-5d8665bb-9d74-4224-8fb0-3c9c7750b8b3.png#averageHue=%23adaa53&clientId=u90dbb483-98c5-4&from=paste&height=552&id=ub377429f&originHeight=552&originWidth=560&originalType=binary&ratio=1&rotation=0&showTitle=false&size=48268&status=done&style=none&taskId=udc502b97-cd15-41d2-937e-167767247bc&title=&width=560)
OK，全部通过！
## Soundex 查询
完成了编码方案，现在我们可以尝试查询姓氏，然后从存储姓氏的文件中读取数据，然后逐个编码，将得到的匹配结果排序打印。
这一部分需要做与用户交互的逻辑，读取用户输入，当输入为换行时退出。
注意斯坦福的 simpio.h 库中，getLine 函数不会将换行符存到输入的结果中。
所以我们可以这样写：
```cpp
/*
 * 本函数负责在数据库中遍历姓氏并计算编码，查询是否匹配。
 */
void inquiry(const Vector<string>& database,string dst) {
    Vector<string> re{};
    for(const auto& name:database) {
        auto encoded = soundex(name);
        if(encoded == dst)
            re.add(name);
    }
    if(re.isEmpty()) {
        std::cout << "No such surname recorded in database. Try again?\n";
    } else {
        re.sort();
        std::cout << "Matches from database: " << re << '\n';
    }
}

void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        allNames = readLines(in);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl;

    // The names read from file are now stored in Vector allNames
    while(1) {
        std::string input{};
        input = getLine("Enter a surname (RETURN to quit): ");
        if(input.empty())
            break;
        
        std::string encoded = soundex(input);
        if(!encoded.empty()) {
            std::cout << "Soundex code is " << encoded << '\n';
            inquiry(allNames,encoded);
        } else {
            std::cout << "Invalid Surname. Please input again.\n";
        }
        
        std::cout << std::endl;
    }
}
```
![image.png](https://cdn.nlark.com/yuque/0/2024/png/43291115/1718534934534-87fafd1c-2eec-4d2a-b692-dcf92a64125f.png#averageHue=%23f8f7f5&clientId=u90dbb483-98c5-4&from=paste&height=523&id=u8eab2033&originHeight=523&originWidth=800&originalType=binary&ratio=1&rotation=0&showTitle=false&size=40721&status=done&style=none&taskId=uf6646358-efbc-47b3-b290-9abb1097d57&title=&width=800)
测试成功！我们成功实现了 Soundex 查询功能。
## Soundex 的局限性
不是所有字符都可以用 ASCII 表示，例如俄文姓氏，这些用 Soundex 编码就不方便。但是 ASCII 码是 C++ 默认支持的编码标准，如果添加 Unicode 编码支持，系统的效率就会降低。所以使用 Soundex 编码方案也是一种公平和效率的权衡。
类似于 Soundex 的发音编码算法还有如 Daitch-Mokotoff、Beider-Morse、Metaphone、New York State Identification and Intelligence System。具体的信息可见参考。
# 参考文章

- [https://blog.csdn.net/nai_long/article/details/131525483](https://blog.csdn.net/nai_long/article/details/131525483)
- [https://www.cnblogs.com/kerwindeng/p/15013426.html](https://www.cnblogs.com/kerwindeng/p/15013426.html)
- [https://t5k.org/notes/proofs/EvenPerfect.html](https://t5k.org/notes/proofs/EvenPerfect.html)
- [https://blog.csdn.net/qq_41848675/article/details/98380652](https://blog.csdn.net/qq_41848675/article/details/98380652)
- [https://www.jianshu.com/p/4f72a03188b4](https://www.jianshu.com/p/4f72a03188b4)
- [https://stackoverflow.com/questions/55321484/copy-a-stdstring-using-copy-if-into-another-string](https://stackoverflow.com/questions/55321484/copy-a-stdstring-using-copy-if-into-another-string)
- [https://en.wikipedia.org/wiki/Daitch%E2%80%93Mokotoff_Soundex](https://en.wikipedia.org/wiki/Daitch%E2%80%93Mokotoff_Soundex)
- [https://stevemorse.org/phonetics/bmpm.htm](https://stevemorse.org/phonetics/bmpm.htm)
- [https://en.wikipedia.org/wiki/Metaphone](https://en.wikipedia.org/wiki/Metaphone)
- [https://en.wikipedia.org/wiki/New_York_State_Identification_and_Intelligence_System](https://en.wikipedia.org/wiki/New_York_State_Identification_and_Intelligence_System)
