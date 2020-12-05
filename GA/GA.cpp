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

namespace GA
{
    class GA
    {
    private:

        struct children
        {
            myList Ca;
            myList Cb;
        };
        

        int _maxbit = 0, _iteration = 0, _runtimes = 0, _popsize = 0;
        int _best_result_value = 0, _best_result_iteration = 0;
        const double mutation_rate = 0.77;

        vector<myList> return_kth_element(vector<myList> population, int k) {
            for(int i = 0; i < k; i++) {
                int minIndex = i;
                int minValue = evaluation(population[i]);
                for (int j = i+1; j < _popsize; j++)
                {
                    if (evaluation(population[j]) > minValue)
                    {
                        minIndex = j;
                        minValue = evaluation(population[j]);
                        myList temp = population[i];
                        population[i] = population[minIndex];
                        population[minIndex] = temp;
                    }
                }
            }
            return population;
        }

        myList random_bit()
        {
            uniform_int_distribution<int> distribution2(0, 1);
            myList bits(_maxbit);
            for (int i = 0; i < bits.bitlist.size(); i++)
            {
                bits.bitlist[i] = distribution2(generator);
            }
            return bits;
        }

        int CDF_setter(vector<int> &CDF, vector<myList> population) {
            int best = evaluation(population.front());
            CDF.push_back(int(pow(best, 3)) / 1000);
            for (int i = 1; i < _popsize; i++)
            {
                int temp = evaluation(population[i]);
                if( temp > best )
                    best = temp;
                CDF.push_back( CDF[i-1] + (int(pow(temp, 3)) / 1000) );
            }
            return best;
        }

        int evaluation(myList new_string)
        {
            return (new_string.count());
        }

        vector<myList> initialization() {
            vector<myList> population;
            for (int i = 0; i < _popsize; i++)
            {
                population.push_back(random_bit());
            }
            return population;
        }

        // Select with replacement
        myList Fitness_Proportionate_Selection(vector<int> CDF, vector<myList> population) {
            uniform_int_distribution<int> distribution_p(0, CDF[CDF.size()-1]);
            int n = distribution_p(generator);
            for (int i = 1; i < population.size(); i++)
            {
                if (CDF[i-1] < n and CDF[i] >= n )
                {
                    return population[i];
                }
            }
            return population[0];
        }

        myList Tournament_Selection(vector<myList> population, int t_size=2) {
            uniform_int_distribution<int> distribution_p(0, _popsize - 1);
            int best = distribution_p(generator);
            for (int i = 0; i < t_size; i++)
            {
                int next = distribution_p(generator);
                if (evaluation(population[next]) > evaluation(population[best]))
                {
                    best = next;
                }
            }
            return population[best];
        }

        void Genetic_Algorithm_Elitism(vector<myList> population, vector<myList> &Q, int fit_size=4) {
            vector<myList> p_order = return_kth_element(population, fit_size);
            for (int i = 0; i < fit_size; i++)
            {
                Q.push_back(p_order[i]);
            }
        }

        // three type of crossover
        // testing pack two myList for gene
        children one_point_crossover(myList Pa, myList Pb) {
            uniform_int_distribution<int> distribution(0, _maxbit - 1);
            int c = distribution(generator);
            if (c != 1)
            {
                for (int i = c; i < _maxbit; i++)
                {
                    bool temp = Pa.bitlist[i];
                    Pa.bitlist[i] = Pb.bitlist[i];
                    Pb.bitlist[i] = temp;
                }
            }
            myList copy_Pa(Pa);
            myList copy_Pb(Pb);
            children childs;
            mutation(copy_Pa);
            mutation(copy_Pb);
            childs.Ca = copy_Pa;
            childs.Cb = copy_Pb;
            return childs;
        }

        children two_point_crossover(myList Pa, myList Pb)
        {
            uniform_int_distribution<int> distribution(0, _maxbit - 1);
            int c = distribution(generator);
            int d = distribution(generator);
            if (c > d)
            {
                int temp = c;
                c = d;
                d = temp;
            }
            for (int i = c; i < d - 1; i++)
            {
                bool temp = Pa.bitlist[i];
                Pa.bitlist[i] = Pb.bitlist[i];
                Pb.bitlist[i] = temp;
            }
            myList copy_Pa(Pa);
            myList copy_Pb(Pb);
            children childs;
            mutation(copy_Pa);
            mutation(copy_Pb);
            childs.Ca = copy_Pa;
            childs.Cb = copy_Pb;
            return childs;
        }

        children uniform_crossover(myList Pa, myList Pb) {
            uniform_real_distribution<double> distribution2(0, 1);

            for (int i = 0; i < _maxbit; i++)
            {
                if (distribution2(generator) > 0.5)
                {
                    bool temp = Pa.bitlist[i];
                    Pa.bitlist[i] = Pb.bitlist[i];
                    Pb.bitlist[i] = temp;
                }
            }
            myList copy_Pa(Pa);
            myList copy_Pb(Pb);
            children childs;
            mutation(copy_Pa);
            mutation(copy_Pb);
            childs.Ca = copy_Pa;
            childs.Cb = copy_Pb;
            return childs;
        }

        void mutation(myList &Px) {
            uniform_int_distribution<int> distribution(0, _maxbit - 1);
            uniform_real_distribution<double> distribution2(0, 1);
            if ( distribution2(generator) < mutation_rate ) {
                int c = distribution(generator);
                Px.flip(c);
            }
        }

    public:

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator;
    uniform_int_distribution<int> distribution;      // random number 0~MAX_BITS-1 -> pos
    uniform_real_distribution<double> distribution2; // random number 0~1 for r
    uniform_int_distribution<int> distribution_p;      // random number 0~popsize-1 

