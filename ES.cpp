#include <iostream>
#include <ctime>
#include "test.h"
#include "myList.h"
using namespace std;

namespace ES
{

    void ES(int maxbit, int iteraion, int time);

    void ES(int maxbit, int iteraion, int time)
    {
        int max = 0;
        myList ts(maxbit);
        clock_t t1;
        t1 = clock();
        while (clock() - t1 < time)
        {
            bit_add(ts);

            if (ts.count() > max)
                max = ts.count();

            cout << ts.to_string() << " Total one : " << ts.count() << endl;

            if (ts.count() == 100)
                break;
        }
        cout << "Max one : " << max << endl;
        mySystemPause();
    }
} // namespace ES
