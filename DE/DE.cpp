#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>
#include "test.h"

#define PI 3.14159265358

using namespace std;

namespace DE
{
    int d = 2;                   // dimension for ackley function
    double c = 2 * PI;
    double scale = 0.5;

    struct chromo
    {
        vector<double> xi;
        double current;
    };
    

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_real_distribution<double> distribution = uniform_real_distribution<double>(-40, 40);
    
    double ackley_function(chromo individual);    // fitness function
    auto mutation();
    auto reconbination();        // like crossover
    auto selection();
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

        return -a * exp(-b * sqrt( ( 1/d ) * sigma )) - exp( ( 1/d ) * sigma_cos ) + a + exp(1);
    }

    /*
    parameter : population pointer(?)
    return    : individual
    process   : randomly select 3 individual and caculate new individual
    *setting scale function weight 0.8?
    mutation one dimension or all dimension?
    maybe reconnination and selection in this function?
    */
    auto mutation(vector<chromo> *population) {
        uniform_int_distribution<int> distribution2 = uniform_int_distribution<int>(0, population->size() - 1);
        vector<chromo> Q;
        for(int i = 0; i < population->size(); i++) {
            int x1 = distribution2(generator);
            int x2 = distribution2(generator);
            int x3 = distribution2(generator);

            chromo v;
            for( int j = 0; j < d; j++) {
                
            }
        }
    }

    /*
    parameter : new individaul and parent
    return    : new individual(after crossover with parent)
    process   : setting mutation rate, if i = j crossover
    */
    auto reconbination() {

    }

    /*
    parameter : new individual and parent
    return    : survived individaul
    process   : compare with its parent and let better one survive
    */
    auto selection() {

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
            vector<chromo> *population;
            for (int j = 0; j < popsize; j++)
            {
                chromo temp = random_chromo();
                population->push_back(temp);
            }

            for(int j = 0; j < iteration; j++) {

            }
        }
    }
}