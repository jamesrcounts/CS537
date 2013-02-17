/***************************************************
test.cpp

Jim Counts & Michael Ngyuen
CS 537 - Data Communications
Homework 1
**************************************************/

#include <igloo/igloo.h>

using namespace igloo;
void SwapIntPointers(int * a, int * b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

Context(SwappingIntPointers)
{
    Spec(TwoIntValuesAreSwapped)
    {
        int a = 37;
        int b = 42;
        SwapIntPointers(&a, &b);
        Assert::That(a, Equals(42));
        Assert::That(b, Equals(37));
    }

};

int main()
{
    return TestRunner::RunAllTests();
}