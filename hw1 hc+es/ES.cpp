#include <iostream>
#include <bitset>
#include <ctime>
#include "test.h"
#include "mylib.h"
#define MAX_BITS 100
#define RUNTIMES 1000
using namespace std;

int main()
{
    mylib mb("ESout", 10);
    int max = 0;
    bitset<MAX_BITS> ts;
    clock_t t1,t2;
    t1 = clock();
    t2 = clock();
    while (t2 - t1 < 3600000)
    {
        bit_add(ts);
        if (ts.count() > max)
        {
            max = ts.count();
        }
        cout << ts.to_string() << " Total one : " << ts.count() << endl;
        if (ts.count() == 100)
            break;
        t2 = clock();
    }
    cout << "Max one : " << max << endl;
    mySystemPause();
}