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

namespace PSO
{
    int swarm_size;
    double alpha;    // proportion of velocity to be retained
    double beta;     // proportion of personal best to be retained
    double gamma;    // proportion of the informants’ best to be retained
    double delta;    // proportion of global best to be retained
    double epsilon;  // jump size of a particle

    double a = 20;
    double b = 0.2;
    double c = 2 * PI;
    double d = 2;        // default dimenesion 2

    struct particle {
        vector<double> xi;
        vector<double> personal_v;
        vector<double> personal_xi;
        double best;
        double current;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_real_distribution<double> distribution = uniform_real_distribution<double>(-40, 40);
    uniform_real_distribution<double> distribution_v = uniform_real_distribution<double>(-15, 15);

    auto random_particle();
    double ackley_function(particle p);
    auto PSO_main(int runtimes, int iteration, int _swarm_size);


    auto random_particle() {
        particle p;
        // determine position at dimension
        for(int i = 0; i < d; i++)
            p.xi.push_back(distribution(generator));

        for(int i = 0; i < d; i++)
            p.personal_v.push_back( distribution_v(generator));

        p.personal_xi = p.xi;
        p.best = ackley_function(p);
        p.current = p.best;
        return p;
    }

    double ackley_function(particle p) {
        double sigma = 0;
        double sigma_cos = 0;
        for (int i = 0; i < d; i++)
        {
            sigma += pow( p.xi[i], 2);
        }

        for(int i = 0; i < d; i++) {
            sigma_cos += cos(c * p.xi[i]);
        }

        return -a * exp(-b * sqrt( ( 1/d ) * sigma )) - exp( ( 1/d ) * sigma_cos ) + a + exp(1);  
    }

    auto PSO_main(int runtimes, int iteration, int _swarm_size)
    {
        uniform_real_distribution<double> distribution2 = uniform_real_distribution<double>(0, 1);
        vector<double> log(iteration, 0);

        for (int i = 0; i < runtimes; i++)
        {
            vector<particle> P;
            for (int j = 0; j < _swarm_size; j++)
                P.push_back(random_particle());

            particle best = P[0];

            for(int j = 0; j < iteration; j++) {
                for(auto p : P)
                    if( p.current < best.current )
                        best = p;

                for(auto &p : P) {
                    for(int k = 0; k < d; k++) {
                        double lo1 = distribution2(generator);
                        double lo2 = distribution2(generator);
                        p.personal_v[k] = 0.4 * p.personal_v[k] + lo1 * 2 * (p.personal_xi[k] - p.xi[k]) + lo2 * 2 * (best.xi[k] - p.xi[k]);
                    }

                    for(int k = 0; k < d; k++) {
                        p.xi[k] += p.personal_v[k];
                        if(p.xi[k] < -40) p.xi[k] = -40;
                        if(p.xi[k] > 40 ) p.xi[k] = 40;
                    }

                    p.current = ackley_function(p);
                    if(p.current < p.best) {
                        p.best = p.current;
                        p.personal_xi = p.xi;
                    }

                    if(p.current < best.current) best = p;
                }
                cout << j << " ";
                cout << best.current << endl;
                log[j] += best.current;
            }

        }

        return log;
    }
} // namespace PSO

int main(int argc, char *argv[]) {
    int runtimes = 30;
    int iteration = 200;
    int swarm_size = 20;
    if(argc > 3) {
        runtimes = std::stoi(argv[1]);
        iteration = std::stoi(argv[2]);
        swarm_size = std::stoi(argv[3]);
    }

    vector<double> temp = PSO::PSO_main(runtimes, iteration, swarm_size);

    ofstream plotfile;
    plotfile.open("PSO_result.txt");
    for(int i = 0; i < iteration; i++) {
        plotfile << i << " " << temp[i] / runtimes << endl;
    }

    plotfile.close();
}