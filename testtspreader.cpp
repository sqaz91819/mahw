#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <map>
#include <unordered_map>
#include <ctime>
#include "test.h"

#define T_SIZE 51

using namespace std;

namespace ACO
{
    vector<int> cities;
    double alpha = 0.01; // learning rate
    double beta = 0.5; // evaparation
    double gamma = 0.001; // init pheromone
    double theta = 0.01; // heuristic selection
    double e = 0.01; // pheromones selection
    double t = 1; // iteration for HC
    double q = 0.8; // select component in elist way
    double tao = 0.01;
    vector<vector<double>> p(T_SIZE, vector<double>(T_SIZE, gamma)); // pheromones for all component size:T_SIZE * T_SIZE

    double dis_matrix[T_SIZE][T_SIZE];
    double inverse_dis[T_SIZE][T_SIZE];
    double prob[T_SIZE][T_SIZE];
    double pheXdis[T_SIZE][T_SIZE] = {{1}};
    vector<vector<int>> coordination;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_int_distribution<int> distribution = uniform_int_distribution<int>(0, 50);


    // vector<vector<int>> or vector<int[2]> ??
    auto coordinations()
    {
    }

    auto distance_matrix()
    {
    }

    auto distance_cal(vector<int> trail) {
        double total = 0;
        for (int i = 0; i < T_SIZE - 1; i++)
        {
            total += dis_matrix[trail[i]][trail[i+1]];
        }
        return total;
    }

    auto random_trail()
    {
        vector<int> cities;
        for (int i = 0; i < T_SIZE; i++)
        {
            cities.push_back(i);
        }
        
        random_shuffle(cities.begin(), cities.end());
        return cities;
    }

    auto constructAntSolution(int popsize, vector<int> cities, vector<vector<int>> P) {
        for (int i = 0; i < popsize; i++)
        {
            P.push_back(random_trail());
        }
    }

    auto applyLocalSearch() {

    }

    auto updatePheromones() {

    }

    double trail_dis(vector<int> trail) {
        double total = 0;
        for(int i = 0; i < T_SIZE - 1; i++) {
            total += dis_matrix[trail[i]][trail[i+1]];
        }
        total += dis_matrix[trail[T_SIZE - 1]][trail[0]];
        return total;
    }

    void printer(vector<vector<int>> trails) {
        for( auto s : trails) {
            for( auto si : s) {
                cout << si << " ";
            }
            cout << "size  : " << s.size() << "  ";
            cout << "trail : " <<  trail_dis(s);
            cout << endl;
        }
    }

    auto local_hc(vector<int> trail) {
        vector<int> new_trail(trail);
        for (int i = 0; i < 30; i++)
        {
            int a = distribution(generator);
            int b = distribution(generator);
            int temp = new_trail[a];
            new_trail[a] = new_trail[b];
            new_trail[b] = temp;
            if (distance_cal(new_trail) < distance_cal(trail))
            {
                return new_trail;
            }
        }
        return trail;
    }

    auto opt2_swap(vector<int> trail, int i, int j) {
        vector<int> new_trail(T_SIZE, 0);
        for (int a = 0; a < i; a++)
        {
            new_trail[a] = trail[a];
        }
        for(int a = j; a >= i; a--) {
            new_trail[i+j-a] = trail[a];
        }

        for(int a = j+1; a < T_SIZE; a++) {
            new_trail[a] = trail[a];
        }
        
        return new_trail;
    }

    vector<int> opt2(vector<int> trail) {
        double best = distance_cal(trail);
        for(int i = 0; i < T_SIZE - 1; i++) {
            for (int j = i + 1; j < T_SIZE; j++)
            {
                vector<int> new_trail = opt2_swap(trail, i, j);
                double dis = distance_cal(new_trail);
                if (dis < best)
                {
                    return new_trail;
                }
            }
        }
        return trail;
    }

