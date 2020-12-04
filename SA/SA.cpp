#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>
#include <cmath>
#include "myList.h"
#include "mylib.h"
#include "test.h"

#define ALPHA 0.97
using namespace std;

namespace SA
{
    class SA
    {
    private:
        int _maxbit = 0, _iteration = 0, _runtimes = 0;
        double _temperature = 0;
        int _best_reulst_value = 0;

        // random setting for reverse position
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

        myList initialization()
        {
            myList bitstring = random_bit(); // set bitstring
            cout << "Initial : " << bitstring.to_string() << endl;
            cout << "Counter : " << bitstring.count() << endl;
            cout << "---------------------------------------------" << endl;
            return bitstring;
        }

        myList transition(myList bitstring)
        {
            distribution = uniform_int_distribution<int>(0, _maxbit - 1);
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

        void SA_run()
        {
            int average_result = 0, average_iteration = 0;
            for (int j = 0; j < _runtimes; j++)
            {
                double temperature = _temperature;
                mylib mb("SAout" + to_string(j), 20);
                distribution2 = uniform_real_distribution<double>(0, 1);

                myList s = initialization();
                int f1 = evaluation(s);
                int best_result = f1, best_iteration = 0;
                for (int i = 0; i < _iteration /*and counter != maxbit and temperature > 0.00001*/; i++)
                {
                    myList v = transition(s);
                    int f2 = evaluation(v);
                    double r = distribution2(generator); // r = [0~1]
                    double Pa = exp((f2 - f1) / temperature);
                    if (f2 > f1 or Pa > r)
                    {
                        cout << "New     : " << v.to_string() << endl;
                        cout << "Counter : " << f2 << endl;
                        cout << "<Updated !!>" << endl;
                        cout << "---------------------------------------------" << endl;
                        s = v;   // update it
                        f1 = f2; // update current object  value

                        // record current best result and iteration
                        if( f1 > best_result ) {
                            best_result = f1;
                            best_iteration = i;
                        }
                    }
                    temperature *= ALPHA;              // alpha for Annealing
                    mb.adddata(i, f1); // i is iteration
                }

                average_result += best_result;
                average_iteration += best_iteration;
                cout << std::fixed;
                cout << "Temp : " << setprecision(5) << temperature << endl;
                mb.outputdata();
            }
            output(average_result / _runtimes, average_iteration / _runtimes, "SA" + to_string(_maxbit) + ".txt");
            // mySystemPause();
        }

        // default attributions for testing
        SA()
        {
            _maxbit = 100;
            _iteration = 1000;
            _runtimes = 1;
            _temperature = 10;
            generator = default_random_engine(seed);
        }

        SA(int maxbit, int iteration, int runtimes, double temprature)
        {
            _maxbit = maxbit;
            _iteration = iteration;
            _runtimes = runtimes;
            _temperature = temprature;
            generator = default_random_engine(seed);

        }
    };
}; // namespace SA