// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
#include <iostream>
#include <fstream>
#include <array>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;

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

/*
 * 四个方向逐个判断，返回可行（在界内，不撞墙）下一步的集合
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    const static std::array<GridLocation,4> directions = { // 注意这里的写法，初始化为三层大括号
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

/*
 * 首先判断栈顶是否为终点，之后逐步判断位置的移动是否合法。
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
        MazeGraphics::highlightPath(path,"green",1);
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


/*
 * 不带重复点剔除的求解，耗时较长，甚至可能死循环。
 */
Stack<GridLocation> solveMazeSlower(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;
    Queue<Stack<GridLocation>> bfs{{{0, 0}}}; // 初始状态
    GridLocation mazeExit = {maze.numRows() - 1, maze.numCols() - 1}; // 终点

    bool hasSolution = false;
    while(!bfs.isEmpty()) {
        path = bfs.dequeue();
        MazeGraphics::highlightPath(path,"green",1);
        if(!path.isEmpty() && path.peek() == mazeExit) {
            // path = front;
            hasSolution = true;
            break;
        }

        Set<GridLocation> nextLocations = generateValidMoves(maze, path.peek());
        // Set<GridLocation> visited = visitedLocations(path);
        for(GridLocation next: nextLocations) {
            Stack<GridLocation> extended = path;
            extended.push(next);
            bfs.enqueue(extended);
        }
    }
    if(!hasSolution)
        error("The maze has no solution, please check it again");

    return path;
}


/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// TODO: add your test cases here
/*
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
*/
