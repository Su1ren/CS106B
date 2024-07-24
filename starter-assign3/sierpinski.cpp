/*
 * TODO: remove and replace this file header comment
 * You will edit and turn in this file.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

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


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

