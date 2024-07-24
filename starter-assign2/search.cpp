// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "testing/SimpleTest.h"
#include "simpio.h"
using namespace std;

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
string cleanToken(string token)
{
    trimPunct(token);
    isValidToken(token);
    toLowerCaseInPlace(token);
    return token;
}

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
            // std::cout << "All done!" << std::endl;
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

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


// TODO: add your test cases here
STUDENT_TEST("cleanToken on strings with various tests") {
    EXPECT_EQUAL(cleanToken("561247"), "");
    EXPECT_EQUAL(cleanToken("''suiren''"), "suiren");
    EXPECT_EQUAL(cleanToken("\"suiren"), "suiren");
}

STUDENT_TEST("inverted index construction time cost measure") {
    Map<string, Set<string>> index;
    // int nPages = buildIndex("res/tiny.txt", index);
    TIME_OPERATION(11,buildIndex("res/tiny.txt", index));
    // EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
}

STUDENT_TEST("test trimQuery does not trim qualifier") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    std::string test = "++f+ish-";
    std::string check = "+f+ish";

    trimQuery(test);

    EXPECT_EQUAL(test,check);
}
