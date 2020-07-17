#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>
#include <list>
#include <cmath>
#include "mylib.h"
#include "myList.h"
#include "test.h"

using namespace std;

namespace TS_abort
{
    // random setting for reverse position
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, MAX_BITS - 1); // random number 0~MAX_BITS-1

    bool find(list<myList> tabu_list, myList target)
    {
        for (myList tabu : tabu_list)
        {
            if (tabu.bitlist == target.bitlist)
            {
                return true;
            }
        }
        return false;
    }

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

    void determination(int new_counter, int &counter, myList new_string, myList &bitstring, list<myList> &tabu_list, int length)
    {
        // new stirng is better and not found in tabu list
        if (new_counter > counter and not find(tabu_list, new_string))
        {
            cout << "New     : " << new_string.to_string() << endl;
            cout << "Counter : " << new_counter << endl;
            cout << "<Updated !!>" << endl;
            cout << "---------------------------------------------" << endl;
            bitstring = new_string;          // update it
            counter = new_counter;           // recording number of one
            tabu_list.push_back(new_string); // update tabu list
        }
        // Control the size of tabu list less than LENGTH
        if (tabu_list.size() > length)
            tabu_list.pop_front();
    }

    void output(myList bitstring, int counter)
    {
        ofstream myFile;
        myFile.open("TS.txt");
        myFile << "Best string : " << bitstring.to_string() << endl;
        myFile << "Counter     : " << counter << endl;
        myFile.close();
    }

    void TS(int maxbit, int iteration, int runtimes, int length)
    {
        for (int j = 0; j < runtimes; j++)
        {
            list<myList> tabu_list; // empty tabu list
            int counter = 0, new_counter = 0;
            myList bitstring(maxbit);
            myList new_string;
            mylib mb("TSout" + to_string(j), 10);

            initialization(bitstring, counter);
            for (int i = 0; i < iteration /*and counter != MAX_BITS*/; i++)
            {
                transition(new_string, bitstring);
                evaluation(new_counter, new_string);
                determination(new_counter, counter, new_string, bitstring, tabu_list, length);
                mb.adddata(i, counter); // i is iteration, counter is how many one
            }

            cout << "Tabu list size : " << tabu_list.size() << endl;
            output(bitstring, counter);
            mb.outputdata();
        }
        mySystemPause();
    }
} // namespace TS_abort