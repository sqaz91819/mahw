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

namespace TS
{
    class TS
    {

    private:
        
        int _maxbit = 0, _iteration = 0, _runtimes = 0, _length = 0;
        int _best_reulst_value = 0;

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

        myList random_bit()
        {
            uniform_int_distribution<int> distribution(0, 1);
            myList bits(_maxbit);
            for (int i = 0; i < bits.bitlist.size(); i++)
            {
                bits.bitlist[i] = distribution(generator);
            }
            return bits;
        }

        myList initialization(int &counter)
        {
            myList bitstring = random_bit(); // set bitstring
            counter = bitstring.count();     // recording number of 1
            cout << "Initial : " << bitstring.to_string() << endl;
            cout << "Counter : " << counter << endl;
            cout << "---------------------------------------------" << endl;
            return bitstring;
        }

        myList transition(myList bitstring)
        {
            uniform_int_distribution<int> distribution(0, _maxbit - 1);
            myList new_string(bitstring);
            int pos = distribution(generator); // select random position from 0~ (MAXBITS-1)
            new_string.flip(pos);              // reverse one bit
            cout << "Test string : " << new_string.to_string() << endl;
            cout << "Test counter : " << new_string.count() << endl;
            cout << "---------------------------------------------" << endl;
            return new_string;
        }

        int evaluation(myList new_string)
        {
            return (new_string.count());
        }

    public:
        
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator;
        uniform_int_distribution<int> distribution;      // random number 0~MAX_BITS-1 -> pos
        uniform_real_distribution<double> distribution2; // random number 0~1 for r

        void TS_run()
        {
            int average_result = 0, average_iteration = 0;
            for (int j = 0; j < _runtimes; j++)
            {
                int counter = 0, new_counter = 0;
                list<myList> tabu_list; // empty tabu list
                mylib mb("TSout" + to_string(j), 20);
                myList s = initialization(counter);
                int f1 = evaluation(s);
                int best_result = f1, best_iteration = 0;
                for (int i = 0; i < _iteration /*and counter != MAX_BITS*/; i++)
                {
                    myList v = transition(s);
                    int f2 = evaluation(v);
                    if (f2 > f1 and not find(tabu_list, v))
                    {
                        cout << "New     : " << v.to_string() << endl;
                        cout << "Counter : " << f2 << endl;
                        cout << "<Updated !!>" << endl;
                        cout << "---------------------------------------------" << endl;
                        s = v;   // update it
                        f1 = f2; // update current object  value
                        tabu_list.push_back(s); // update tabu list
                        if (tabu_list.size() > _length)
                            tabu_list.pop_front();
                        if( f1 > best_result ) {
                            best_result = f1;
                            best_iteration = i;
                        }
                    }
                    mb.adddata(i, best_result); // i is iteration
                }
                
                average_result += best_result;
                average_iteration += best_iteration;
                cout << "Tabu list size : " << tabu_list.size() << endl;
                mb.outputdata();
            }
            output(average_result / _runtimes, average_iteration / _runtimes, "TS" + to_string(_maxbit) + ".txt");
            // mySystemPause();
        }

        TS()
        {
            _maxbit = 100;
            _iteration = 1000;
            _runtimes = 1;
            _length = 100;
            generator = default_random_engine(seed);
        }

        TS(int maxbit, int iteration, int runtimes, int length)
        {
            _maxbit = maxbit;
            _iteration = iteration;
            _runtimes = runtimes;
            _length = length;
            generator = default_random_engine(seed);
        }
    };
}; // namespace TS