    // uniform corssover for roll
    void GA_run_r(){
        int average_i = 0, average_r = 0;
        for (int j = 0; j < _runtimes; j++)
        {
            mylib mb("GA_rout" + to_string(j), 1);
            _best_result_value = 0;
            _best_result_iteration = 0;
            vector<myList> P = initialization();
            for (int i = 0; i < _iteration; i++)
            {
                vector<int> CDF;
                int temp = CDF_setter(CDF, P);
                if (temp > _best_result_value) {
                    _best_result_value = temp;
                    _best_result_iteration = i;
                }
                
                // calculate best
                vector<myList> Q;
                for (int k = 0; k < _popsize / 2; k++)
                {
                    myList Pa = Fitness_Proportionate_Selection(CDF, P);
                    myList Pb = Fitness_Proportionate_Selection(CDF, P);
                    mutation(Pa);
                    mutation(Pb);
                    children temp = uniform_crossover(Pa, Pb);
                    Q.push_back(temp.Ca);
                    Q.push_back(temp.Cb);
                }
                P = Q;
                mb.adddata(i, _best_result_value);
            }
            average_i += _best_result_iteration;
            average_r += _best_result_value;
            cout << "Best    : " << _best_result_value << endl;
            cout << "Best it : " << _best_result_iteration << endl;
            // mb.outputdata();
            
        }
        cout << "Average Iteration: " << average_i / _runtimes << endl;
        cout << "Average Result   : " << average_r / _runtimes << endl;
        //mySystemPause();
    }

    // uniform crossover for tournament
    void GA_run_t(){
        int average_i = 0, average_r = 0;
        for (int j = 0; j < _runtimes; j++)
        {
            mylib mb("GA_tout" + to_string(j), 1);
            _best_result_value = 0;
            _best_result_iteration = 0;
            vector<myList> P = initialization();
            for (int i = 0; i < _iteration; i++)
            {
                vector<int> CDF;
                int temp = CDF_setter(CDF, P);
                if (temp > _best_result_value) {
                    _best_result_value = temp;
                    _best_result_iteration = i;
                }
                
                vector<myList> Q;
                // GA main process for selection and mutation
                for (int k = 0; k < _popsize / 2; k++)
                {
                    myList Pa = Tournament_Selection(P, 3);
                    myList Pb = Tournament_Selection(P, 3);
                    mutation(Pa);
                    mutation(Pb);
                    children temp = uniform_crossover(Pa, Pb);
                    Q.push_back(temp.Ca);
                    Q.push_back(temp.Cb);
                }
                P = Q;
                mb.adddata(i, _best_result_value);
            }
            average_i += _best_result_iteration;
            average_r += _best_result_value;
            cout << "Best    : " << _best_result_value << endl;
            cout << "Best it : " << _best_result_iteration << endl;
            // mb.outputdata();
        }
        cout << "Average Iteration: " << average_i / _runtimes << endl;
        cout << "Average Result   : " << average_r / _runtimes << endl;
        // mySystemPause();
    }

    // one point crossover for elitism
    void GA_run_e(){
        int average_i = 0, average_r = 0;
        for (int j = 0; j < _runtimes; j++)
        {
            mylib mb("GA_eout" + to_string(j), 1);
            _best_result_value = 0;
            _best_result_iteration = 0;
            vector<myList> P = initialization();
            for (int i = 0; i < _iteration; i++)
            {
                int temp = 0;
                // best individual in population
                for( auto p : P) {
                    if (evaluation(p) > temp)
                    {
                        temp = evaluation(p);
                    }
                }
                if (temp > _best_result_value) {
                    _best_result_value = temp;
                    _best_result_iteration = i;
                }
                
                // calculate best
                vector<myList> Q;
                // Pick top
                Genetic_Algorithm_Elitism(P, Q, 4);
                for (int k = 0; k < (_popsize - 4) / 2; k++)
                {
                    myList Pa = Tournament_Selection(P);
                    myList Pb = Tournament_Selection(P);
                    mutation(Pa);
                    mutation(Pb);
                    children temp = one_point_crossover(Pa, Pb);
                    Q.push_back(temp.Ca);
                    Q.push_back(temp.Cb);
                }
                P = Q;
                mb.adddata(i, _best_result_value);
            }
            average_i += _best_result_iteration;
            average_r += _best_result_value;
            cout << "Best    : " << _best_result_value << endl;
            cout << "Best it : " << _best_result_iteration << endl;
            // mb.outputdata();
            
        }
        cout << "Average Iteration: " << average_i / _runtimes << endl;
        cout << "Average Result   : " << average_r / _runtimes << endl;
        // mySystemPause();
    }

    GA() {
        _maxbit = 50;
        _iteration = 100;
        _runtimes = 30;
        _popsize = 20;
        generator = default_random_engine(seed);
        distribution = uniform_int_distribution<int>(0, _maxbit - 1);
        distribution2 = uniform_real_distribution<double>(0, 1);
        distribution_p = uniform_int_distribution<int>(0, _popsize - 1);
    }
    
    GA(int maxbit, int iteration, int runtimes, int popsize) {
        _maxbit = maxbit;
        _iteration = iteration;
        _runtimes = runtimes;
        _popsize = popsize;
        generator = default_random_engine(seed);
    }
    
    void output_population(vector<myList> population){
        for(auto gene: population) {
            for(auto bit : gene.bitlist)
                cout << bit ;
            cout << "  " << evaluation(gene) << endl;
        }
        mySystemPause();
    }
    };
}; // namespace GA