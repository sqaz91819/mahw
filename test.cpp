
#include <bitset>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#define MAX_BITS 10
#define RUNTIMES 1000
using namespace std;

void bit_neg(bitset<MAX_BITS> &test)
{
    bool carry = false;
    if (!test[0])
        carry = true;
    for (int i = 0; i < test.size(); i++)
    {
        if (!test[i] && carry)
        {
            test.flip(i);
            carry = true;
        }
        else
        {
            test.flip(i);
            break;
        }
    }
}

int main() {
    bitset<MAX_BITS> test("0000000001");
    for (int i = 0; i < 10; i++)
    {
        bit_neg(test);
        cout << test.to_string() << endl;
    }
    
}