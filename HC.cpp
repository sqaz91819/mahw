#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include "mylib.h"
#include "test.h"
#include "myList.h"

using namespace std;

namespace HC
{
    // random setting for reverse position
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, MAX_BITS - 1);

    void random_bit(myList &bits)
    {
        uniform_int_distribution<int> distribution(0, 1);
        for (int i = 0; i < bits.bitlist.size(); i++)
        {
            bits.bitlist[i] = distribution(generator);
        }
    }

    void initialization(myList &bitstring, int &counter)
    {
        random_bit(bitstring);       // set bitstring
        counter = bitstring.count(); // recording number of 1
        cout << "Initial : " << bitstring.to_string() << endl;
        cout << "Counter : " << counter << endl;
        cout << "---------------------------------------------" << endl;
    }

    void transition(myList &new_string, myList bitstring)
    {
        new_string = myList(bitstring);
        int pos = distribution(generator); // select random position from 0~ (MAXBITS-1)
        new_string.flip(pos);              // reverse one bit
        cout << "Test string : " << new_string.to_string() << endl;
        cout << "Test counter : " << new_string.count() << endl;
        cout << "---------------------------------------------" << endl;
    }

    void evaluation(int &new_counter, myList new_string)
    {
        new_counter = new_string.count();
    }

    void determination(int new_counter, int &counter, myList new_string, myList &bitstring)
    {
        // new stirng is better
        if (new_counter > counter)
        {
            cout << "New : " << new_string.to_string() << endl;
            cout << " Counter : " << new_counter << endl;
            cout << "Updated !!" << endl;
            cout << "---------------------------------------------" << endl;
            bitstring = new_string; // update it
            counter = new_counter;  // recording number of one
        }
    }

    void HC(int maxbit, int iteration, int runtimes)
    {
        int average_result = 0, average_iteration = 0;
        for (int j = 0; j < runtimes; j++)
        {
            int counter = 0, new_counter = 0;
            myList bitstring(maxbit);
            myList new_string;
            mylib mb("HCout" + to_string(j), 20);

            initialization(bitstring, counter);
            int best_result = counter, best_iteration = 0;
            for (int i = 0; i < iteration /*&& counter != MAX_BITS*/; i++)
            {
                transition(new_string, bitstring);
                evaluation(new_counter, new_string);
                determination(new_counter, counter, new_string, bitstring);
                mb.adddata(i, counter);
                if (counter > best_result)
                {
                    best_result = counter;
                    best_iteration = i;
                }
                // i is iteration, counter is how many one
            }

            average_result += best_result;
            average_iteration += best_iteration;
            //mb.outputdata();
        }
        output(average_result / runtimes, average_iteration / runtimes, "HC" + to_string(maxbit) + ".txt");
        // mySystemPause();
    }
} // namespace HC