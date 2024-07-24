#include "labyrinth.h"
#include "demo/MazeGenerator.h"
#include "testing/SimpleTest.h"
using namespace std;

/* Change kYourName constant to your full name.
 *
 * WARNING: Once you've set set this constant and started exploring your labyrinth,
 * do NOT edit the value of kYourName. Changing kYourName will change which
 * maze you get back, which might invalidate all your hard work!
 */
const string kYourName = "Suiren";

/* Change this constant to the path out of your labyrinth. */
const string kPathOutOfNormalMaze = "SSEESNNESSNNWWNE";

/**
 * Given a location in a maze, returns whether the given sequence of
 * steps will let you escape the maze.
 *
 * To escape the maze, you need to collect all items in the `needs` set.
 * You can only take steps in the four cardinal directions,
 * and you can't move in directions that don't exist in the maze.
 */
bool isPathToFreedom(MazeCell* start, string moves, Set<string> needs) {
    if(needs.contains(start->contents))
        needs.remove(start->contents);
    for(char step: moves) {
        switch(step) {
        case 'E':
            start = start->east; break;
        case 'W':
            start = start->west; break;
        case 'S':
            start = start->south; break;
        case 'N':
            start = start->north; break;
        default:
            error("Invalid direction");
        }
        if(start == nullptr)
            return false;

        needs.remove(start->contents);
        if(needs.isEmpty())
            return true;
    }

    return needs.isEmpty();
}


/* * * * * * Test Cases Below This Point * * * * * */

PROVIDED_TEST("Check paths in the sample from writeup") {
    Set<string> allThree = {"Spellbook", "Potion", "Wand"};
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN", allThree));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES", allThree));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES", allThree));

    /* This path works to pick up just potion */
    EXPECT(isPathToFreedom(maze[2][2], "ESNW", {"Potion"}));

    /* This path doesn't work, since doesn't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW", allThree));

    /* This path doesn't work, cannot walk through walls. */
    EXPECT(!isPathToFreedom(maze[2][2], "NNWWSSSEEE", allThree));
}

PROVIDED_TEST("Works when start on an item") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "E", {"Potion"}));
    EXPECT(isPathToFreedom(maze[0][1], "W", {"Potion", "Spellbook"}));
}

PROVIDED_TEST("Reports errors if given illegal characters") {
    auto maze = toMaze({"P-S-W"});

    EXPECT_ERROR(isPathToFreedom(maze[0][0], "Q", {"Wand"}));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "Ee", {"Wand"}));
}

PROVIDED_TEST("Escape from your personal labyrinth!") {
    Set<string> allThree = {"Spellbook", "Potion", "Wand"};
    /* A maze for you to escape from. This maze will be personalized
     * based on the constant kYourName.
     */
    EXPECT(kYourName != "TODO");
    MazeCell* startLocation = mazeFor(kYourName);

    /* Set a breakpoint here. As a reminder, the labyrinth you
     * get will be personalized to you, so don't start exploring the labyrinth
     * unless you've edited the constant kYourName to include your name(s)!
     * Otherwise, you'll be escaping the wrong labyrinth.
     */
    EXPECT(kPathOutOfNormalMaze != "TODO");
    EXPECT(isPathToFreedom(startLocation, kPathOutOfNormalMaze, allThree));
}

