/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "SimpleTest.h"
#include "perfect.h"
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/*
 *
 */
long smarterSum(long n) {
    long total = 1;
    for(int i = 2;i <= sqrt(n);++i) {
        if(n % i == 0) {
            total += i;
            total += n / i;
        }
        if(n / i == i && n % i == 0)
            total -= i;
    }

    return total;
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
bool isPerfectSmarter(long n) {
    return n != 1 && smarterSum(n) == n;
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
void findPerfectsSmarter(long stop) {
    for(long i = 1;i < stop;++i) {
        if(isPerfectSmarter(i))
            std::cout << "Find a perfect num: " << i << '\n';
        if(i % 10000 == 0)
            std::cout << "." << std::flush;
    }
    std::cout << "Finish smart search to " << stop << '\n';
}

/* TODO: Replace this comment with a descriptive function
 * header comment.
 */
long findNthPerfectEuclid(long n) {
    int num {0};
    for(int k = 2;num != n;++k) {
        long long m = std::pow(2,k) - 1;
        if(smarterSum(m) == 1) { // m is prime
            std::cout << "Find the " << ++num << " th perfect num: " << m * std::pow(2,k - 1) <<'\n';
            if(num == n)
                return m * std::pow(2,k - 1);
        }
    }
    return 0;
}


/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!

/*
 * Here is sample test demonstrating how to use a loop to set the input sizes
 * for a sequence of time trials.
 */
//STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {
//    int smallest = 1000, largest = 8000;
//    for (int size = smallest; size <= largest; size *= 2) {
//        TIME_OPERATION(size, findPerfects(size));
//    }
//}


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}

STUDENT_TEST("Negative test, should fail") {
    //EXPECT(!isPerfect(-5));
}

STUDENT_TEST("Time test") {
    //TIME_OPERATION(40000,findPerfects(40000));
    //TIME_OPERATION(40000,findPerfectsSmarter(40000));
    //EXPECT_EQUAL(findNthPerfectEuclid(4),8128);
    //TIME_OPERATION(40000,findNthPerfectEuclid(4));
}
