#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>

#define PI 3.14159265358

using namespace std;

namespace DE
{
    int d = 2;                   // dimension for ackley function
    double a = 20;
    double b = 0.2;
    double c = 2 * PI;
    double scale = 0.5;
    double reconbination_rate = 0.1;

    struct chromo
    {
        vector<double> xi;
        double current;
    };
    

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_real_distribution<double> distribution = uniform_real_distribution<double>(-40, 40);
    uniform_int_distribution<int> distribution3 = uniform_int_distribution<int>(0, d - 1);
    uniform_real_distribution<double> distribution4 = uniform_real_distribution<double>(0, 1);
    
    double ackley_function(chromo individual);          // fitness function
    auto mutation(vector<chromo> *population);
    chromo reconbination(chromo v, chromo parent);        // like crossover
    chromo selection(chromo v, chromo parent);
    auto DE_main(int runtimes, int iteration, int popsize);


    double ackley_function(chromo individual) {
        double sigma = 0;
        double sigma_cos = 0;
        for (int i = 0; i < d; i++)
        {
            sigma += pow( individual.xi[i], 2);
        }

        for(int i = 0; i < d; i++) {
            sigma_cos += cos(c * individual.xi[i]);
        }
        double temp = -a * exp(-b * sqrt( ( 1/d ) * sigma )) - exp( ( 1/d ) * sigma_cos ) + a + exp(1);
        return -a * exp(-b * sqrt( ( 1/d ) * sigma )) - exp( ( 1/d ) * sigma_cos ) + a + exp(1);
    }

    /*
    parameter : population pointer(?)
    return    : individual
    process   : randomly select 3 individual and caculate new individual
    *setting scale function weight 0.8 or lower?
    mutation one dimension or all dimension?
    maybe reconnination and selection in this function?
    */
    auto mutation(vector<chromo> &population) {
        uniform_int_distribution<int> distribution2 = uniform_int_distribution<int>(0, population.size() - 1);
        vector<chromo> Q;
        for(int i = 0; i < population.size(); i++) {
            int x1 = distribution2(generator);
            int x2 = distribution2(generator);
            int x3 = distribution2(generator);

            chromo v;
            // wtf???  *set stop point*
            for( int j = 0; j < d; j++) {
                v.xi.push_back(population.at(x1).xi[j] + scale * (population.at(x2).xi[j] - population.at(x3).xi[j]));
            }

            v = reconbination(v, population.at(x1));
            v = selection(v, population.at(x1));
            Q.push_back(v);
            cout << i << ". current : " << v.current << endl;
        }

        population = Q;
    }

    /*
    parameter : new individaul and parent
    return    : new individual(after crossover with parent)
    process   : setting mutation rate, if i = j crossover
    */
    chromo reconbination(chromo v, chromo parent) {
        int force = distribution3(generator);
        for( int i = 0; i < d; i++) {
            if( ( i == force ) or distribution4(generator) < reconbination_rate)
                parent.xi[i] = v.xi[i];
        }

        return parent;
    }

    /*
    parameter : new individual and parent
    return    : survived individaul
    process   : compare with its parent and let better one survive
    */
    chromo selection(chromo v, chromo parent) {
        v.current = ackley_function(v);
        parent.current = ackley_function(parent);
        if(v.current < parent.current)
            return v;
        else
            return parent;
    }

    auto random_chromo() {
        chromo temp;
        for(int i = 0; i < d; i++)
            temp.xi.push_back(distribution(generator));

        temp.current = ackley_function(temp);
        return temp;
    }

    auto DE_main(int runtimes, int iteration, int popsize)
    {
        for (int i = 0; i < runtimes; i++)
        {
            vector<chromo> population;
            // generate random popsize chromo
            for (int j = 0; j < popsize; j++)
            {
                chromo temp = random_chromo();
                temp.current = ackley_function(temp);
                population.push_back(temp);
            }

            for(int j = 0; j < iteration; j++) {
                mutation(population);
            }
        }
    }
}

int main() {
    DE::DE_main(1,10,20);
    return 0;
}