    void ACO(int runtimes, int iteration, int popsize) {
        double run_total = 0;
        double total_best = 9999;
        vector<int> total_trail;
        for (int i = 0; i < runtimes; i++)
        {
            vector<vector<double>> p(T_SIZE, vector<double>(T_SIZE, gamma)); // pheromones for all component size:T_SIZE * T_SIZE
            for( auto &X : pheXdis)
                for( auto &D : X)
                    D = 1;
            double best = 99999;
            vector<int> best_trail;
            for (int j = 0; j < iteration; j++)
            {
                vector<vector<int>> population; // population for ant trail
                for (int k = 0; k < T_SIZE; k++)
                {
                    for (int l = 0; l < T_SIZE; l++)
                    {
                        pheXdis[k][l] = p[k][l] * pow(inverse_dis[k][l], 2);
                    }
                }

                // clock_t t = clock();

                for (int k = 0; k < popsize; k++)
                {
                    unordered_map<int,int> test;
                    vector<int> s; // solution trail and record used cities to avoid repetition
                    int start = distribution(generator);
                    s.push_back(start);
                    test.insert({start,start});


                    // generate trail
                    for (int l = 0; l < T_SIZE - 1; l++)
                    {
                        // CDF to select next city
                        vector<double> CDF_prob;
                        vector<int> unused;
                        double total = 0;
                        for (int m = 0; m < T_SIZE; m++)
                        {
                            if(test.find(m) == test.end()) {
                                CDF_prob.push_back(pheXdis[start][m]);
                                total += pheXdis[start][m];
                                unused.push_back(m);
                            }
                        }
                        CDF_prob[0] = CDF_prob[0] / total;

                        for(int m = 1; m < CDF_prob.size(); m++) {
                            CDF_prob[m] = CDF_prob[m-1] + (CDF_prob[m] / total);
                        }

                        CDF_prob[CDF_prob.size()-1] = 1;
                        uniform_real_distribution<double> distribution_real = uniform_real_distribution<double>(0, 1);
                        double rand_real = distribution_real(generator);
                        for (int m = 0; m < unused.size(); m++)
                        {
                            if( CDF_prob[m] >= rand_real )
                            {
                                s.push_back(unused[m]);
                                test.insert({unused[m],unused[m]});
                                start = unused[m];
                                break;
                            }
                        }
                    }

                    // s = local_hc(s);
                    if (best < 480)
                    {
                        s = opt2(s);
                    }
                    
                    population.push_back(s);
                    double temp = trail_dis(s);
                    if(temp < best) {
                        best = temp;
                        best_trail = s;
                    }
                    if(best < total_best) {
                        total_best = best;
                        total_trail = best_trail;
                    }

                }
                // cout << "time1 : " << clock() - t << endl;
                // printer(population);

                // evaporate pheromones
                for(auto &pi : p) {
                    for( auto &pk : pi)
                        pk = (1-beta) * pk;
                }
                // update pheromones
                
                for(int k = 0; k < popsize; k++) {
                    double dis_temp = 10 / trail_dis(population[k]) * beta;
                    for( int l = 0; l < T_SIZE - 1; l++) {
                        p[population[k][l]][population[k][l+1]] += dis_temp;
                        p[population[k][l+1]][population[k][l]] += dis_temp;
                    }
                    p[population[k][T_SIZE-1]][population[k][0]] += dis_temp;
                    p[population[k][0]][population[k][T_SIZE-1]] += dis_temp;
                }

                double dis_temp = 10 / trail_dis(total_trail) * tao;
                for(int k = 0; k < T_SIZE - 1; k++) {
                    p[total_trail[k]][total_trail[k+1]] = p[total_trail[k]][total_trail[k+1]] * (1-tao) + dis_temp;
                    p[total_trail[k+1]][total_trail[k]] = p[total_trail[k]][total_trail[k+1]];
                }
                p[total_trail[T_SIZE-1]][total_trail[0]] = p[total_trail[T_SIZE-1]][total_trail[0]] * (1-tao) + dis_temp;
                p[total_trail[0]][total_trail[T_SIZE-1]] = p[total_trail[T_SIZE-1]][total_trail[0]];
                // cout << endl;
                
            }
            cout << "Best : " << best << endl;
            run_total += best;
        }
        cout << "Average Best : " << run_total / runtimes << endl;
        ofstream plotfile;
        plotfile.open("plot.txt");
        for (auto t : total_trail)
        {
            plotfile << coordination[t][0] << " " << coordination[t][1] << endl;
        }
        plotfile << coordination[total_trail[0]][0] << " " << coordination[total_trail[0]][1] << endl;
    }

}; // namespace ACO

int main(int argc, char *argv[])
    {
        ifstream file;
        file.open("eil51.tsp.txt");
        string temp;
        for (int i = 0; i < 6; i++)
        {
            getline(file, temp);
        }

        for (int i = 0; i < 51; i++)
        {
            int x, y;
            file >> x >> x >> y;
            vector<int> temp;
            temp.push_back(x);
            temp.push_back(y);
            ACO::coordination.push_back(temp);
        }

        for (int i = 0; i < ACO::coordination.size(); i++)
        {
            for (int j = 0; j < ACO::coordination.size(); j++)
            {
                double distance = sqrt(
                    pow(ACO::coordination[j][0] - ACO::coordination[i][0], 2) + pow(ACO::coordination[j][1] - ACO::coordination[i][1], 2));
                ACO::dis_matrix[i][j] = distance;
                ACO::inverse_dis[i][j] = 1 / distance;
            }
        }

        int runtime = 3, iteration = 1250, popsize = 20;
        /*
        runtime = std::stoi(argv[1]);
        iteration = std::stoi(argv[2]);
        popsize = std::stoi(argv[3]);
        */

        cout << "runtimes : " << runtime << endl;
        cout << "iteration : " << iteration << endl;
        cout << "popsize : " << popsize << endl;
        ACO::ACO(runtime, iteration, popsize);
        //mySystemPause();
        return 0;
    }