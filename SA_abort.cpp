#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>
#include <cmath>
#include "myList.h"
#include "mylib.h"
#include "test.h"

#define ALPHA 0.98

using namespace std;
namespace SA_abort
{

    // random setting for reverse position
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(0, MAX_BITS - 1); // random number 0~MAX_BITS-1 -> pos
    uniform_real_distribution<double> distribution2(0.0, 1.0);   // random number 0~1 for r

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

    void determination(int new_counter, int &counter, myList new_string, myList &bitstring, double temperature)
    {
        double r = distribution2(generator); // r = [0~1]
        double Pa = exp((new_counter - counter) / temperature);
        // new stirng is better
        if (new_counter > counter or Pa > r)
        {
            cout << "New     : " << new_string.to_string() << endl;
            cout << "Counter : " << new_counter << endl;
            cout << "<Updated !!>" << endl;
            cout << "---------------------------------------------" << endl;
            bitstring = new_string; // update it
            counter = new_counter;  // recording number of one
        }
    }

    void output(myList bitstring, int counter)
    {
        ofstream myFile;
        myFile.open("SA.txt");
        myFile << "Best string : " << bitstring.to_string() << endl;
        myFile << "Counter     : " << counter << endl;
        myFile.close();
    }

    void SA(int maxbit, int iteration, int runtimes, double temperature)
    {
        for (int j = 0; j < runtimes; j++)
        {
            int counter = 0, new_counter = 0;
            myList bitstring(maxbit);
            myList new_string;
            mylib mb("SAout" + to_string(j), 10);
            initialization(bitstring, counter);
            for (int i = 0; i < iteration /*and counter != maxbit*/ and temperature > 0; i++)
            {
                transition(new_string, bitstring);
                evaluation(new_counter, new_string);
                determination(new_counter, counter, new_string, bitstring, temperature);
                temperature *= ALPHA;   // alpha for Annealing
                mb.adddata(i, counter); // i is iteration, counter is how many one
            }

            cout << std::fixed;
            cout << "Temp : " << setprecision(5) << temperature << endl;
            mb.outputdata();
            output(bitstring, counter);
        }
        mySystemPause();
    }
} // namespace SA