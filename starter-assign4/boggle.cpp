/*
 * TODO: remove and replace this file header comment
 * You will edit and turn in this file.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "gridlocation.h"
#include "grid.h"
#include "set.h"
#include "lexicon.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * 小于四字母的不得分
 */
int points(string str) {
    /* TODO: Implement this function. */
    if(str.length() < 4)
        return 0;
    return str.length() - 3;
}


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


void printWords(const Set<std::string>& words) {
    std::cout << words << std::endl;;
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

    printWords(words);
    int sum = 0;
    for(std::string word: words) {
        sum += points(word);
    }
    return sum;
}

/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Load shared Lexicon, confirm number of words") {
    Lexicon lex = sharedLexicon();
    EXPECT_EQUAL(lex.size(), 127145);
}

PROVIDED_TEST("Test point scoring") {
    EXPECT_EQUAL(points("and"), 0);
    EXPECT_EQUAL(points("quad"), 1);
    EXPECT_EQUAL(points("quint"), 2);
    EXPECT_EQUAL(points("sextet"), 3);
    EXPECT_EQUAL(points("seventh"), 4);
    EXPECT_EQUAL(points("supercomputer"), 10);
}

PROVIDED_TEST("Test scoreBoard, board contains no words, score of zero") {
    Grid<char> board = {{'B','C','D','F'}, //no vowels, no words
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}

PROVIDED_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'C','_','_','_'},
                        {'Z','_','_','_'},
                        {'_','A','_','_'},
                        {'_','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, alternate paths for same word, still score of 1") {
    Grid<char> board = {{'C','C','_','_'},
                        {'C','Z','C','_'},
                        {'_','A','_','_'},
                        {'R','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, small number of words in corner of board") {
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    Set<string> words = {"SMILE", "LIMES", "MILES", "MILE", "MIES", "LIME", "LIES", "ELMS", "SEMI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()),  2 + 2 + 2 + 1 + 1 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, small number of words") {
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    Set<string> words = { "HORIZON", "OHIA", "ORZO", "JOHN", "HAJI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 4 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, medium number of words") {
    Grid<char> board = {{'O','T','H','X'},
                        {'T','H','T','P'},
                        {'S','S','F','E'},
                        {'N','A','L','T'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 76);
}

PROVIDED_TEST("Test scoreBoard, full board, large number of words") {
    Grid<char> board = {{'E','A','A','R'},
                        {'L','V','T','S'},
                        {'R','A','A','N'},
                        {'O','I','S','E'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 234);
}

STUDENT_TEST("check one missing") {
    Set<std::string> inspected = {"ALEF", "ALEFS", "ALEPH", "EFTS", "ELAN", "ELANS", "ETHS", "FANS", "FASH", "FAST", "FASTS", "FETS", "FLAN", "FLANS", "FLASH", "HOTS", "LASH", "LASS", "LAST", "LASTS", "LEFT", "LEFTS", "LEPT", "LETS", "PELF", "PELFS", "PELT", "PETS", "PETTO", "SAFE", "SALE", "SALEP", "SALS", "SALT", "SANS", "SASH", "SHOT", "SHOTS", "SHOTT", "SHOTTS", "SHTETL", "SLASH", "SLEPT", "SNASH", "STELA", "STEP", "STET", "TELA", "TELS", "TETH", "TETHS", "TETS", "TOTS"};
    Set<std::string> all = {"TOTS","HOTS","TELS","TELA","PETTO","PETS","PELF","PELFS","PELT","SHOT","SHOTT","SHOTTS","SHOTS","SHTETL","SNASH","SASH","SAFE","SANS","SALS","SALE","SALEP","SALT","STEP","STELA","STET","SLEPT","SLASH","FETS","FELT","FAST","FASTS","FASH","FANS","FLASH","FLAN","FLANS","ETHS","EFTS","ELAN","ELANS","ALEPH","ALEF","ALEFS","LETS","LEPT","LEFT","LEFTS","LAST","LASTS","LASH","LASS","TETH","TETHS","TETS"};
    EXPECT_EQUAL(all.size(),54);
    std::cout << "The missing one is " << all.difference(inspected) << std::endl;
    // Lexicon lex = sharedLexicon();
    // EXPECT(!lex.containsPrefix(""));
}
